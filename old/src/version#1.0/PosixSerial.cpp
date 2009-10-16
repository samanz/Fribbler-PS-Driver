#include "PosixSerial.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

extern int      gDebugging;
const int       thisModule = 0x0002;



PosixSerial::PosixSerial(char const devName[])
{
	mSetup = 0;
	mDevName = new char[strlen(devName) + 1];
	strcpy(mDevName, devName);
        mPortFd = 0;}
PosixSerial::~PosixSerial()
{
        if (mPortFd != 0)
		return;
	close(mPortFd);
}	
void    PosixSerial::setupLink()
{
	struct termios attr;
	
	/* make it raw */				
    	if ((mPortFd = open(mDevName, O_RDWR | O_NOCTTY | O_NONBLOCK, 0)) < 0)
   	{
        	fprintf(stderr, "Unable to open PosixSerial port '%s': %s\n",
				mDevName, strerror(errno));
        	exit(2);
    	}

    	if (tcgetattr(mPortFd, &attr) < 0 )
    	{
        	fprintf(stderr, "Call to tcgetattr failed: %s\n",
							 strerror(errno));
       		 exit(3);
    	}

    	attr.c_iflag = IGNPAR | IGNBRK; // | IXON | IXOFF;
    	attr.c_oflag = 0;
    	attr.c_cflag = CLOCAL | CREAD | CS8;
    	attr.c_lflag = 0;
    	attr.c_cc[ VTIME ] = 1; // timeout in tenths of a second
    	attr.c_cc[ VMIN ] = 0;  // allow read to return 0 characters

	attr.c_ispeed = attr.c_ospeed = B115200;


    	if (tcsetattr(mPortFd, TCSAFLUSH, &attr) < 0)
    	{
        	fprintf(stderr, "Call to tcsetattr failed: %s\n",
					 strerror(errno));
        	exit(6);
    	}
	/* o.k. now let's make it blocking */
	fcntl(mPortFd, F_SETFL, fcntl(mPortFd, F_GETFL, 0) & ~O_NONBLOCK);

	mSetup = 1;
}
 

int     PosixSerial::takeChar(unsigned char ch)
{
	int	ret;

	if (!mSetup)
		setupLink();

	if ((ret = write(mPortFd, &ch, 1)) != 1)
	{
		if (gDebugging & thisModule)
			printf("write returned %d %s\n", ret, strerror( errno ) );
		return 0;
	}
	if (gDebugging & thisModule)
	  	printf("out(%x)\n", ch);
	return 1;
}
int     PosixSerial::getChar(unsigned char &ch)
{
	int	ret;

	if (!mSetup)
		setupLink();

	ret = read(mPortFd, &ch, 1);
	if (ret != 1)
		return false;
	if (gDebugging & thisModule)
		printf("in(%x)\n", ch);
	return true;
}
int     PosixSerial::getBlock(char *buf, unsigned long size)
{
	unsigned long	count; 
	int	bytesRead;
	int	emptyReads = 0;

	if (!mSetup)
		setupLink();

	for (count = 0;count < size;count += bytesRead)
	{
		bytesRead = read( mPortFd, &buf[count], 64);
		/* to avoid hanging here if we loose contact	*/
		if (bytesRead > 0)
			emptyReads = 0;
		else
			emptyReads++;
		if (emptyReads > 10)
			return 0;
	}
	return 1;
}
int     PosixSerial::takeBlock(char *buf, unsigned long size)
{
	unsigned long	ret;

	if (!mSetup)
		setupLink();

	ret = write(mPortFd, buf, size);
	if (ret != size)
		return 0;

	return 1;
}
void    PosixSerial::flushOutput()
{
	if (!mSetup)
		setupLink();
}
void    PosixSerial::flushInput()
{
	unsigned char	ch;

	if (!mSetup)
		setupLink();

	while (getChar(ch))
		;
}

