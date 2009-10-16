/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* assistance from M. Q. Azhar and Howard of Surveyor.com, and		*/
/* supervision from Professor Sklar.					*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
/********************************************************************************/
/* posixSoc.cpp									*/ 
/* The OS dependent function for POSIX conforming OS's using sockets		*/
/*										*/
/* usage: linked in to executable so it works on UNIX like OS			*/
/* 										*/
/* 										*/
/********************************************************************************/
#include "winSoc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>

void	usleep(long t)
{
	Sleep(t / 1000);
}
void	sleep(int t)
{
	Sleep(t * 1000);
}




WinSock::WinSock(char *param) 
{
	char	*s;

	mPortId = 10001;
	if (!param)
		param = getenv("SVR_ADDRESS");
	if (!param)
	{
		printf("Please set SVR_ADDRESS environment variable\n");
		exit(1);
	}
	mDevName = new char[strlen(param) + 1];
	strcpy(mDevName, param);
	for (s = mDevName;*s != '\0';s++)
	{
		if (*s == ':')
		{
			*s++ = '\0';
			mPortId = atoi(s);
			break;
		}
	}
	mSocket = 0;
	inCount = inPosn = 0;
	outCount = 0;
	openRobotLink();
}
WinSock::~WinSock()
{
	if (mSocket < 0)
		return;
	close(mSocket);
}
// We take a client role	
void	WinSock::openRobotLink()
{
	int	ret;
	WSADATA	data = { 0 };
        WORD    ver  = MAKEWORD(2, 2);

	ret = WSAStartup(ver, &data);

	if (ret == SOCKET_ERROR)
	{
		printf("Failed to init WinSock lib\n");
		exit(-1);
	}

	sockaddr_in 	remote_addr;
        int             sin_size;


	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(mDevName);
	remote_addr.sin_port = htons(mPortId);

	if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return;
	}

	sin_size = sizeof(sockaddr_in);
	if (connect(mSocket, (sockaddr *)&remote_addr, sin_size) < 0)
	{
		perror("connect");
		return;
	}
} 
int	WinSock::writeOneChar(char *ch)
{
	outMsg[outCount++] = *ch;
	return 1;
}
int	WinSock::readOneChar(char *ch)
{
	int	ret;

	if (inPosn == inCount)
	{
		ret = recv(mSocket, inMsg, sizeof(inMsg), 0);
		if (ret == 0)
		{
                        usleep(100000);
                        ret = recv(mSocket, inMsg, sizeof(inMsg), 0);
		}
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
	*ch = inMsg[inPosn++];
	return true;
}
int	WinSock::readManyChars(char *buf, int size)
{
	int	count; 
	int	bytesRead;
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
int	WinSock::saveNamedData(char *name, 
				     char *data, int size)
{
        unsigned long    bytesWritten;
	HANDLE        fh;

        fh = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
                                         0,
                                         0,
                                         CREATE_ALWAYS, 
                                         0,
                                         0
                                         );

        WriteFile(fh, data, size, &bytesWritten, 0);

	CloseHandle(fh);

	if (bytesWritten == size)
		return true;
	return false;
}
void	WinSock::flushOutputBuffer()
{
	int	ret;

	ret = ::send(mSocket, outMsg, outCount, 0);
	
	outCount = 0;
}
void	WinSock::flushInputBuffer()
{
	int	ret;

// not sure if SO_DONTLINGER is what we need
        ret = recv(mSocket, inMsg, sizeof(inMsg), SO_DONTLINGER);
	if (ret != -1)
	{
		inMsg[ret] = '\0';
		printf("dumping %d <%s>\n", ret, inMsg);
	}
	inPosn = inCount = 0;
}


	
