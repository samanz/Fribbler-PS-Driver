#ifndef SOCSERIAL
#define SOCSERIAL

#include "data.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>


class SocSerial : public Serial
{
  protected:
	int	mStatus;
	int	mSocId;
	char	mDevName[32];
	int	mPortNum;

	int	mSocket;

 	sockaddr_in mAddr;


	char	inMsg[10240];
	int	inCount;
	int	inPosn;

	char	outMsg[512];
	int	outCount;
	int	mSetup;

  public:
	SocSerial(char const []);
	virtual ~SocSerial();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
		void	flushInput();


	virtual	int	takeChar(unsigned char);
	virtual int	takeBlock(char *, unsigned long);
		void	flushOutput();


	virtual void    setupLink()	= 0;
	virtual int	getStatus()	{ return mStatus; }
};

class ClientSerial : public SocSerial
{
  public:
	ClientSerial(char const addr[]) : SocSerial(addr)	{}
	virtual ~ClientSerial();

	virtual	void	setupLink();
};

class ServerSerial : public SocSerial
{
  private:
	struct sockaddr_un sa;
	socklen_t	addrLen;

  public:
	ServerSerial(char const addr[]) : SocSerial(addr) 	{}
	virtual ~ServerSerial();

	virtual void	setupLink();
		void	waitForConnection();
};

	
#endif
