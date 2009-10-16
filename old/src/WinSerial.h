#ifndef WINSERIAL
#define WINSERIAL

#include "data.h"
#include <windows.h>

class WinSerial : public Serial
{
  private:
	int	mSetup;
	char	*mDevName;
	HANDLE 	mPortFh;

	void    setupLink();
  public:
	WinSerial(char const []);
	virtual ~WinSerial();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
		void	flushInput();


	virtual	int	takeChar(unsigned char);
	virtual int	takeBlock(char *, unsigned long);
		void	flushOutput();

	int	getStatus();
	void	waitForConnection();
};
#endif
