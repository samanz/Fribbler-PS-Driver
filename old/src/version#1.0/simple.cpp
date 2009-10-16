#include "scribbler.h" 
#include "system.h"



int	gDebugging = 0;


CLIENT_SERIAL	port(ADDRESS);

Scribbler       robot(port); 

int main( int argc, char *argv[])
{
	robot.drive(50, 50);

	sleep(1);

	robot.stop();

    	return 0;   
}
