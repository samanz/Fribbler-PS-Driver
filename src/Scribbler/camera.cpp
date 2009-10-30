#include "camera.h"
#include <stdio.h>

YUVRange::YUVRange()
{
	mYFrom = mUFrom = mVFrom = 0;
	mYTo = mUTo = mVTo = 0;
}

YUVRange::YUVRange(int Y, int U, int V)
{
	mYFrom = Y >> 8;
	mYTo   = Y & 0x00FF;
	mUFrom = U >> 8;
	mUTo   = U & 0x00FF;
	mVFrom = V >> 8;
	mVTo   = V & 0x00FF;
}

void	YUVRange::setY(unsigned char from, unsigned char to)
{
	mYFrom = from;
	mYTo   = to;
}
void	YUVRange::setU(unsigned char from, unsigned char to)
{
	mUFrom = from;
	mUTo   = to;
}
void	YUVRange::setV(unsigned char from, unsigned char to)
{
	mVFrom = from;
	mVTo   = to;
}
void	YUVRange::getY(unsigned char &from, unsigned char &to)
{
	from = mYFrom;
	to   = mYTo;
}
void	YUVRange::getU(unsigned char &from, unsigned char &to)
{
	from = mUFrom;
	to   = mUTo;
}
void	YUVRange::getV(unsigned char &from, unsigned char &to)
{
	from = mVFrom;
	to   = mVTo;
}
void	YUVRange::expandY(int del)
{
	int	from, to;

	from = mYFrom - del;
	to   = mYTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mYFrom = from;
	mYTo   = to;
}
void	YUVRange::expandV(int del)
{
	int	from, to;

	from = mVFrom - del;
	to   = mVTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mVFrom = from;
	mVTo   = to;
}
void	YUVRange::expandU(int del)
{
	int	from, to;

	from = mUFrom - del;
	to   = mUTo   + del;
	if (from < 0)
		from = 0;
	if (to > 0xFF)
		to = 0xFF;
	if (from > to)
		from = to = (from + to)/2;
	mUFrom = from;
	mUTo   = to;
}

char	*YUVRange::say()
{
	static char line[64];

	sprintf(line, "YUVRange %2x-%2x, %2x-%2x, %2x-%2x", 
		mYFrom, mYTo, mUFrom, mUTo, mVFrom, mVTo);

	return line;
}

VidWindow::VidWindow()
{
	xFrom = yFrom = 0;
	xTo = yTo = 0;
}
VidWindow::VidWindow(int xF, int xT, int yF, int yT)
{
	xFrom = xF;
	xTo   = xT;
	yFrom = yF;
	yTo   = yT;
}
VidWindow::VidWindow(char *buf)
{
	xFrom = *buf++;
	xTo   = *buf++;
	yFrom = *buf++;
	yTo   = *buf;
}
void VidWindow::setX(unsigned char from, unsigned char to)
{
	xFrom = from;
	xTo   = to;
}
void VidWindow::setY(unsigned char from, unsigned char to)
{
	yFrom = from;
	yTo   = to;
}
void VidWindow::getX(unsigned char &from, unsigned char &to)
{
	from = xFrom;
	to   = xTo;
}
void VidWindow::getY(unsigned char &from, unsigned char &to)
{
	from = yFrom;
	to   = yTo;
}
char	*VidWindow::say()
{
	static	char	line[64];

	sprintf(line, "Rect x = (%d - %d), y = (%d - %d)", 
			xFrom, xTo, yFrom, yTo);

	return line;
}
