#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cmath>
#include "poly.h"


extern void log(char const str[]);
extern char	logchunk[512];

static	char	data[512];
static  char	*parms;
static 	void	setString(char const str[])
{
	strcpy(data, str);
	parms = data;
}
static	int	getNextCords(char *cord)
{
	while (isspace(*parms))
		parms++;
	if (*parms != '(')
		return 0;
	while (*parms != ')')
	{
		if (!*parms)
			return 0; 
		*cord++ = *parms++;
	}
	*cord++ = *parms++;
	while (isspace(*parms))
		parms++;
	if (*parms == ',')
		parms++;
	*cord = '\0';

	return 1;
}

	


Poly::Poly(char const  parms[])
{
	char	cord[32];
	int 	i;
	int	x, y;

	mMaxPoints = 10;
	mXdef = new short[mMaxPoints];
	mYdef = new short[mMaxPoints];

	setString(parms);

	for (i = 0;getNextCords(cord);i++)
	{
		if (i == mMaxPoints)
		{
			short 	*newX = new short[mMaxPoints + 10];
			short	*newY = new short[mMaxPoints + 10];
			for (int j = 0;j < mMaxPoints;j++)
			{
				newX[j] = mXdef[j];
				newY[j] = mYdef[j];
			}
			mMaxPoints = mMaxPoints + 10;
			delete [] mXdef;
			delete [] mYdef;
			mXdef = newX;
			mYdef = newY;
		}
		sscanf(cord, "(%d,%d)", &x, &y);
		mXdef[i] = x;
		mYdef[i] = y;
	}
	mPointCount = i;
	mXtentative = new short[mPointCount];
	mYtentative = new short[mPointCount];
	mXactual = new short[mPointCount];
	mYactual = new short[mPointCount];
}
void	Poly::say(FILE *fp)
{
	fprintf(fp, "Poly count = %d, max = %d\n", mPointCount, mMaxPoints);

	fprintf(fp, "Defination ");
	for (int i = 0;i < mPointCount;i++)
	{
		if (i)
			fprintf(fp, ", ");
		fprintf(fp, "(%3d, %3d)", mXdef[i], mYdef[i]);
	}
	fprintf(fp, "\n"); 
	fprintf(fp, "Actual     ");
	for (int i = 0;i < mPointCount;i++)
	{
		if (i)
			fprintf(fp, ", ");
		fprintf(fp, "(%3d, %3d)", mXactual[i], mYactual[i]);
	}
	fprintf(fp, "\n"); 
}
/************************************************************************/
/* the standard version on the rotation matrix seems to give rotation	*/
/* in the clockwise direction. We want rotation in the counter-clockwise*/
/* direction for which we use the negative angle			*/
/************************************************************************/
void	Poly::position(int originX, int originY, double theta)
{
	double 	c = cos(-theta), s = sin(-theta);
/* I don't know why we need to get negative theta but it works why? 	*/


	for (int i = 0;i < mPointCount;i++)
	{
		mXtentative[i] = mXdef[i] * c - mYdef[i] * s + originX;
		mYtentative[i] = mXdef[i] * s + mYdef[i] * c + originY;


	}

}
void	Poly::commit()
{
	int 	i;

	for (i = 0;i < mPointCount;i++)
	{
		mXactual[i] = mXtentative[i];
		mYactual[i] = mYtentative[i];
	}
}
int	Poly::getTentative(short x[], short y[])
{
	int	i;

	for (i = 0;i < mPointCount;i++)
	{
		x[i] = mXtentative[i];
		y[i] = mYtentative[i];
	}
	return i;
}
int	Poly::getActual(short x[], short y[])
{
	int	i;

	for (i = 0;i < mPointCount;i++)
	{
		x[i] = mXactual[i];
		y[i] = mYactual[i];
	}
	return i;
}
#ifdef UNDEFINED
int	main(int argc, char *argv[])
{
	Poly	p(argv[1]);

	p.position(20, 20, 2 * M_PI);

	p.commit();
	
	p.say(stdout);
}
#endif
		
		
			
