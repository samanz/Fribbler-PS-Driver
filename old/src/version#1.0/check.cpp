#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>



int	gDebugging = 0;

//extern int	sleep(int);




CLIENT_SERIAL	port(ADDRESS);

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
			robot.scribblerLeftLight(),
			robot.scribblerCenterLight(),
			robot.scribblerRightLight(),
			robot.scribblerLineLeft(),
			robot.scribblerLineRight(),
			robot.scribblerStall()
		       );


		robot.drive(-10, 10);


usleep(100000);

			
	}

	robot.drive(-50, 50);
	usleep(5000);

	robot.stop();



    	return 0;   
}
	
