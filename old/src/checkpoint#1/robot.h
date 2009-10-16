#ifndef ROBOT
#define ROBOT
#include "data.h"
#include "camera.h"

/****************************************************************/
/* Blob a rectangular section of the video screen with		*/
/* a high density of color of interest.				*/
/* Essentially an area of interest				*/
/****************************************************************/
class	Blob
{
  public:
	Blob(VidWindow w, int h);

	int		hits;
	VidWindow	wind;
};
/****************************************************************/
/* BlobList							*/
/* manages all the areas of interest				*/
/****************************************************************/
class	BlobList
{
  private:
	int	mMax;
	int 	mCount;
	Blob	**mTable;
  public:
	BlobList(int max = 20);

	void	extend(int howMuch);
	void	append(Blob *b);
	void	sort();
	Blob	*get(int);
};

/****************************************************************/
/* Robot							*/
/* an abstract class that defines the common parts of the 	*/
/* Surveyor and Scribbler robots				*/
/* It is hoped that this may evolve in to a more general	*/
/* Mobile-Robot-With-Camera class				*/
/* 								*/
/* This turned out to be less than successful as the jpg format	*/
/* is the only commonly agreed standard				*/ 
/****************************************************************/
class	Robot
{
  protected:
 	Serial		&mPort;
	int		getChar();		// returns -1 if no char available
	int		getCharBlocking();
	int		getBlock(int len, char *buf);
	void		flushOutputBuffer();
	int		takeChar(unsigned char);
	int		takeBlock(int len, char *buf);
	void		flushInputBuffer();
  public:
	Robot(Serial &port);
	virtual	~Robot();

	virtual Data		*takePhotoJPEG()				= 0;
	virtual int		drive(int left, int right, int duration)	= 0;

	virtual int 		setColorId(int id, YUVRange &color)		= 0;

	virtual	int		getInfo(char *)					= 0;

};

#endif

