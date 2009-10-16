 /*****************************************************************************
 *
 * Firmware upgrader
 * 
 * Gaurav Gupta <gaurav@cc.gatech.edu>, Keith O'Hara <keith.ohara@gatech.edu>
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/


#include "firmware_upgrade.h"
#include "fluke.h"
#include "uart.h"
#include "ov7649.h"

extern unsigned char img[HEIGHT_SIZE*WIDTH_SIZE] __attribute__((aligned(4)));

static unsigned long iap_command[5];
static unsigned long iap_result[2];
static unsigned int numEEPROMsegments = 0;

__attribute__ ((section (".data"), naked, long_call))
void iapCall (void)
{
  register void *pFP0 asm("r0") = iap_command;
  register void *pFP1 asm("r1") = iap_result;

  asm volatile(" bx  %[iapLocation]"
               :
               : "r" (pFP0), "r" (pFP1), [iapLocation] "r" (IAP_LOCATION) );
}

void uf_writeEEPROMPage(char page)
{
	char segment = 0;

	while(segment < 264/UF_SEGMENT_SIZE)
	{
		int counter;
		char sum = 0, chksum = 0;
		for(counter=0;counter<UF_SEGMENT_SIZE;counter++)
		{
			char recv = uart1GetchBlockRTS();
			img[segment*UF_SEGMENT_SIZE + counter] = recv;
	 		sum += recv;
		}
 		chksum = uart1GetchBlockRTS();
		if(chksum == sum)
		{
			segment ++;
	 		uart1PutchCTS(UF_SUCCESS);
		}
 		else
	 		uart1PutchCTS(UF_ERROR);
	}

	erase_mem(page);
	write_mem(page,0,img,264);
}

void uf_readEEPROMPage(char page)
{
	char segment = 0;
	read_mem(page,0,img,264);

	while(segment < 264/UF_SEGMENT_SIZE)
	{
		int counter;
		char sum = 0, result = 0;
		for(counter=0;counter<UF_SEGMENT_SIZE;counter++)
		{
			char recv = img[segment*UF_SEGMENT_SIZE + counter];
			uart1PutchCTS(recv);
	 		sum += recv;
		}
		uart1PutchCTS(sum);
 		result = uart1GetchBlockRTS();
		if(result == UF_SUCCESS)
			segment ++;
	}
}

void uf_saveEEPROMdump(void)
{
	int i;
	for(i=0;i<512;i++)
		uf_readEEPROMPage(i); 
}

void uf_restoreEEPROMdump(void)
{
	int i;
	for(i=0;i<512;i++)
		uf_writeEEPROMPage(i); 
}

void uf_storeinEEPROM(void)
{
	unsigned char recv = uart1GetchBlockRTS();
	numEEPROMsegments = recv;
	numEEPROMsegments = numEEPROMsegments << 8;
	recv = uart1GetchBlockRTS();
	numEEPROMsegments = numEEPROMsegments | recv;

	for(recv=0;recv<numEEPROMsegments;recv++)
		uf_writeEEPROMPage(recv);
}

__attribute__ ((section (".data"), long_call)) 
void ram_uf_loadFlash(void)
{
	//load in 8k byte segments = 31 eeprom segments + 8 bytes
	int byteindex=0,eepromSegIndex=0,oldFlashSegIndex = 0;
	int bytesread = 0,i=0;
	while(eepromSegIndex < numEEPROMsegments)
	{
		while(bytesread < 8192 && eepromSegIndex < 512)
		{
			read_mem(eepromSegIndex++,byteindex,img + bytesread,264-byteindex);
			bytesread += 264 - byteindex;
			byteindex = 0;
		}
		if(bytesread > 8192)
		{
			byteindex = 264 - (bytesread - 8192);
 			eepromSegIndex--;
		}
		
		int chksum = 0;
		for(i=0;i<8192;i++)
			chksum += img[i];

		unsigned int tempVec = VICIntEnable;
		VICIntEnClr = 0xffffffff; 

		// do IAP stuff here
		// Prepare the sectors
		iap_command[0] = 50;
		iap_command[1] = oldFlashSegIndex;
		iap_command[2] = oldFlashSegIndex;
		iapCall();

		// do IAP stuff here
		// Erase the sectors
		iap_command[0] = 52;
		iap_command[1] = oldFlashSegIndex;
		iap_command[2] = oldFlashSegIndex;
		iap_command[3] = 60000;
		iapCall();

		// do IAP stuff here
		// Prepare the sectors
		iap_command[0] = 50;
		iap_command[1] = oldFlashSegIndex;
		iap_command[2] = oldFlashSegIndex;
		iapCall();

		//IOCLR=LED;
		// Write RAM to flash
		iap_command[0] = 51;
		iap_command[1] = oldFlashSegIndex * 8192;
		iap_command[2] = (int)img;
		iap_command[3] = 8192;
		iap_command[4] = 60000;
		iapCall();

		// Compare RAM to flash
		iap_command[0] = 56;
		iap_command[1] = oldFlashSegIndex * 8192;
		iap_command[2] = (int)img;
		iap_command[3] = 8192;
		iapCall();

		VICIntEnable = tempVec;

		oldFlashSegIndex++;
		bytesread = 0;
	}

	//reset
	WDTC = 0x00FFF; 
	WDMOD = 0x03; 
	WDFEED= 0xAA;
	WDFEED= 0x55;
}

