/************************************************************************/
/* This was developed by John Cummins at Brooklyn College, with 	*/
/* assistance from M. Q. Azhar and Howard of Surveyor.com, and		*/
/* supervision from Professor Sklar.					*/
/*									*/
/* It is released under the copyleft understanding. That is, any one is	*/
/* free to use, and modify, any part of it so long as it continues to 	*/
/* carry this notice.							*/
/************************************************************************/
/************************************************************************/
/* posixSoc.cpp								*/
/* The OS dependent function for POSIX conforming OS's using sockets	*/
/*									*/
/* usage: linked in to executable so it works on UNIX like OS		*/
/* 									*/
/* 									*/
/************************************************************************/
#include "posixSoc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


PosixSock::PosixSock(char *addr)
{
	char	*s;

	if (!addr)
		addr = getenv("SVR_ADDRESS");
	if (!addr)
	{
		printf("Please export SVR_ADDRESS environment variable\n");
		exit(1);
	}
	strcpy(mDevName, addr);
	for (s = mDevName;*s != '\0';s++)
	{
		if (*s == ':')
		{
			*s++ = '\0';
			break;
		}
	}
	mPortNum = atoi(s);		
		
	mSocket = -1;
	inCount = inPosn = 0;
	outCount = 0;
	openRobotLink();
}
PosixSock::~PosixSock()
{
	if (mSocket < 0)
		return;
	close(mSocket);
}	
void	PosixSock::openRobotLink()
{
	struct	sockaddr_in remote_addr;
	socklen_t	sin_size;


	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(mDevName);
	remote_addr.sin_port = htons(mPortNum);


	if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return;
	}

	sin_size = sizeof(struct sockaddr_in);
	if (connect(mSocket, (struct sockaddr *)&remote_addr, sin_size) < 0)
	{
		perror("connect");
		return;
	}

	printf("connected to server\n");
} 
int	PosixSock::writeOneChar(char *ch)
{
	outMsg[outCount++] = *ch;
	return 1;
}
int	PosixSock::readOneChar(char *ch)
{
	int	ret;

	if (inPosn == inCount)
	{
		ret = recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
		if (ret <= 0)
		{
			usleep(100000);
			ret = recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
			if (ret <= 0)
			{
				usleep(200000);
				ret = recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
			}
		}
		inCount = 0;
		if (ret > 0)
		{
			inCount = ret;
			inMsg[inCount] = '\0';
		}
		inPosn = 0;
	}
	if (inPosn == inCount)
		return false;
	*ch = inMsg[inPosn++];
	return true;
}
int	PosixSock::readManyChars(char *buf, int size)
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
int	PosixSock::saveNamedData(char *name, 
				     char *data, int size)
{
	int	fd;

	fd = open(name, O_TRUNC | O_CREAT | O_WRONLY, 00644);
	if (fd < 0)
		return false;
	

	if (write(fd, data, size) != size)
	{
		close(fd);
		return false;
	}
	close(fd);

	return true;
}
void	PosixSock::flushOutputBuffer()
{
	int	ret;

	ret = ::send(mSocket, outMsg, outCount, 0);
	
	outCount = 0;
}
void	PosixSock::flushInputBuffer()
{
	int	ret;

	ret = recv(mSocket, inMsg, sizeof(inMsg), MSG_DONTWAIT);
	if (ret != -1)
	{
		inMsg[ret] = '\0';
		printf("dumping %d <%s>\n", ret, inMsg);
	}
	inPosn = inCount = 0;
}


	
