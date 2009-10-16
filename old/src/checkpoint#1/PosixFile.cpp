#include "PosixFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>


FileOut::FileOut(char const fileName[]) 
{
	mFileName = new char [strlen(fileName) + 1];
	strcpy(mFileName, fileName);

        mFd = open(mFileName, O_CREAT | O_WRONLY, 00666);
}
FileOut::~FileOut()
{
	close(mFd);
	delete [] mFileName;
}

int 	FileOut::takeChar(unsigned char ch)
{
        if (write(mFd, &ch, 1) != 1)
		return 0;

	return 1;
}
int	FileOut::takeBlock(char *buf, unsigned long len)
{
        unsigned long    bytesWritten;

        bytesWritten = write(mFd, buf, len);

	if (bytesWritten != len)
		return 0;
	return 1;
}
void	FileOut::flushOutput()
{
}
FileIn::FileIn(char const fileName[]) 
{
	mFileName = new char [strlen(fileName) + 1];
	strcpy(mFileName, fileName);

        mFd = open(mFileName, O_RDONLY);
}
FileIn::~FileIn()
{
	close(mFd);
	delete [] mFileName;
}
int	FileIn::getChar(unsigned char &ch)
{
	unsigned long	bytesRead;

	bytesRead = read(mFd, &ch, 1);
	if (bytesRead != 1)
		return 0;
	return 1;
}
int	FileIn::getBlock(char *buf, unsigned long len)
{
	unsigned long	bytesRead;

	bytesRead = read(mFd, buf, len);
	if (bytesRead != len)
		return 0;
	return 1;
}
void	FileIn::flushInput()
{
}

Data	*loadNamedData(char const name[], int mode)
{
	FileIn	file(name);

	Data	*data = new Data(file, mode);

	if (!data->getSize())
	{
		delete data;
		return 0;
	}

	return data;
}

