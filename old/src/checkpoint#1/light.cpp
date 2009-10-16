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
	int	rl, cl, ll;



	srand(time(0));
       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


	robot.setForwardness(0); // Scribbler Forward

        robot.beep(512, 500);

	for (int i = 0;;i++)
	{
		robot.updateScribblerSensors();

		printf("IR %d-%d, Light %5d-%5d-%5d, Line %d-%d, Stall %d\n", 
			robot.scribblerLeftIR(), robot.scribblerRightIR(),
			ll = robot.scribblerLeftLight(),
			cl = robot.scribblerCenterLight(),
			rl = robot.scribblerRightLight(),
			robot.scribblerLineLeft(),
			robot.scribblerLineRight(),
			robot.scribblerStall()
		       );


		if(ll == 100 && cl == 100 && rl == 100)
		{
			left = 0;
			right= 0;
			robot.drive(left, right);
		} 
		else if (robot.scribblerStall())
		{
			robot.setScribblerLeds(1, 1,1);
			robot.drive(-left, -right); 	// back off a bit
			usleep(20000);
			robot.drive(-50, 50);		// and turn
			usleep(20000);
			robot.drive(rand()%200 - 100, rand()%200 - 100);
			robot.setDimmerLED(++dimmer);
		}
		
		else if (ll > rl)
		{
			left  = 50;
			right = 20;
			robot.drive(left, right);
		}
		else
		{
			left  = 20;
			right = 50;
			robot.drive(left, right);
		}
		
		
usleep(100000);

			
	}

	robot.drive(-50, 50);
	usleep(5000);

	robot.stop();



    	return 0;   
}
	
