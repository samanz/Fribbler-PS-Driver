#ifndef SCRIBBLER
#define SCRIBBLER

#include <stdio.h>
#include "robot.h"
#include "data.h"


#include "const.h"

extern  void 	log(char const str[]);
extern	char	logchunk[512];



const int	leftIR   = 1;
const int	centerIR = 2;
const int	rightIR  = 3;



/****************************************************************/
/* The Scribbler robot is manages a Scribbler/Fluke pair	*/
/****************************************************************/

		
class Scribbler : public Robot
{
  private:
	int	mInSync;		// is object syncronized with robot?
	int	mEchoMode;
	int	mDirection;

	VidWindow	*mWin[4];
	int 		mXstep[4];
	int		mYstep[4];


/****************************************************************/
/* The Scribbler sensors					*/
/****************************************************************/
        unsigned char   mLeftIR;
        unsigned char   mRightIR;
        unsigned int    mLeftLight;
        unsigned int    mCenterLight;
        unsigned int    mRightLight;
        unsigned char   mLineLeft;
        unsigned char   mLineRight;
        unsigned char   mStall;
                    

  protected:	int	getIntBEnd();
	long	getLongLEnd();
	int	getEcho(char ch);	// returns true if we got echo	
        int     loadScribblerSensors();
        void    send(int ch);
	int	getFinalAck(char cmd);
	int	sendScribblerCommand(char cmd, char data[8]);
  public:
        Scribbler(Serial &port);
        virtual ~Scribbler();

/****************************************************************/
/* Functions to access the local copies of Scribbler sensors	*/
/*								*/
/* could be inline but that caused Stall to not work		*/
/****************************************************************/
	int	scribblerLeftIR();	
	int	scribblerRightIR();	
	int	scribblerLeftLight();	
	int	scribblerCenterLight();	
	int	scribblerRightLight();	
	int	scribblerLineLeft();	
	int	scribblerLineRight();	
	int	scribblerStall();


/****************************************************************/
/* Virtual functions from Robot interface			*/
/****************************************************************/
	int		drive(int left, int right, int duration = 0);
	int		setColorId(int Id, YUVRange &range);
	int		getInfo(char *);
	Data		*takePhotoJPEG();


/****************************************************************/
/* Functions that access the underling robot, The Scribbler	*/
/****************************************************************/

        int     beep(int freq, int duration);
        int     stop();
	int	setScribblerLeftLed(int value);
	int	setScribblerCenterLed(int value);
	int	setScribblerRightLed(int value);
	int	setScribblerAllLeds(int value);
	int	setScribblerLeds(int left, int center, int right);
        int     updateScribblerSensors();
	int	setEchoMode(int);
	int	setScribblerData(char data[8]);
	int	getScribblerData(char data[8]);
	int	setScribblerName(char name[16]);
	int	getScribblerName(char name[]);
	int	setScribblerPass(char pass[16]);
	int	getScribblerPass(char pass[16]);
	int	readScribblerLeftIR(int &);	
	int	readScribblerRightIR(int &);	
	int	readScribblerLeftLight(int &);	
	int	readScribblerCenterLight(int &);	
	int	readScribblerRightLight(int &);	
	int	readScribblerLineLeft(int &);	
	int	readScribblerLineRight(int &);	
	int	readScribblerStall(int &);

// returns 1 to indicate clear on that side, 0 obsticle on that side	
	int	getScribblerIR(int);



/****************************************************************/
/* Functions that access the dongle, The Fluke			*/
/****************************************************************/
/****************************************************************/
/* The Following functions have been tested some 		*/
/****************************************************************/
	int 	setBitmapParams( unsigned char delay, unsigned char thresh,
			YUVRange &range);
	Data	*getCompressedBitmap();


	Data	*readJpegHeader();
	Data	*readJpegScan();
	Data	*readJpegGrayHeader();
	Data	*readJpegGrayScan();
	Data	*getImage();

	Data	*takePhotoGrayJPEG();

// returns the number of pulses recieved
// high value indicates obsticle near
// low values indicates obsticle far away
	int	getDongleIR(int);

	int	setDonglePowerIR(int);
	int	getVersion(char *ver);
	int	setForwardness(int d);

	int	getCamParam(int id, int &val);
	int	setCamParam(int id, int val);
	int	setWhiteBalance(int );

	int	getBattery();
	int	setDimmerLED(int);
	int	setDongleLED(int d);


/****************************************************************/
/* The Following functions have not been tested yet NOT TESTED	*/
/* most of them put Fluke in a unresponsive mode		*/
/****************************************************************/
	int	setWindow(int id, VidWindow &win,
			  int xstep, int ystep);
	Data	*getWindow(int id);
	long	getWindowLight(int win);
	int	getBlob(int &x, int &y);
	int	getBlobWindow(int wind, int &x, int &y);



/****************************************************************/
/* The Following functions have not been implemented on Fluke	*/
/****************************************************************/
//	int	setEmittersIR(int);
//	int	getMessageIR(char *buf);
//	int	sendMessageIR(char const buf[]);

};

#endif
