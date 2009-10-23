#include "robot.h"


Blob::Blob(VidWindow w, int h)
{
	wind = w;
	hits = h;
}
BlobList::BlobList(int max)
{
	mMax = max;
	mCount = 0;
	mTable = new Blob *[mMax];
	for (int i = 0;i < mMax;i++)
		mTable[i] = 0;
}
void	BlobList::extend(int howMuch)
{
	int	i;
	int	newMax = mMax + howMuch;
	Blob	**newTable;
	newTable = new Blob *[newMax];

	for (i = 0;i < mMax;i++)
		newTable[i] = mTable[i];
	for (;i < newMax;i++)
		newTable[i] = 0;

	delete mTable;
	mTable = newTable;
	mMax = newMax;
}	

void	BlobList::append(Blob *b)
{
	if (mCount == mMax)
		extend(5);
	mTable[mCount++] = b;
}
void	BlobList::sort()
{
	int	swap;

	do
	{
		swap = 0;

		for (int i = 0;i < mCount - 1;i++)
		{
			if (mTable[i]->hits < mTable[i+1]->hits)
			{
				Blob	*temp;

				temp = mTable[i];
				mTable[i] = mTable[i+1];
				mTable[i+1] = temp;

				swap = 1;
			}
		}
	} while (swap);
}
Blob	*BlobList::get(int id)
{
	if (id < mCount)
		return 0;
	return mTable[id];
}

Robot::Robot(Serial &port) : mPort(port)
{
}
Robot::~Robot()
{
}
int     Robot::getChar()
{
	unsigned char	ch;
	int	ret;

	ret = mPort.getChar(ch);
	if (!ret)
		return 0;

	return ch;
}
int     Robot::getCharBlocking()
{
	unsigned char	ch;

	mPort.getCharBlocking(ch);

	return ch;
}
void	Robot::flushInputBuffer()
{
	mPort.flushInput();
}
int	Robot::takeChar(unsigned char ch)
{
	mPort.takeChar(ch);
}
void	Robot::flushOutputBuffer()
{
	mPort.flushOutput();
}
