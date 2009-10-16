/********************************************************************************/
/* sample  									*/ 
/* Take a color sample from the bottom center of visual field			*/
/*										*/
/* usage: sample <filename>							*/
/* <filename> is optional defaults to sample					*/
/* 										*/
/* Description: samples the color range in rectangle x = (30 .. 50) Y = (0 .. 1)*/
/* reports that sample and results of common Vision member functions		*/
/* produces picture in <filename>.jpg and results in <filename>.txt		*/
/* 										*/
/********************************************************************************/
#include "SVR.h"
#include <string>
#include <iostream>
using namespace std;

#include "picframe.h"

#include <sys/time.h>
#include <time.h>

class	Timer
{
	struct timeval	start;
  public:
	Timer();
	~Timer();

	long	msLapsed();
};
Timer::Timer()
{
	gettimeofday(&start, NULL);
}
Timer::~Timer()
{
}
long	Timer::msLapsed()
{
	long	lapsed;
	struct	timeval	now;

	gettimeofday(&now, NULL);

	lapsed = (now.tv_sec - start.tv_sec) * 1000;
	lapsed += (now.tv_usec - start.tv_usec) / 1000;


	return lapsed;
}




Surveyor	robot(ADDRESS);

PicFrame	frame;


void	myUsleep(long wait)
{
	Timer	tim;

	for (;tim.msLapsed() < wait;)
	{
		robot.takePhoto();

		frame.loadPic(robot.getPhoto(), robot.getPhotoSize());
	}
}
void	calibrateCam(int exp)
{
	int		i;
	int		buf[80];
	VidWindow	win;
	VidWindow	win1(30, 50, 0, 10);	
	YUVRange	cRange;	

	printf("loadBin(2, %s) =\n", win1.say());
	robot.loadBin(2, win1);
	printf("done\n");

	printf("getBin(2 ...) =\n");
	robot.getBin(2, cRange);
	printf("%s\n", cRange.say());

	cRange.expandY(0xFF);
	cRange.expandU(exp);
	cRange.expandV(exp);


	printf("setBin()\n");
	robot.setBin(2, cRange);
	printf("getBin()\n");
	robot.getBin(2, cRange);
	printf("%s\n", cRange.say());


	printf("getDistMatching(2, ...) =");
	robot.getDistMatching(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");

	printf("getDistToMatch(2, ...) =");
	robot.getDistToMatch(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");

	printf("getCountScan(2, ...) =");
	robot.getCountScan(2, buf);
	for (i = 0;i < 80;i++)
	{
		if (i%20 == 0)
			printf("\n");
		printf("%2d ", buf[i]);
	}
	printf("\nend\n");

	cout << "loadBlobs(2) =" << endl;
	robot.loadBlobs(2);
	BlobNode	*list = robot.getBlobList();
	BlobNode	*node;
	int		hits, totalHits;
	

	totalHits = 0;
	for (node = list;node;node = node->next)
	{
		hits += node->hits;
		cout << node->wind.say() <<  " hits = " << hits << endl;
		totalHits += hits;
	}
	cout << "Total Hits" << totalHits << endl;

	BlobNode	*next;
	
	for (node = list;node;node = next)
	{
		next = node->next;
		delete node;
	}

	for (i = 0;i < 12;i++)
	{
		robot.drive(50, -50);
		myUsleep(1000);
		robot.drive(50, 50);
		myUsleep(1000);
	}
 
	robot.drive(0, 0);
}




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];
	int	arg = 0;
	int	ret;

	robot.setDebuging(0);
	ret = robot.getVersion(buf);
	printf("getVersion() = %d\n", ret);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);
	if (argc > 1)
		arg = atoi(argv[1]);


	calibrateCam(arg);


    	return 0;   
}
	
