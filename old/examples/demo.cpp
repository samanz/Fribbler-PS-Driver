/********************************************************************************/
/* demo  									*/ 
/* a simple demonstration program for the Surveyor class			*/
/*										*/
/* usage: demo									*/
/* 										*/
/* Description: robot will wander around always staying on the football field	*/
/* 										*/
/********************************************************************************/
#include "SVR.h" // use approapriate header for your OS
#include <string>



Surveyor	robot(ADDRESS);	


void	calibrateCam()
{
	int		i;
	int		j;
	int		buf[80];
	VidWindow	win;
	VidWindow	win1(0, 79, 0, 2);	
	YUVRange	cRange;
	YUVRange	soccerField(0x556A, 0x7B80, 0x7678);
	int		turn = 0;
	int		min;

	/* soccerField specifies what the carpet of the soccer field 	*/
	/* looks like to the robots camera				*/
	/* YUV is a old color encoding dating from early color TV	*/
	/* the Y field is the intensity ie the black and white signal	*/
	/* the U and V fields define a color rectangle			*/
	/* we improve this slightly					*/
	soccerField.expandY(0xFF);	// ignore intensity
	soccerField.expandU(1);		// make color rectangle wider
	soccerField.expandV(1);

	/* then we set this color rectangle in the robots #2 color bin	*/
	robot.setBin(2, soccerField);
	/* then we read it back and print it out			*/
	robot.getBin(2, cRange);
	printf("after update %s\n", cRange.say());

 	for (i = 0;i < 500;i++)
	{
		/* the getDistWhileSame looks through the camera and	*/
		/* returns in the chars of buf how many pixels look	*/
		/* like the carpet					*/
		/* if there is something in the way that char will 	*/
		/* be zero						*/
		printf("getDistMatching(2, ...) =\n");
		robot.getDistMatching(2, buf);
		/* show the user what we are seeing			*/
		for (j = 0;j < 80;j++)
			printf("%2x.", buf[j] & 0xFF);
		printf("end\n");
		/* we just look a a little section, large enough for 	*/
		/* the robot to go through				*/
		min = 100;
		for (j = 35;j < 45;j++)
			if (buf[j] >= 0 && buf[j] < min)
				min = buf[j];

		/* if there is an object in the way min will be zero	*/

		if (min > 1)
		{
			/* nothing in the way, go forward		*/
			turn = 0;
			robot.drive(40, 40);
		}
		else
		{
			/* something in the way				*/
			/* back off a bit and turn			*/
			if (!turn)
			{
				robot.drive(-30, -30);
				usleep(10);
			}
			turn = 1;		
			robot.drive(-40, 40);
		}
	}
	/* stop the robot						*/
	robot.drive(0, 0);
	printf("\n\n\n\n");

}


/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main( int argc, char *argv[])
{
	char	buf[256];

	robot.getVersion(buf);
	printf("SVR-1 version %s\n", buf);

	robot.setVideoMode(1);
	if (argc > 1)
		robot.setDebuging(atoi(argv[1]));
	srand(time(NULL));

	calibrateCam();


    	return 0;   
}
	
