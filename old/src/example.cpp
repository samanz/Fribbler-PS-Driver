#include "scribbler.h"
#include "system.h"

int	gDebugging = 0;

//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
PosixSerial	port(ADDRESS);		/* Linux or Mac with Real Robot */
//ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */

Scribbler       robot(port);




/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	if (argc > 1)
                gDebugging = atoi(argv[1]);

	/* example code begins	*/

	int	left, center, right;

	robot.setDonglePowerIR(137);

	char   buffer[16];
	robot.getScribblerName(buffer);
	printf("Robots name is <%s>\n", buffer);

//	for (int i = 0;i < 100;i++)
//	{
//		center = robot.getDongleIR(centerIR);
//		left = robot.getDongleIR(leftIR);
//		right = robot.getDongleIR(rightIR);
//
//		printf("%4d - %4d - %4d\n", left, center, right);
//	}


	/* example code ends	*/

	return 0;
}
