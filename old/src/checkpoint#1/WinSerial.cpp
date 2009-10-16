#include "WinSerial.h"
#include <stdio.h>
#include <windows.h>
#include <assert.h>

extern 	int	gDebugging;
const	int	thisModule = 0x0002;

extern  void 	log(char const str[]);
extern	char	logchunk[512];


WinSerial::WinSerial(char const devName[])
{
	mSetup = 0;
	if (devName != NULL)
	{
		mDevName = new char[strlen(devName) + 1];
		strcpy(mDevName, devName);
	}
	else
	{
		char	*ret;

		ret = getenv("ROBOT_ADDRESS");
		if (ret == NULL)
		{
			log("ADDRESS is NULL and no ROBOT_ADDRESS\n");
			exit(-1);
		}
		mDevName = new char[strlen(ret) + 1];
		strcpy(mDevName, ret);
	}
        mPortFh = 0;}
WinSerial::~WinSerial()
{
        if (mPortFh != 0)
		return;
	CloseHandle(mPortFh);
}	
void    WinSerial::setupLink()
{

        int     ret;
        DCB     dcb = {0};
        COMMTIMEOUTS cto = {10,10,10,10,10};

        dcb.DCBlength = sizeof(DCB);

        cto.ReadIntervalTimeout = 100;

sprintf(logchunk, "opening com port <%s>\n", mDevName);
log(logchunk);
        mPortFh = CreateFile(mDevName, GENERIC_READ | GENERIC_WRITE,
                                         0,
                                         0,
                                         OPEN_EXISTING, 
                                         0,
                                         0
                                         );

   	if (mPortFh == INVALID_HANDLE_VALUE) 
   	{
       		// Handle the error.
       		printf ("CreateFile failed with error %ld.\n", GetLastError());
       		exit(1);
   	}

sprintf(logchunk, " port %d\n", mPortFh);
log(logchunk);
        assert(mPortFh);
        ret = GetCommState(mPortFh, &dcb);

        if (!ret)
        {
                fprintf(stderr, "GetCommState failed %d\n",
                                 GetLastError());
                exit(2);
        } 
 //       dcb.BaudRate = CBR_57600;
        dcb.BaudRate = CBR_256000;
    	dcb.ByteSize    = 8;
    	dcb.StopBits    = ONESTOPBIT;
    	dcb.Parity      = NOPARITY;
    	dcb.fDtrControl = DTR_CONTROL_DISABLE;
    	dcb.fOutX       = FALSE;
    	dcb.fInX        = FALSE;
    	dcb.fNull       = FALSE;
    	dcb.fRtsControl = RTS_CONTROL_DISABLE;

// need to send binary data
	dcb.fBinary = 1;

//    *  fInX, fOutX,fOutXDsrFlow, fOutXCtsFlow are set to FALSE
//    * fDtrControl is set to DTR_CONTROL_ENABLE
//    * fRtsControl is set to RTS_CONTROL_ENABLE

   
  

        ret = SetCommState(mPortFh, &dcb);
        if (!ret)
        {
                fprintf(stderr, "SetCommState failed %d\n",
                                GetLastError());
                exit(3);
        }

        ret = SetCommTimeouts(mPortFh, &cto);
        if (!ret)
        {
                fprintf(stderr, "SetCommTimeouts failed %d\n",
                                GetLastError());
                exit(3);
        }

	log("setup\n");
	mSetup = 1;
} 
int     WinSerial::takeChar(unsigned char ch)
{
	int	ret;
        unsigned long   bytesWritten = 0;

	if (!mSetup)
		setupLink();
	ret = WriteFile(mPortFh, &ch, 1, &bytesWritten, 0);
	if (bytesWritten != 1)
		return 0;
	if (gDebugging & thisModule)
		fprintf(stderr, "out(%x)\n", ch);

	return 1;
}
int     WinSerial::getChar(unsigned char &ch)
{
	int	ret;
        unsigned long    bytesRead = 0;

	if (!mSetup)
		setupLink();

	ret = ReadFile(mPortFh, &ch, 1, &bytesRead, 0);
	if (bytesRead != 1)
		return false;
	if (gDebugging & thisModule)
		fprintf(stderr, "in(%x)\n", ch);
	return true;
}
int     WinSerial::getBlock(char *buf, unsigned long size)
{
	int		ret;
	unsigned long	count; 
        unsigned long    bytesRead;
	int		emptyReads = 0;

	if (!mSetup)
		setupLink();

	for (count = 0;count < size;count += bytesRead)
	{
		ret = ReadFile(mPortFh, &buf[count], 64, &bytesRead, 0);
		/* to avoid hanging here if we loose contact	*/
		if (bytesRead > 0)
			emptyReads = 0;
		else
			emptyReads++;
		if (emptyReads > 10)
		{
			if (gDebugging & thisModule)
				fprintf(stderr, "short read %ld\n", size - count);
			printf("throwing DataError\n");
			throw DataError(1);
		}
	}
	return 1;
}
int     WinSerial::takeBlock(char *buf, unsigned long size)
{
	int	ret;
	unsigned long	bytesWritten;

	if (!mSetup)
		setupLink();

	ret = WriteFile(mPortFh, buf, size, &bytesWritten, 0);
	if (bytesWritten != size)
		return 0;

	return 1;
}
void    WinSerial::flushOutput()
{
	if (!mSetup)
		setupLink();
}
void    WinSerial::flushInput()
{
	unsigned char	ch;

	if (!mSetup)
		setupLink();

	while (getChar(ch))
		;
}

int	WinSerial::getStatus()
{
	return 1;
}
void	WinSerial::waitForConnection()
{
}
