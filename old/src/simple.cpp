#include "scribbler.h" 
#include "system.h"




int	gDebugging = 0;


//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
PosixSerial	port(ADDRESS);		/* Linux or Mac with Real Robot */
//ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */


Scribbler       robot(port); 


int main( int argc, char *argv[])
{
	robot.drive(50, 50);

	sleep(1);

	robot.stop();

    	return 0;   
}
