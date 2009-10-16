 /*****************************************************************************
 *
 * Firmware upgrade header file
 * 
 * Gaurav Gupta <gaurav@cc.gatech.edu>, Keith O'Hara <keith.ohara@gatech.edu>
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#ifndef __FIRMWARE_UPGRADE__
#define __FIRMWARE_UPGRADE__

#define UF_SUCCESS 42
#define UF_ERROR 1
#define UF_SEGMENT_SIZE 132
#define IAP_LOCATION 0x7ffffff1

typedef void (*IAP)(unsigned long[],unsigned long[]);

__attribute__ ((section (".data"), naked, long_call))  
void iapCall (void);

void uf_writeEEPROMPage(char page);
void uf_readEEPROMPage(char page);
void uf_saveEEPROMdump(void);
void uf_restoreEEPROMdump(void);
void uf_storeinEEPROM(void);
__attribute__ ((section (".data"), long_call))  void ram_uf_loadFlash(void);


#endif /* __FIRMWARE_UPGRADE__ */
