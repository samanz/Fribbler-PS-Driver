#include "scribbler.h" 
#include "system.h"




int	gDebugging = 0;


//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
PosixSerial	port1("/dev/rfcomm1");		/* Linux or Mac with Real Robot */
PosixSerial	port("/dev/rfcomm0");		/* Linux or Mac with Real Robot */
//ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */



Scribbler       robot1(port1); 
Scribbler       robot(port);



int main( int argc, char *argv[])
{
	int	i;
	int	count = 4;
	int	speed = 50;

	if (argc > 1)
		count = atoi(argv[1]);

	if (argc > 2)
		speed = atoi(argv[2]);
		

	for (i = 0;i < count;i++)
	{
		robot1.drive(speed, speed);
		sleep(1);
		robot1.drive(-speed, speed);
		sleep(1);
		robot1.stop();

		robot.drive(speed, speed);
		sleep(1);
		robot.drive(-speed, speed);
		sleep(1);
		robot.stop();
	}

	robot1.stop();
	robot.stop();

    	return 0;   
}
