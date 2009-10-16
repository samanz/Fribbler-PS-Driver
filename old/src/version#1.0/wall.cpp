#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>


int	gDebugging = 0;






CLIENT_SERIAL	port(ADDRESS);

Scribbler       robot(port); 



int main( int argc, char *argv[])
{



	srand(time(0));
       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


	robot.setForwardness(0); // Scribbler Forward


        robot.beep(512, 500);


	for (int i = 0;i < 1000;i++)
	{
		robot.updateScribblerSensors();

		printf("IR %d-%d, Light %d-%d-%d, Line %d-%d, Stall %d\n", 
			robot.scribblerLeftIR(), robot.scribblerRightIR(),
			robot.scribblerLeftLight(),
			robot.scribblerCenterLight(),
			robot.scribblerRightLight(),
			robot.scribblerLineLeft(),
			robot.scribblerLineRight(),
			robot.scribblerStall()
		       );
 
		if (robot.scribblerStall())
		{
			robot.setScribblerLeds(1, 1,1);
			robot.drive(-50, -50); 	// back off a bit
			usleep(200);
			robot.drive(-50, 50);		// and turn
			usleep(200);
			robot.drive(rand()%200 - 100, rand()%200 - 100);

		}
		else 
		{
			
			switch (robot.scribblerLeftIR() *2 + robot.scribblerRightIR())
			{
			  case 0:
				robot.setScribblerLeds(0, 0, 0);
				robot.drive(-50, -50); 	// back off a bit
				usleep(10000);
				robot.drive(-50, 50);		// and turn
				usleep(2000 * rand()%200);
				break;
			  case 1:
				robot.setScribblerLeds(0, 0,1);
				robot.drive(50, -30);
				break;
			  case 2:
				robot.setScribblerLeds(1, 1, 1);
				robot.drive(50, 50);
				break;
			  case 3:
				robot.setScribblerLeds(0, 1,1);
				robot.drive(50, 50);
				break;
			}


		}
		usleep(10000);			
	}



    	return 0;   
}
	
