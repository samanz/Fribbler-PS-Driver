#include "scribbler.h" 
#include "system.h"

#include <string>
#include <assert.h>
#include <unistd.h>

int	gDebugging = 0;



//WinSerial	port(ADDRESS);  	/* Windows with Real Robot */
//PosixSerial	port(ADDRESS);		/* Linux or Mac with Real Robot */
ClientSerial	port(ADDRESS);		/* Linux, Mac, Windows with Simulated Robot */


Scribbler       robot(port); 

//YUVRange	range(0x00FE, 0x7E8C, 0x5A78);
//YUVRange	range(0x00FE, 0x00FF, 0x00FF);
YUVRange	range(0x00FE, 0x3388, 0xBEFE);

//VidWindow	myWindow(1, 1, 1, 1); // works
VidWindow	myWindow(0, 10, 0, 10); // 



/************************************************************************/
/*									*/
/*  main								*/
/*									*/
/************************************************************************/


int main(int argc, char *argv[])
{
	char	buf[256];
	long	ret;
	int	x, y;
	Data	*data;
	int	battery;
	int	value;



	if (argc > 1)
                gDebugging = atoi(argv[1]);


	robot.setEchoMode(0);


	log("Beep\n"); 
        robot.beep(512, 1000);


// GOOD FUNCTIONS
        robot.updateScribblerSensors();

	robot.scribblerLeftIR();	
	robot.scribblerRightIR();	
	robot.scribblerLeftLight();	
	robot.scribblerCenterLight();	
	robot.scribblerRightLight();	
	robot.scribblerLineLeft();	
	robot.scribblerLineRight();	
	robot.scribblerStall();


/****************************************************************/
/* Virtual functions from Robot robot.erface			*/
/****************************************************************/
	robot.drive(100, 100);

	data = robot.takePhotoJPEG();

/****************************************************************/
/* Functions that access the underling robot, The Scribbler	*/
/****************************************************************/
        robot.beep(512, 500);
        robot.stop();
	robot.setScribblerLeftLed(1);
	robot.setScribblerCenterLed(0);
	robot.setScribblerRightLed(1);
	robot.setScribblerAllLeds(0);
	robot.setScribblerLeds(1, 0, 1);


log("Point A\n");
	robot.getScribblerData(buf);
	robot.setScribblerData(buf);

log("Point B\n");
	robot.getScribblerName(buf);
	robot.setScribblerName(buf);
	robot.getScribblerPass(buf);
	robot.setScribblerPass(buf);
log("Point C\n");
	robot.readScribblerLeftIR(value);	
	robot.readScribblerRightIR(value);	
	robot.readScribblerLeftLight(value);	
	robot.readScribblerCenterLight(value);	
	robot.readScribblerRightLight(value);	
	robot.readScribblerLineLeft(value);	
	robot.readScribblerLineRight(value);	
	robot.readScribblerStall(value);

log("Point D\n");
	log("Beep 4\n"); 
	robot.beep(512, 1000);

log("Point D1\n");



/****************************************************************/
/* Functions that access the dongle, The Fluke			*/
/****************************************************************/
/****************************************************************/
/* The Following functions have been tested some 		*/
/****************************************************************/

log("Point D2\n");
	data = robot.readJpegScan();
	log("Beep 4.1\n"); 
	robot.beep(512, 1000);
	delete data;

log("Point D3\n");
	data = robot.readJpegGrayHeader();
	delete data;

log("Point D4\n");
	data = robot.readJpegGrayScan();
	delete data;

log("Point D5\n");
	data = robot.getImage();
	log("Beep 4.2\n"); 
	robot.beep(512, 1000);
	delete data;




log("Point E\n");


	robot.setDonglePowerIR(150);
	robot.getVersion(buf);
	robot.setForwardness(0);
	log("Beep 4.3\n"); 
	robot.beep(512, 1000);


	robot.getCamParam(1, value);
	robot.setCamParam(1, value);
	robot.setWhiteBalance(1);
	log("Beep 4.4\n"); 
	robot.beep(512, 1000);



	robot.setDimmerLED(150);
	robot.setDongleLED(1);
	log("Beep 5\n"); 
	robot.beep(512, 1000);
log("Point F\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);

	robot.setEchoMode(1);

	robot.getDongleIR(value);

	data = robot.readJpegHeader();
	robot.getScribblerIR(1);
	log("Beep 6\n"); 
	robot.beep(512, 1000);
	delete data;

log("Point G\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);



// Testing Video fns
	robot.setBitmapParams( 90, 4, range);
	log("Beep 6.1\n"); 
	robot.beep(512, 1000);

// following line messes up Windows machine
	data = robot.getCompressedBitmap();
	log("Beep 6.2\n"); 
	robot.beep(512, 1000);
log("Point H\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	robot.setColorId(0, range);
	log("Beep 6.3\n"); 
	robot.beep(512, 1000);
log("Point I\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	data = robot.readJpegGrayHeader();
	log("Beep 6.4.1\n"); 
	robot.beep(512, 1000);
	delete data;


	data = robot.readJpegGrayScan();
	log("Beep 6.4.2\n"); 
	robot.beep(512, 1000);
log("Point J\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	data = robot.takePhotoGrayJPEG();
	log("Beep 6.4\n"); 
	robot.beep(512, 1000);
log("Point K\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);
	delete data;








	battery = robot.getBattery();
sprintf(logchunk, "battery = %d\n", battery);
log(logchunk);
log("Point L\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	log("Beep 7\n"); 
	robot.beep(512, 1000);



	log("Beep 8\n"); 
	robot.beep(512, 1000);
log("Point M\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	log("setWindow(1, ...)\n"); 
	robot.setWindow(1, myWindow, 2, 5);

	log("getWindowLight(1)\n"); 
	ret = robot.getWindowLight(1);
	sprintf(logchunk, " = %ld\n", ret);
	log(logchunk);
log("Point N\n");
	robot.getInfo(buf);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	log("getBlobWindow(1, ...)\n"); 
	ret = robot.getBlobWindow(1, x, y);
	sprintf(logchunk, "ret = %ld, (%d, %d)\n", ret, x, y);
	log(logchunk);


	log("getWindow(1)\n"); 
	data = robot.getWindow(1);
	if (data)
		data->talk(stdout);
	delete data;


log("Point O\n");
	robot.getInfo(buf);
	sprintf(logchunk, "ret = %ld, (%d, %d)\n", ret, x, y);
	log(logchunk);
sprintf(logchunk, "Info = <%s>\n", buf);
log(logchunk);


	log("Beep 9\n"); 
	robot.beep(512, 1000);



    	return 0;   
}
	
