#ifndef DATA_STUFF
#define DATA_STUFF
#include <stdio.h>

#define NO_LEN		0
#define BIGENDIAN	1
#define LITTLENDIAN	2

struct DataError
{
	int	mId;
	DataError(int id) 	{ mId = id; }
};

/************************************************************************/
/* DataSource								*/
/*									*/
/* A class of objects that supply data					*/
/************************************************************************/
class	DataSource
{
  public:
	DataSource()		{}
	virtual ~DataSource();

	virtual int	getChar(unsigned char &)		= 0;
		int	getCharBlocking(unsigned char &);
	virtual int	getBlock(char *buf, unsigned long)	= 0;
		int	getUint16(unsigned int &);
		int	getUint32(unsigned long &);
	virtual void	flushInput()			= 0;
	virtual	int	getFlags()		{ return 1; }
};
/************************************************************************/
/* DataSink								*/
/*									*/
/* A class of objects that consumes data				*/
/************************************************************************/
class	DataSink
{
  public:
	DataSink()		{}
	virtual ~DataSink();

	virtual	int	takeChar(unsigned char)			=0;
	virtual int	takeBlock(char *, unsigned long)=0;
	virtual void	flushOutput()			=0;
};
/************************************************************************/
/* Serial								*/
/*									*/
/* Serial communication is both a Source and a Sink of data		*/
/************************************************************************/
class	Serial : public DataSource, public DataSink
{
  public:
	Serial()		{}
	virtual ~Serial();

	virtual int	getChar(unsigned char &)		= 0;
	virtual int	getBlock(char *, unsigned long)		= 0;
	virtual void	flushInput()				= 0;

	virtual	int	takeChar(unsigned char)			=0;
	virtual int	takeBlock(char *, unsigned long)	=0;
	virtual void	flushOutput()				=0;
	virtual	int	getFlags()		{ return 0; }
};
/************************************************************************/
/* Data									*/
/*									*/
/* A class of objects of unstructured data				*/
/************************************************************************/
class Data
{
  private:
	long	mMax;
	long	mSize;
	char	*mData;
  public:
	Data(long = 128);
	Data(long, char *);
	Data(DataSource &s, int mode = BIGENDIAN);
	~Data();

	void	extend(long = 128);

	void	append(char);
	void	append(char *, int);
	void	append(Data &d);
	void	append(DataSource &s, long len);
	
	inline	long	getSize()	{ return mSize; }

	char	operator[](int index);
	
	char *	getData();
	int		getDataSize();
	void	write(DataSink &s, int mode = BIGENDIAN);
	void	read(DataSource &s, int mode = BIGENDIAN);
	void	talk(FILE*);
};

/************************************************************************/
/* RLE									*/
/*									*/
/* A class of objects that support the RLE encoded bit map		*/
/* returned by some Fluke commands					*/
/************************************************************************/
class	RLE 
{
  private:
	int	mSize;
	int	*mData;
  public:
	RLE(Data &s);
	~RLE();

	int	generateCharMap(char *&map, char onChar, char offChar);
	void	talk(FILE*);
};
#endif