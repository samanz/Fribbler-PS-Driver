#include "data.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>


unsigned int	sleep(unsigned int);

extern 	int	gDebugging;
const	int	thisModule = 0x0001;

extern void	log(char const []);
extern char	logchunk[512];

Data::Data(long len, char *buf)
{
	mMax = len + 16;
	mSize = len;
	mData = new char[mMax];
	memcpy(mData, buf, mSize);
}
Data::Data(long len)
{
	mMax = len + 128;
	mSize = 0;
	mData = new char[mMax];
}
Data::Data(DataSource &s, int mode)
{
	mMax = 256;
	mSize = 0;
	mData = new char[mMax];
	read(s, mode);
}
void	Data::extend(long howMuch)
{
	long	newMax; 
	char	*newData;

	if ((mMax - mSize) > howMuch)
		return;

	newMax = mMax + howMuch;
	newData = new char [newMax];

	assert(newData);

	memcpy(newData, mData, mSize);
	delete [] mData;
	mData = newData;
	mMax = newMax;
}
void	Data::append(char ch)
{
	if (mSize == mMax)
		extend(128);
	mData[mSize++] = ch;
}
void	Data::append(char *buf, int len)
{
	if ((mSize + len) >= mMax)
		extend(len);
	memcpy(mData + mSize, buf, len);
	mSize += len;
}
void	Data::append(Data &d)
{
	extend(d.mSize);
	append(d.mData, d.mSize);
}
void	Data::append(DataSource &s, long len)
{
	extend(len);
	s.getBlock(mData + mSize, len);
	mSize += len;
}
char	Data::operator[](int index)
{
	return mData[index];
}


Data::~Data()
{
	delete [] mData;
}
DataSink::~DataSink()
{
}
DataSource::~DataSource()
{
}
Serial::~Serial()
{
}
int	DataSource::getCharBlocking(unsigned char &ch)
{
	long	int	trys;
	for (trys = 0;!getChar(ch);trys++)
	{
		if (trys >= 100000)
		{
			printf("Throwing DataError\n");
			throw DataError(1);
		}
	}

	if (gDebugging & thisModule)
	{		
		sprintf(logchunk, "<-- 0x%x\n", ch);
		log(logchunk);
	}

	return 1;
}
int	DataSource::getUint16(unsigned int &i)
{
	return 0;
}
int	DataSource::getUint32(unsigned long &i)
{
	return 0;
}
void	Data::write(DataSink &s, int mode)
{
	char	hiByte, loByte;

	hiByte = mSize >> 8;
	loByte = mSize & 0xFF;
	switch (mode)
	{
	  case 0:
		break;
	  case BIGENDIAN:
		s.takeChar(hiByte);
		s.takeChar(loByte);
		break;
	  case LITTLENDIAN:
		s.takeChar(loByte);
		s.takeChar(hiByte);
		break;
	}		
	s.takeBlock(mData, mSize);
}
void	Data::read(DataSource &s, int mode)
{
	unsigned char	loByte, hiByte;
	int		len;
	unsigned char	ch;

	if (mode == 0)
	{
		for (;;)
		{
			if (!s.getChar(ch))
			{
				/* if its a slow device 	*/
				/* give it an extra second 	*/
				/* before we say its over	*/
				if (!s.getFlags())
				{
					log("Wait a second\n");
					sleep(1);
				}
				else
					log("don't wait a sec\n");
				if (!s.getChar(ch))
					break;
			}
			append(ch);
		}
		return;
	}

	switch (mode)
	{
	  case BIGENDIAN:
		s.getCharBlocking(hiByte);
		s.getCharBlocking(loByte);
		break;
	  case LITTLENDIAN:
		s.getCharBlocking(loByte);
		s.getCharBlocking(hiByte);
		break;
	}		
	
	len = (hiByte << 8) | loByte;
	extend(len);

	for (int i = 0;i < len;i++)
	{
		s.getCharBlocking(ch);
		append(ch);
	}

}
void	Data::talk(FILE *fp)
{
	fprintf(fp, "Data object len = %ld, max = %ld\n",
			mSize, mMax);
	for (int i = 0;i < mSize;i++)
	{
		if (i % 16 == 0)
			fprintf(fp, "\n");
		fprintf(fp, "%02x ", mData[i]);
	}
	fprintf(fp, "\n End of DataObject\n");
}
RLE::RLE(Data &data) 
{
	unsigned char	hiByte;
	unsigned char	loByte;
	int	len;
	int	i;
	
	len = data.getSize();
	
	mSize = len/2;
	mData = new int [mSize];

	for (i = 0;i < mSize;i++)
	{
		hiByte = data[2*i];
		loByte = data[2*i +1];
		mData[i] = (hiByte << 8) | loByte;
	}
	
}
RLE::~RLE()
{
	delete [] mData;
}
int	RLE::generateCharMap(char *&map, char onChar, char offChar)
{
	int	tot = 0;
	int	i;
	int	on;
	int	ch;
	char	*ptr;

	for (i = 0;i < mSize;i++)
	{
		tot += mData[i];
	}
	map = new char[tot];
	on = 0;
	ptr = map;
	for (i = 0;i < mSize;i++)
	{
		ch = on ? onChar : offChar;
		for (int j = 0;j < mData[i];j++)
			*ptr++ = ch;
		on = on ? 0 : 1;
	}
	return tot;
}
			
void	RLE::talk(FILE* fd)
{
	int	i;
	int	in;
	long	tot = 0;
	int	pos = 0;
	

	fprintf(fd, "RLE count %d\n", mSize);
	for (i = 0;i < mSize;i++)
	{
		fprintf(fd, "%d.", mData[i]);
		tot += mData[i];
	}
	fprintf(fd, "\ntotal = %ld\n", tot);

	in = 1;
	for (i = 0;i < mSize;i++)
	{
		in = !in;
		for (int j = 0;j < mData[i];j++)
		{
			if (pos % 256 == 0)
				fprintf(fd, "\n");
			if (pos++ % 4 == 1)
			{
				if (in)
					fprintf(fd, "X");
				else
					fprintf(fd, ".");
			}
		}
	}
	fprintf(fd, "\n\n");
}



