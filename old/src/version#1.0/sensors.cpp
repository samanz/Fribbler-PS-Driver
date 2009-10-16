#include "scribbler.h" 
#include "system.h"
#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 0;




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	int	count = 1;
	char	*addr;

       	addr = getenv("SCRIBBLER");

	CLIENT_SERIAL	port(addr);
	Scribbler       robot(port);

       
	if (argc > 1)
                count = atoi(argv[1]);

	for (int i = 0;i < count;i++)
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
			
	}

    	return 0;   
}
	
