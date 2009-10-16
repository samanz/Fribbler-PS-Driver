#ifndef WINFILE
#define WINFILE
#include "data.h"
#include <windows.h>

unsigned int	sleep(unsigned int);

unsigned int	usleep(unsigned long);

class	FileOut : public DataSink
{
  private:
	HANDLE	mFh;
	char	*mFileName;
  public:
	FileOut(char const fileName[]);
	virtual ~FileOut();

	virtual	int	takeChar(unsigned char);
	virtual int	takeBlock(char *, unsigned long);
	virtual void	flushOutput();
};

class	FileIn : public DataSource
{
  private:
	HANDLE	mFh;
	char	*mFileName;
  public:
	FileIn(char const fileName[]);
	virtual ~FileIn();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
	virtual void	flushInput();
};
Data	*loadNamedData(char const name[], int mode = 0);

#endif
