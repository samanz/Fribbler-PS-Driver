#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>




int	gDebugging = 0;

//extern int	sleep(int);

// For Mac
//#define ADDRESS         "/dev/tty.IPRE6-245782-DevB-1"

// For Windows XP
//#define ADDRESS         "com9"

// For Linux
//#define ADDRESS		"/dev/rfcomm0"


//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
//PosixSerial	port(ADDRESS);		/* Linux or Mac with Real Robot */
ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */



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
	char	buf[256];
	Data	*jpeg;
	Data	*data;
	FileOut	*file;

 
	srand(time(0));
       
	if (argc > 1)
                gDebugging = atoi(argv[1]);


	robot.setForwardness(0); // Scribbler Forward

        robot.beep(512, 500);

	printf("GetVersion\n");
	robot.getVersion(buf);
	printf("Version = <%s>\n", buf);

	data = robot.takePhotoJPEG();
	if (data)
	{
		printf("got a jpeg photo - save it\n");
		file = new FileOut("tryphoto.jpeg");
		/* just save the data so we can examine the picture	*/
		data->write(*file,  0);
		delete data;
		delete file;
	}

	data = robot.readJpegHeader();
	if (data)
	{
		printf("got a jpeg header - save it\n");
		file = new FileOut("trychead.dat");
		data->write(*file,  LITTLENDIAN);
		delete data;
		delete file;
	}

	data = robot.readJpegScan();
	if (data)
	{
		printf("got a jpeg scan - save it\n");
		file = new FileOut("trycscan.dat");
		data->write(*file,  LITTLENDIAN);
		delete data;
		delete file;
	}


	printf("readJpegGrayHeader()\n");
	jpeg = robot.readJpegGrayHeader();
	if (jpeg)
	{
//		jpeg->talk(stdout);
		printf("got a gray jpeg header - save it\n");
		file = new FileOut("tryghead.dat");
		jpeg->write(*file,  LITTLENDIAN);
		delete jpeg;
		delete file;
	}

	printf("readJpegGrayScan()\n");
	jpeg = robot.readJpegGrayScan();
	if (jpeg)
	{
		jpeg->talk(stdout);
		printf("got a gray jpeg scan - save it\n");
		file = new FileOut("trygscan.dat");
		jpeg->write(*file,  LITTLENDIAN);
		delete jpeg;
		delete file;
	}


	printf("getImage()\n");
	data = robot.getImage();
	if (data)
	{
		printf("got a Image - save it\n");
		file = new FileOut("tryimage.dat");
		data->write(*file,  LITTLENDIAN);
		delete data;
		delete file;
	}


	printf("getCompressedBitmap\n");
	data = robot.getCompressedBitmap();
	if (data)
	{
		printf("got a compressed bitmap - save it\n");
		file = new FileOut("trybitmap.dat");
		data->write(*file, LITTLENDIAN);
		delete data;
		delete file;
	}
 


	printf("Beep\n"); 
	robot.beep(512, 1000);

}

