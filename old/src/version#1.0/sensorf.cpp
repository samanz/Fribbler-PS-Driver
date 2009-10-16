#include "scribbler.h" 
#include "system.h"


#include <stdlib.h>
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
	int	count = 1;
	int	power = 135;
	char	*addr;

       	addr = getenv("SCRIBBLER");

	CLIENT_SERIAL	port(addr);
	Scribbler       robot(port); 

	if (argc > 1)
                count = atoi(argv[1]);
	if (argc > 2)
                power = atoi(argv[2]);

	robot.setDonglePowerIR(power);

 	for (i = 0;i < count;i++)
	{
		center = robot.getDongleIR(centerIR);
		left = robot.getDongleIR(leftIR);
		right = robot.getDongleIR(rightIR);

		robot.updateScribblerSensors();
		printf("IR %04d-%04d-%04d, Stall %d\n", 
			left, center, right,
			robot.scribblerStall()
		       );
	
	}

    	return 0;   
}
	
