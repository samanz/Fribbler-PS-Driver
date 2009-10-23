#ifndef POSIXSERIAL
#define POSIXSERIAL

#include "data.h"


class PosixSerial : public Serial
{
  private:
	int	mSetup;
	char	*mDevName;
	int 	mPortFd;

	void    setupLink();
  public:
	PosixSerial(char const []);
	virtual ~PosixSerial();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
		void	flushInput();


	virtual	int	takeChar(unsigned char);
	virtual int	takeBlock(char *, unsigned long);
		void	flushOutput();

	int	getStatus()		{ return 1; }
	void	waitForConnection() 	{}
};
#endif
