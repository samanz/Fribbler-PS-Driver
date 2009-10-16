#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 0;



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
	char	*addr;

       	addr = getenv("SCRIBBLER");

	CLIENT_SERIAL	port(addr);
	Scribbler       robot(port);


       
	if (argc > 1)
                gDebugging = atoi(argv[1]);

	robot.setForwardness(1); // Fluke Forward

	robot.setDonglePowerIR(137);

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
		else if (center < 600)
		{
			robot.drive(50, 50);	
		}
		else 
		{
			if (right < left)
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
	
