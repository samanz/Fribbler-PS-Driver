#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>



int	gDebugging = 0;

//extern int	sleep(int);



//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
//PosixSerial	port(ADDRESS);		/* Linux or Mac with Real Robot */
ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */


Scribbler       robot(port); 

//YUVRange	range(0x00FE, 0x7E8C, 0x5A78);
//YUVRange	range(0x00FE, 0x00FF, 0x00FF);
YUVRange	range(0x00FE, 0x3388, 0xBEFE);



/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{

	int	dimmer = 0;
	int	left = 50;
	int	right = 50;
	int	flukeLeftIR;
	int	flukeCenterIR;
	int	flukeRightIR;



	srand(time(0));
       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


	robot.setForwardness(0); // Scribbler Forward

        robot.beep(512, 500);

	for (int i = 0;;i++)
	{
		robot.updateScribblerSensors();
		flukeLeftIR = robot.getDongleIR(leftIR);
		flukeCenterIR = robot.getDongleIR(centerIR);
		flukeRightIR = robot.getDongleIR(rightIR);

		printf("IR %d-%d, Light %5d-%5d-%5d, Line %d-%d, Stall %d; Fluke %3d-%3d-%3d\n", 
			robot.scribblerLeftIR(), robot.scribblerRightIR(),
			robot.scribblerLeftLight(),
			robot.scribblerCenterLight(),
			robot.scribblerRightLight(),
			robot.scribblerLineLeft(),
			robot.scribblerLineRight(),
			robot.scribblerStall(),
			flukeLeftIR,
			flukeCenterIR,
			flukeRightIR
		       );


		if (robot.scribblerStall())
		{
			robot.setScribblerLeds(1, 1,1);
			robot.drive(-50, -50); 	// back off a bit
			usleep(2000);
			robot.drive(-50, 50);		// and turn
			usleep(2000);
			robot.drive(rand()%200 - 100, rand()%200 - 100);
			robot.setDimmerLED(++dimmer);
		}
		else if (robot.scribblerLeftIR())
		{
			robot.setDongleLED(1);

			if (robot.scribblerRightIR())
			{
				robot.setScribblerLeds(1, 0, 1);
				// both clear
				robot.drive(left = 100 - rand()%100, right = 100 - rand()%100);	// forward
			}
			else
			{
				robot.setScribblerLeds(1, 0, 0);
				// Left clear Right Blocked
				robot.drive(left = 0, right = 100 - rand()%100);	// Left
			
			}
		}
		else 
		{
			robot.setDongleLED(0);

			if (robot.scribblerRightIR())
			{
				robot.setScribblerLeds(0, 0, 1);
				// Left Blocked Right clear
				robot.drive(left = 100 - rand()%100, right = 0);	// Right
			}
			else
			{
				robot.setScribblerLeds(1, 0, 1);
				// both blocked
				robot.drive(left = -rand()%100, right = -rand()%100);	
			}
			
		}

 

 
	usleep(10000);

			
	}

	robot.drive(-50, 50);
	usleep(5000);

	robot.stop();



    	return 0;   
}
	
