#ifndef WINSOCSERIAL
#define WINSOCSERIAL

#include "data.h"
#include <windows.h>

class WinsocSerial : public Serial
{
  protected:
	int	mSocId;
	char	*mDevName;
	int	mPortId;

	SOCKET	mSocket;

	char	inMsg[10240];
	int	inCount;
	int	inPosn;

	char	outMsg[512];
	int	outCount;

	int	mSetup;
	int	mStatus;
  public:
	WinsocSerial(char const []);
	virtual ~WinsocSerial();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
		void	flushInput();


	virtual	int	takeChar(unsigned char);
	virtual int	takeBlock(char *, unsigned long);
		void	flushOutput();

	virtual void	setupLink() = 0;
		int	getStatus()	{ return mStatus; }
};
class ClientSerial : public WinsocSerial
{
  public:
	ClientSerial(char const addr[]) : WinsocSerial(addr)	{}
	virtual ~ClientSerial() {}

	virtual	void	setupLink();
};

class ServerSerial : public WinsocSerial
{
	sockaddr_in 	remote_addr;
        int             sin_size;

  public:
	ServerSerial(char const addr[]) : WinsocSerial(addr) 	{}
	virtual ~ServerSerial() {}

	virtual void	setupLink();
		void	waitForConnection();
};
#endif
