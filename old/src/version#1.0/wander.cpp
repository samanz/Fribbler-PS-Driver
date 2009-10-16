#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 0;




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
	Data	*jpeg;
	FileOut	*file;
	char	*addr;

       	addr = getenv("SCRIBBLER");


	CLIENT_SERIAL	port(addr);
	Scribbler       robot(port);

       
	if (argc > 1)
                gDebugging = atoi(argv[1]);

	robot.setForwardness(0); // Scribbler Forward

        robot.beep(512, 500);

	for (int i = 0;i < 100;i++)
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
			robot.drive(-50, -50, 10); 	// back off a bit
			robot.drive(-50, 50);		// and turn
		}
		else if (robot.scribblerLeftIR())
		{
			if (robot.scribblerRightIR())
			{
				// both clear
				robot.drive(50, 50);	// forward
			}
			else
			{
				// Left clear Right Blocked
				robot.drive(0, 50);	// Left
			}
		}
		else 
		{
			if (robot.scribblerRightIR())
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
	jpeg = robot.takePhotoJPEG();
	if (jpeg)
	{
//		jpeg->talk(stdout);
		printf("got a jpeg - save it\n");
		file = new FileOut("whereAmI.jpeg");
		jpeg->write(*file, 0);
		delete file;
	}	





    	return 0;   
}
	
