/********************************************************************************/
/* kick  									*/ 
/* a more involved demonstration program for the Surveyor class			*/
/*										*/
/* usage: kick									*/
/* 										*/
/* Description: robot will attempt to push the ball into the blue goal		*/
/* 										*/
/********************************************************************************/
#include "SVR.h" // use approapriate header for your OS
#include "color.h"
#include <string>

#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <assert.h>
using namespace std;

int	moving = 1;

// at drive(-40, 40) left turn
#define FIVE_DEGREES 		64200
#define SECONDS(n)	(long) (n * 1000000)



Surveyor	robot(ADDRESS);	

void	drive(int l, int r, int d = 0)
{
	if (moving)
		robot.drive(l, r, d);
}

#define	GROUND	0
#define	BALL	1
#define GOAL	2

YUVRange	groundColor(T_GREEN_FIELD);
YUVRange	ballColor(T_ORANGE_BALL);
YUVRange	goalColor(T_BLUE_GOAL);

int	ballFound;
int	ballFrom, ballTo;
int	ballPix;
int	goalFound;
int	goalFrom, goalTo;

void	initalizeRobot()
{
	YUVRange	cRange;

	groundColor.expandY(0xFF);  // ignore intensity
	groundColor.expandU(1);
	groundColor.expandV(1);
	robot.setBin(GROUND, groundColor);

	ballColor.expandY(0xFF);  // ignore intensity
	ballColor.expandU(1);
	ballColor.expandV(1);
	robot.setBin(BALL, ballColor);

	goalColor.expandY(0xFF);  // ignore intensity
	goalColor.expandU(1);
	goalColor.expandV(1);
	robot.setBin(GOAL, goalColor);

	robot.getBin(GROUND, cRange);
	printf("Ground %s\n", cRange.say());
	robot.getBin(BALL, cRange);
	printf("Ball   %s\n", cRange.say());
	robot.getBin(GOAL, cRange);
	printf("Goal   %s\n", cRange.say());
}
int	lookForBall()
{
	int	ball[80];
	int	i;
	int	found;


	found = 0;
	ballFrom = ballTo = -1;

	robot.getCountScan(BALL, ball);
	for (i = 0;i < 80;i++)
	{
		if (ball[i] != 0)
		{
			ballPix += ball[i];
			found = 1;
			if (ballFrom == -1)
				ballFrom = i;
			ballTo= i;
		}
	}
	
	return found;
}

int	locateBall()
{
	int	ball[80];
	int	i;
	int	found;


printf("looking for ball\n");
	for (;;)
	{
		ballPix = 0;
		found = 0;
		ballFrom = ballTo = -1;

		robot.getCountScan(BALL, ball);
		for (i = 0;i < 80;i++)
		{
			if (ball[i] != 0)
			{
				ballPix += ball[i];
				found = 1;
				if (ballFrom == -1)
					ballFrom = i;
				ballTo= i;
			}
		}
		if (!found)
		{
			drive(40, 40);
			usleep(SECONDS(1.5));
			drive(40, -40);		// turn a bit
			usleep(4 * FIVE_DEGREES);		// say 20 degrees
		}
		else if (ballFrom + ballTo < 50)
		{
			drive(40, -40);
			usleep(FIVE_DEGREES/3);
		}
		else if (ballFrom + ballTo > 110)
		{
			drive(-40, 40);
			usleep(FIVE_DEGREES/3);
		}
		else
			break;
	}	

	printf("findBall %d (%d-%d)\n", found, ballFrom, ballTo);
	return found;
}
int	lookForGoal()
{
	int	goal[80];
	int	i;
	int	found = 0;

	goalFrom = goalTo = -1;

	robot.getCountScan(GOAL, goal);
	for (i = 0;i < 80;i++)
	{
		if (goal[i] != 0)
		{
			found = 1;
			if (goalFrom == -1)
				goalFrom = i;
			goalTo = i;
		}
	}
	printf("findGoal %d (%d-%d)\n", found, goalFrom, goalTo);	
	return found;
}
int	findField()
{
	int	field[80];
	int	i;
	int	count = 0;
	int	found = 0;

	robot.getDistMatching(GROUND, field);
	for (i = 30;i < 50;i++)
	{
		if (field[i] > 0)
			count++;
	}
	if (count > 1)
		found = 1;

	printf("findField %d   %d\n", found, count);	
	return found;
}

void	aimAt(int aim, int &left, int &right)
{
	printf("aim at %d\n", aim);
	if (aim < 20)
	{
		left = 10;
		right = 40;
	}
	else if (aim < 30)
	{
		left = 20;
		right = 40;
	}
	else if (aim < 50)
	{
		left = right = 50;
	}
	else if (aim < 70)
	{
		left = 40;
		right = 20;
	}
	else
	{
		left = 40;
		right = 10;
	}
}
void	adjustAim(int aim)
{
	if (aim < 20)
	{
		drive(0, 40);
		usleep(100000);
	}
	else if (aim < 30)
	{
		drive(0, 20);
		usleep(100000);
	}
	else if (aim < 50)
	{

	}
	else if (aim < 70)
	{
		drive(20, 0);
//		usleep(100000);
	}
	else
	{
		drive(40, 0);
//		usleep(100000);
	}
}
void	focusOnBall(int toll)
{
	int	i;
	int	ball[80];
	long	cg = 0;

	for (;;)
	{
		robot.getCountScan(BALL, ball);
		for (i = 0;i < 80;i++)
		{
			cg += (i - 40)*ball[i];
		}
		printf("cg = %ld\n", cg);
		if (cg > -toll && cg < toll)
		{
			drive(0, 0);
			break;
		}
		if (cg > 0)
		{
			drive(40, -40, 3);
		}
		else
		{
			drive(-40, 40, 3);
		}
		cg = 0;
	}
}

void	robotPlay()
{
	int	shootCounter = 0;

	for (;;)
	{
		ballFound = lookForBall();
		
		if (!ballFound && !shootCounter)
		{
			printf("searching for ball\n");
			drive(40, -40);
			usleep( FIVE_DEGREES);
			continue;
		}
		printf("ball found\n");
		adjustAim((ballFrom + ballTo)/2);
		goalFound = lookForGoal();
		if (goalFound && ballFound && !shootCounter)
		{
			printf("goal found, start shoot!\n");
			shootCounter = 5;
				
			focusOnBall(100);

			printf("Shoot!\n");
			drive(100, 100);		// run like hell!
			usleep(200000);
		}
		else if (shootCounter)
		{
			printf("shoot follow through\n");
			shootCounter--;
			drive(100, 100);		// follow through
			usleep(200000);
		}
		else
		{
			focusOnBall(1000);
			printf("go around ball\n");
			drive(-40, -40);
			usleep(100000);			// back off
			printf("turn left\n");
			drive(-40, 40); 		// turn left
			usleep(18 * FIVE_DEGREES);

			printf("go in circle\n");
			drive(40, 32); 			// go around circle
			usleep(3000000);

			printf("turn right\n");
			drive(40, -40);			// point back at ball
			usleep(18 * FIVE_DEGREES);

			focusOnBall(1000);
		}
	}
}





/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];

	robot.getVersion(buf);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);
	if (argc > 1)
		moving = atoi(argv[1]);
	srand(time(NULL));


	initalizeRobot();

	robotPlay();	

    	return 0;   
}
	
