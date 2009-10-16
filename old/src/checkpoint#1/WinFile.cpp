#include "WinFile.h"




FileOut::FileOut(char const fileName[]) 
{
	mFileName = new char [strlen(fileName) + 1];
	strcpy(mFileName, fileName);

        mFh = CreateFile(mFileName, GENERIC_READ | GENERIC_WRITE,
                                         0,
                                         0,
                                         CREATE_ALWAYS, 
                                         0,
                                         0
                                         );
}
FileOut::~FileOut()
{
	CloseHandle(mFh);
	delete [] mFileName;
}

int 	FileOut::takeChar(unsigned char ch)
{
        unsigned long    bytesWritten;

        WriteFile(mFh, &ch, 1, &bytesWritten, 0);

	if (bytesWritten != 1)
		return 0;
	return 1;

}
int	FileOut::takeBlock(char *buf, unsigned long len)
{
        unsigned long    bytesWritten;

        WriteFile(mFh, buf, len, &bytesWritten, 0);

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

        mFh = CreateFile(mFileName, GENERIC_READ | GENERIC_WRITE,
                                         0,
                                         0,
                                         OPEN_EXISTING, 
                                         0,
                                         0
                                         );
}
FileIn::~FileIn()
{
	CloseHandle(mFh);
	delete [] mFileName;
}
int	FileIn::getChar(unsigned char &ch)
{
	unsigned long	bytesRead;

	ReadFile(mFh, &ch, 1, &bytesRead, 0);
	if (bytesRead != 1)
		return 0;
	return 1;
}
int	FileIn::getBlock(char *buf, unsigned long len)
{
	unsigned long	bytesRead;

	ReadFile(mFh, buf, len, &bytesRead, 0);
	if (bytesRead != len)
		return 0;
	return 1;
}
void	FileIn::flushInput()
{
}
unsigned int	sleep(unsigned int t)
{
	Sleep(t * 1000);
}
unsigned int	usleep(unsigned long t)
{
	Sleep(t / 1000);
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

