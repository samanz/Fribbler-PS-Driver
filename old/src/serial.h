#ifndef SERIAL
#define SERIAL

#include "data.h"
#include <windows.h>

class Serial : public DataSource, public DataSink
{
  private:
	int	mSetup;
	char	*mDevName;
	HANDLE 	mPortFh;

	void    setupLink();
  public:
	Serial(char *);
	virtual ~Serial();

	virtual int	getChar(char &);
	virtual int	getBlock(char *buf, long);
		void	flushInput();


	virtual	int	takeChar(char);
	virtual int	takeBlock(char *, long);
		void	flushOutput();
};
#endif
