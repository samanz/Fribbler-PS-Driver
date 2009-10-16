#ifndef POSIXFILE
#define POSIXFILE
#include "data.h"
#include <unistd.h>

class	FileOut : public DataSink
{
  private:
	int	mFd;
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
	int	mFd;
	char	*mFileName;
  public:
	FileIn(char const fileName[]);
	virtual ~FileIn();

	virtual int	getChar(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long);
	virtual void	flushInput();
};

Data	*loadNamedData(char const fileName[], int mode = 0);


#endif
