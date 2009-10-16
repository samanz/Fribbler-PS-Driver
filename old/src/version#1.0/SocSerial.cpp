
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "SocSerial.h"

extern int      gDebugging;
const int       thisModule = 0x0002;

extern void	log(char const []);
extern char	logchunk[512];



SocSerial::SocSerial(char const addr[])
{
	strcpy(mDevName, addr);

	mPortNum = 101;		
		
	mSocket = -1;
	inCount = inPosn = 0;
	outCount = 0;
	mSetup = 0;
	mStatus = 0;

}
SocSerial::~SocSerial()
{
	if (mSocket < 0)
		return;
	close(mSocket);
}	


int     SocSerial::takeChar(unsigned char ch)
{
	if (!mSetup)
		setupLink();

	if (gDebugging & thisModule)
	{
		sprintf(logchunk, "send(%x) %c\n", ch, ch);
		log(logchunk);
	}
	outMsg[outCount++] = ch;
	return 1;

}
int     SocSerial::getChar(unsigned char &ch)
{
	int	ret;

	if (!mSetup)
		setupLink();

	if (inPosn == inCount)
	{
		ret = ::recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
		if (ret == 0)
		{
			mStatus = 0;
			return false;
		}


		inCount = 0;
		if (ret > 0)
		{
			inCount = ret;
			if (gDebugging & thisModule)
			{
				sprintf(logchunk, "Message in %d bytes\n", inCount);
				log(logchunk);
			}
		}
		inPosn = 0;
	}
	if (inPosn == inCount)
		return false;
	ch = inMsg[inPosn++];
	if (gDebugging & thisModule)
	{
		sprintf(logchunk, "get(%x) %c\n", ch, ch);
		log(logchunk);
	}
	return true;
}
int     SocSerial::getBlock(char *buf, unsigned long size)
{
	unsigned long	count; 
	int	bytesRead;
	int	emptyReads = 0;

	if (!mSetup)
		setupLink();


	count = inCount - inPosn;
	if (count >= size)
	{
		memcpy(buf, &inMsg[inPosn], size);
		inPosn += size;
		return 1;
	}
	memcpy(buf, &inMsg[inPosn], count);
	inCount = inPosn = 0;
	for (;count < size;count += bytesRead)
	{
		bytesRead = ::recv(mSocket, &buf[count], size - count, 0);
		if (bytesRead == 0)
		{
			mStatus = 0;
			return 0;
		}
		if (bytesRead < 0)
			bytesRead = 0;
		
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
int     SocSerial::takeBlock(char *buf, unsigned long size)
{
	char	*p;
	unsigned long	count;

	if (!mSetup)
		setupLink();

	flushOutput();

	p = buf;
	for (count = 0; count + 64 < size;count += 64)
	{
		memcpy(outMsg, p, 64);
		outCount = 64;
		flushOutput();
		p += 64;
	}
	outCount = size - count;
	memcpy(outMsg, p, outCount);

	if (outCount)
		flushOutput();	
	
	return 1;
}

void    SocSerial::flushOutput()
{
	if (!mSetup)
		setupLink();

	int	ret;

	if (outCount == 0)
		return;

	outMsg[outCount] = '\0';
	if (gDebugging & thisModule)
	{
		sprintf(logchunk, "msg => %d<%s>\n", outCount, outMsg);
		log(logchunk);
	}
	ret = ::send(mSocket, outMsg, outCount, 0);
	
	outCount = 0;
}
void    SocSerial::flushInput()
{
	int	ret;

	if (!mSetup)
		setupLink();

	ret = ::recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
	if (ret == 0)
	{
		mStatus = 0;
		return;
	}
	if (ret != -1)
	{
		inMsg[ret] = '\0';
		printf("dumping %d <%s>\n", ret, inMsg);
	}
	inPosn = inCount = 0;

}
ClientSerial::~ClientSerial()
{
}
void    ClientSerial::setupLink()
{
	struct sockaddr_un	address;
	size_t 	len;
	int	ret;

	mSocket = socket(PF_UNIX, SOCK_STREAM, 0);
	if (mSocket < 0)
	{
		log("bad Socket\n");
		exit(1);
	} 

	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, mDevName);

	len = sizeof(address.sun_family) + strlen(address.sun_path) + 1;

	ret = connect(mSocket, (struct sockaddr *)&address, len);
	if (ret < 0)
	{
		sprintf(logchunk, "bad Connect <%s>\n", mDevName);
		log(logchunk);
		exit(2);
	} 
	if (gDebugging & thisModule)
		log("We are connected!\n");
	
  
	mSetup = 1;
	mStatus = 1;
}
ServerSerial::~ServerSerial()
{
	unlink(mDevName);
}
void    ServerSerial::setupLink()
{


	unlink(mDevName);


	mSocId = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (mSocId < 0)
	{
		sprintf(logchunk, "Bad Sock\n");
		log(logchunk);
		exit(1);
	}

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, mDevName);

	addrLen = sizeof(sa.sun_family) + strlen(sa.sun_path) + 1;

	int	ret;

	ret = bind(mSocId, (struct sockaddr *)&sa, addrLen);
	if (ret < 0)
	{
		sprintf(logchunk, "Bad Bind <%s>\n", mDevName);
		log(logchunk);
		exit(1);
	}
	waitForConnection();

	mSetup = 1;


}
void	ServerSerial::waitForConnection()
{
	int	ret;

	ret = listen(mSocId, 5);
	if (ret < 0)
	{
		log("Bad Listen\n");
		exit(1);
	}


	mSocket = accept(mSocId, (struct sockaddr *)&sa, &addrLen);
	if (mSocket < 0)
	{
		log("Bad accept\n");
		exit(1);
	}
	if (gDebugging & thisModule)
		log("We have a Client!\n");



	mStatus = 1;
}

