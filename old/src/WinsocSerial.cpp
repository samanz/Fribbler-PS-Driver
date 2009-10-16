#include "WinsocSerial.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

extern 	int	gDebugging;
const	int	thisModule = 0x0002;

extern	void	log(char const []);
extern char	logchunk[256];

WinsocSerial::WinsocSerial(char const devName[])
{
	mSetup = 0;
	if (devName)
	{
		mDevName = new char[strlen(devName) + 1];
		strcpy(mDevName, devName);
	}
	else
	{
		mDevName = new char[32];
		strcpy(mDevName, "localsocket");
	}
	mPortId = 8888;
        mSocket = 0;
	mStatus = 0;
}
WinsocSerial::~WinsocSerial()
{
// Next two lines commented out as 0 is a valid socket id on Window
//        if (mSocket != 0)
//		return;
	closesocket(mSocket);
}

int     WinsocSerial::takeChar(unsigned char ch)
{
//printf("takeChar(%x)\n", ch);
	if (!mSetup)
		setupLink();

	outMsg[outCount++] = ch;
	return 1;
}
int     WinsocSerial::getChar(unsigned char &ch)
{
	int	ret;

	if (!mSetup)
		setupLink();


	if (inPosn == inCount)
	{

		ret = recv(mSocket, inMsg, sizeof(inMsg), 0);
		if (ret == 0)
		{
			log("broken socket 1\n");
			mStatus = 0;
			return false;
		}
//		if (ret == 0)
//		{
//                        Sleep(1000);
//                        ret = recv(mSocket, inMsg, sizeof(inMsg), 0);
//		}
		inCount = 0;
		if (ret > -1)
		{
			inCount = ret;
			inMsg[inCount] = '\0';
//			printf("msg <%s>\n", inMsg);
		}
		inPosn = 0;
	}
	if (inPosn == inCount)
		return false;
	ch = inMsg[inPosn++];
	return true;}
int     WinsocSerial::getBlock(char *buf, unsigned long size)
{
	long	count; 
	long	bytesRead;
	int	emptyReads = 0;


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
		bytesRead = recv(mSocket, &buf[count], size - count, 0);
		if (bytesRead == 0)
		{
			log("broken socket 2\n");
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
int     WinsocSerial::takeBlock(char *buf, unsigned long size)
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
void    WinsocSerial::flushOutput()
{
	int	ret;

	if (!mSetup)
		setupLink();

	ret = ::send(mSocket, outMsg, outCount, 0);
	
	outCount = 0;
}
void    WinsocSerial::flushInput()
{
	int	ret;

// not sure if SO_DONTLINGER is what we need
        ret = recv(mSocket, inMsg, sizeof(inMsg), SO_DONTLINGER);
	if (ret == 0)
	{
		log("broken socket 3\n");
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
void    ClientSerial::setupLink()
{
	int	ret;
	WSADATA	data = { 0 };
        WORD    ver  = MAKEWORD(2, 2);

log("ClientSerial\n");
	ret = WSAStartup(ver, &data);

	if (ret == SOCKET_ERROR)
	{
		printf("Failed to init WinSock lib\n");
		exit(-1);
	}

	sockaddr_in 	remote_addr;
        int             sin_size;

	struct hostent *host;
	char	name[32];
	gethostname(name, sizeof(name));
sprintf(logchunk, "local name = <%s>\n", name);
log(logchunk);
	host = gethostbyname(name);
sprintf(logchunk, "h_addr = <%s>\n", host->h_addr);
log(logchunk);

	memset(&remote_addr, 0, sizeof(remote_addr));
	
	remote_addr.sin_family = host->h_addrtype;;
	memcpy((char *)&remote_addr.sin_addr, host->h_addr, host->h_length);
//	remote_addr.sin_addr.s_addr = inet_addr(mDevName);
//	remote_addr.sin_addr.s_addr = inet_addr(host->h_name);	remote_addr.sin_port = htons(mPortId);

	mSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mSocket == INVALID_SOCKET)
	{
		log("socket failed");
		return;
	}

	sin_size = sizeof(sockaddr_in);

	ret = connect(mSocket, (sockaddr *)&remote_addr, sin_size);

	if (ret == SOCKET_ERROR)
	{

		log("connect failed for client\n");

		sprintf(logchunk, "connect failed %d\n", WSAGetLastError());
		log(logchunk);
		return;
	}

	mSetup = 1;
	mStatus = 1;
}
void    ServerSerial::setupLink()
{
	int	soc;
	int	ret;
	WSADATA	data = { 0 };
        WORD    ver  = MAKEWORD(2, 2);

	log("SerialServer\n");
	unlink(mDevName);

	ret = WSAStartup(ver, &data);

	if (ret == SOCKET_ERROR)
	{
		log("Failed to init WinSock lib\n");
		exit(-1);
	}


	struct hostent *host;
	char	name[32];
	gethostname(name, sizeof(name));
sprintf(logchunk, "local name = <%s>\n", name);
log(logchunk);
	host = gethostbyname(name);

	memset(&remote_addr, 0, sizeof(remote_addr));
	memcpy((char *)&remote_addr.sin_addr, host->h_addr, host->h_length);
	remote_addr.sin_family = host->h_addrtype;;
//	remote_addr.sin_addr.s_addr = inet_addr(mDevName);
	remote_addr.sin_port = htons(mPortId);

	mSocId = socket(AF_INET, SOCK_STREAM, 0);

	if (mSocId == INVALID_SOCKET)
	{
		log("socket failed");
		return;
	}


	sin_size = sizeof(sockaddr_in);
	if (bind(mSocId, (sockaddr *)&remote_addr, sin_size) != 0)
	{
		log("bind failed");
		return;
	}
	waitForConnection();
	mSetup = 1;
	log("set up\n");
}
void	ServerSerial::waitForConnection()
{
	log("listen\n");
	if (listen(mSocId, 5) != 0)
	{
		log("listen failed");
		return;
	}	
	log("accept\n");
	mSocket = accept(mSocId,(sockaddr *)&remote_addr, &sin_size);
	if (mSocket < 0)
	{
		log("accept failed\n"); 
		return;
	}
	log("connected\n");

	mStatus = 1;
}



	
