#include "scribbler.h" 
//#include "WinSerial.h"
#include "PosixSerial.h"
#include "PosixFile.h"
#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 0;



PosixSerial	port(ADDRESS);

Scribbler       robot(port); 

//YUVRange	range(0x00FE, 0x7E8C, 0x5A78);
//YUVRange	range(0x00FE, 0x00FF, 0x00FF);
YUVRange	range(0x00FE, 0x3388, 0xBEFE);


#define THRESHHOLD		1000

/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	int	left, center, right;
	int 	i;

       
	if (argc > 1)
                gDebugging = atoi(argv[1]);

	robot.setForwardness(1); // Fluke Forward

	robot.setDonglePowerIR(140);

        robot.beep(512, 500);

	for (i = 0;i < 100;i++)
	{
		center = robot.getDongleIR(centerIR);
		left = robot.getDongleIR(leftIR);
		right = robot.getDongleIR(rightIR);

		robot.updateScribblerSensors();
		printf("IR %04d-%04d-%04d, Stall %d\n", 
			left, center, right,
			robot.scribblerStall()
		       );
		if (robot.scribblerStall())
		{
			robot.drive(-50, -50, 10); 	// back off a bit
			robot.drive(-50, 50);		// and turn
		}
		else if (left < THRESHHOLD)
		{
			if (right < THRESHHOLD)
			{
				// both clear
				robot.drive(50, 50);	// forward
			}
			else
			{
				// Left clear Right Blocked
				robot.drive(0, 50);	// Left
			}
		}
		else 
		{
			if (right < THRESHHOLD)
			{
				// Left Blocked Right clear
				robot.drive(50, 0);	// Right
			}
			else
			{
				// both blocked
				robot.drive(-50, 50);	// turn
			}
		}
			
	}
	robot.stop();	





    	return 0;   
}
	
