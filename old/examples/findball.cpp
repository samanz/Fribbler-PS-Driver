/********************************************************************************/
/* findball  									*/ 
/* a simple demonstration program for the Surveyor class			*/
/*										*/
/* usage: findball								*/
/* 										*/
/* Description: robot will turn until it sees the ball				*/
/* 										*/
/********************************************************************************/
#include "SVR.h" // use approapriate header for your OS
#include "color.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <assert.h>
using namespace std;


/* make a global Surveyor object called robot */
Surveyor	robot(ADDRESS);

YUVRange	ballColor(T_ORANGE_BALL);

int	main()
{
	char	buffer[256];
	int	ball[80];


	robot.getVersion(buffer);
	cout << "SRV-1 version " << buffer << endl;

	robot.setBin(1, ballColor);
	
	for (;;)
	{
		robot.getCountScan(1, ball);
		if (ball[40] > 0)
			break;
		robot.drive(-40, 40, 3); // turn a bit
	}
	

	return 0;
}
