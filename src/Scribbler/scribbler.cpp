/************************************************************************/
/* This software was developed at the RoboLab of Brooklyn College 	*/
/* by John Cummins under the supervision and inspiration of 		*/
/* Professors Sklar and Parsons						*/
/************************************************************************/
#include "scribbler.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

extern 	int	gDebugging;
const	int	thisModule  = 0x0008;

void log(char const str[])
{
	static	int firstTime = 1;

	if (firstTime)
	{
		unlink("log1");
		firstTime = 0;
	}
	FILE	*fd = fopen("log1", "a");

	fprintf(fd, "%s", str);

	fclose(fd);
}
char	logchunk[512];


/************************************************************************/
/* A object of Scribbler class manages the dialog with a Fluke          */
/* equipped scribbler robot and serves as a stand in for that robot.    */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
Scribbler::Scribbler(Serial &port) : Robot(port)
{
	mInSync = 0;

/************************************************************************/
/* Set initial state of the robot					*/
/* echoMode is on and direction is Scribbler forward			*/
/************************************************************************/
	setEchoMode(1);
	setForwardness(0);

	for (int i = 0;i < 4;i++)
		mWin[i] = 0;
}
Scribbler::~Scribbler()
{
}
/************************************************************************/
/* send									*/
/* Send a character to robot						*/
/* all communication to robot goes through here				*/
/************************************************************************/
void    Scribbler::send(int ch)
{
       	char   c = ch & 0x00FF;

        if (!mPort.takeChar(c))
	{
		if (gDebugging & thisModule)
			fprintf(stderr, "send(%d) failed\n", ch);
	}
}

/************************************************************************/
/* getIntBEnd								*/
/* read a Big Endian int from the robot 				*/
/************************************************************************/	
int	Scribbler::getIntBEnd()
{
	char	hiByte, loByte;
	int	ret;

	hiByte = getCharBlocking();
	loByte = getCharBlocking();

	ret = (hiByte << 8) | loByte;

	return ret;
}
/************************************************************************/
/* getLongLEnd								*/
/* read a Little Endian long from the robot 				*/
/************************************************************************/	
long	Scribbler::getLongLEnd()
{
	char	buf[4];
	int	i;
	long	ret;

	for (i = 0;i < 4;i++)
		buf[i] = getCharBlocking();

	ret = 0;
	for (i = 3;i != -1;i--)
	{
		ret <<= 8;
		ret |= buf[i];
	}
	
	return ret;
}
/************************************************************************/
/* The virtual functions defined in the Robot interface			*/
/* 									*/
/************************************************************************/

/************************************************************************/
/* drive - the basic motion command					*/
/* left, and right in range -100 to +100, negative means reverse	*/
/* the speed of the wheels						*/
/* duration of zero (the default) means unlimited 			*/
/* that is until the next drive command					*/
/* in hundredths of a second						*/
/************************************************************************/
int     Scribbler::drive(int left, int right, int duration)
{	char	data[8];

	memset(data, '*', 8);
        data[0] = left  + 100;
	data[1] = right + 100;

        if (!sendScribblerCommand(SET_MOTORS, data))
		return 0;

        if (!loadScribblerSensors())
		return 0;

	if (!getFinalAck(SET_MOTORS))
		return 0;

	if (duration)
	{		usleep(duration * 10000);
		return stop();
	}
	return 1;
}
int	Scribbler::setColorId(int id, YUVRange &range)
{
	if (id != 0)
		return 0;
	return setBitmapParams(90, 4, range);
}
/************************************************************************/
/* getInfo								*/
/* get a string describing the Fluke software version			*/
/************************************************************************/	
int	Scribbler::getInfo(char *buf)
{
	int	ch;
        char    data[8];
        
        memset(data, '*', 8);
        if (!sendScribblerCommand(GET_INFO, data))
                return 0;
       
	try
	{
		ch = getCharBlocking();	        while (ch != 0x0A)
	        {
	                *buf++ = ch; 
	                ch = getCharBlocking();
	        }

	        *buf = '\0';
	}
	catch (DataError err)
	{
		fprintf(stderr, "DataError in getInfo\n");
		return 0;
	}

        return getFinalAck(GET_INFO);
}
/************************************************************************/
/* takePhotoJPEG							*/
/* have to Fluke take a photo and return it in JPEG format to host	*/
/************************************************************************/
Data	*Scribbler::takePhotoJPEG()
{
	Data	*jpeg;
	Data	*body;

	jpeg = readJpegHeader();
	if (!jpeg)
		return 0;
	body = readJpegScan();
	if (!body)
	{
		delete jpeg;
		return 0;
	}
	jpeg->append(*body);
	delete body;

	return jpeg;
}

/************************************************************************/
/* Functions that access the underling robot, The Scribbler		*/
/************************************************************************/
/************************************************************************/
/* sendScribblerCommand							*/
/* manages the common dialog between host and Scribbler with Fluke 	*/
/* acting as a pipeline							*/
/* Scribbler blocks are 9 bytes with the first being the command and	*/
/* the rest data bytes.							*/
/* The scribbler echos back all bytes it receives and we check them	*/
/* with what we sent							*/
/* After successful completion of sendScribblerCommand the port is	*/
/* ready to process the scribblers response				*/
/************************************************************************/
int	Scribbler::sendScribblerCommand(char cmd, char data[8])
{
	int	ch;

	flushInputBuffer();
	mInSync = 0;
	send(cmd);
	for (int i = 0;i < 8;i++)
		send(data[i]);
	flushOutputBuffer();

	try
	{
        	ch = getCharBlocking();
		if (ch != cmd)
		{
			if (gDebugging & thisModule)
			{
				fprintf(stderr, "bad first ack\n");
				fprintf(stderr, "   should be %d is %d\n", cmd, ch);
			}
		}
		for (int i = 0;i < 8;i++)
		{
			ch = getCharBlocking();
        	        /* N.B. Fluke messes with SET_MOTORS */
        	        if (ch != data[i] && cmd != SET_MOTORS)
			{
        	                if (gDebugging & thisModule)
				{
					fprintf(stderr, "bad command %d echo of byte %d",
							cmd, i);
        	                        fprintf(stderr, "   should be 0x%x is 0x%x\n", data[i], ch);
				}
			}
		}
		mInSync = 1;
	}
	catch (DataError err)
	{
		return 0;
	}	
	return 1;
}
/************************************************************************/
/* beep									*/
/* have the Scribbler emit a tone					*/
/************************************************************************/		
int     Scribbler::beep(int freq, int duration)
{
        char    data[8];

//	flushInputBuffer();

        memset(data, '*', 8);
        data[0] = duration >> 8;
        data[1] = duration & 0x00FF;
        data[2] = freq >> 8;
        data[3] = freq & 0x00FF;
        if (!sendScribblerCommand(SET_SPEAKER, data))
		return 0;

        if (!loadScribblerSensors())
                return 0;

        return getFinalAck(SET_SPEAKER);
}
int     Scribbler::getFinalAck(char cmd)
{
	int	ch;

	if (!mEchoMode)
		return 1;

	try
	{
        	ch = getCharBlocking();
	}
	catch (DataError err)
	{
		fprintf(stderr, "DataError in getFinalAck\n");
		return 0;
	}
        if (ch != cmd)
	{
		if (gDebugging & thisModule)	
		{	
                	fprintf(stderr, "bad final ack\n");
			fprintf(stderr, "   should be %d is %d\n", cmd, ch);
		}
		return 0;
	}
	return 1;
}
/************************************************************************/
/* updateScribblerSensors						*/
/* refresh the local values of the Scribbler sensors			*/
/************************************************************************/
int     Scribbler::updateScribblerSensors()
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_ALL, data))
		return 0;

        if (!loadScribblerSensors())
		return 0;

	return getFinalAck(GET_ALL);
}
/************************************************************************/
/* loadScribblerSensors							*/
/* load the local values of the Scribbler sensors			*/
/* many Scribbler functions (the set functions and the drive functions)	*/
/* return the values of the scribbler sensors				*/
/************************************************************************/
int     Scribbler::loadScribblerSensors()
{
	try
	{
	        mLeftIR = getCharBlocking();
	        mRightIR = getCharBlocking();

	        mLeftLight = getCharBlocking();
	        mLeftLight <<= 8;
	        mLeftLight += getCharBlocking();
	        mCenterLight = getCharBlocking();
	        mCenterLight <<= 8;
	        mCenterLight += getCharBlocking();
	        mRightLight = getCharBlocking();
	        mRightLight <<= 8;
	        mRightLight += getCharBlocking(); 

	        mLineLeft = getCharBlocking();
	        mLineRight = getCharBlocking();
	        mStall = getCharBlocking();

		if (gDebugging & thisModule)
		{
			if (mStall)
				printf("mStall = %d\n", mStall);
		}
	}
	catch (DataError err)
	{
		fprintf(stderr, "DataError in loadScribblerSensors\n");
		return 0;
	}
	if (gDebugging & thisModule)
	{
        	fprintf(stderr, "Sensors %3d, %3d, %5d, %5d, %5d, %3d, %3d, %3d\n",
                mLeftIR, mRightIR, mLeftLight, mCenterLight, mRightLight,
                mLineLeft, mLineRight, mStall);
	}

        return 1;


}
/************************************************************************/
/* scribblerLeftIR							*/
/* returns local value of Scribbler sensor				*/
/* set by all SET commands and readAllScribblerSensors			*/
/************************************************************************/
int	Scribbler::scribblerLeftIR()	
{ 
	return mLeftIR; 
}
int	Scribbler::scribblerRightIR()	
{ 
	return mRightIR; 
}
int	Scribbler::scribblerLeftLight()	
{ 
	return mLeftLight; 
}
int	Scribbler::scribblerCenterLight()	
{ 
	return mCenterLight; 
}
int	Scribbler::scribblerRightLight()	
{ 
	return mRightLight; 
}
int	Scribbler::scribblerLineLeft()	
{ 
	return mLineLeft; 
}
int	Scribbler::scribblerLineRight()	
{ 
	return mLineRight; 
}
int	Scribbler::scribblerStall()
{ 
	return mStall; 
}
int	Scribbler::readScribblerLeftIR(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_IR_LEFT, data))
		return 0;

 	value = mLeftIR = getCharBlocking(); 

	return getFinalAck(GET_IR_LEFT);
}
	
int	Scribbler::readScribblerRightIR(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_IR_RIGHT, data))
		return 0;

 	value = mRightIR = getCharBlocking(); 

	return getFinalAck(GET_IR_RIGHT);
}	
int	Scribbler::readScribblerLeftLight(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_LIGHT_LEFT, data))
		return 0;

 	value = mLeftLight = getIntBEnd(); 

	return getFinalAck(GET_LIGHT_LEFT);
}
	
int	Scribbler::readScribblerCenterLight(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_LIGHT_CENTER, data))
		return 0;

 	value = mCenterLight = getIntBEnd(); 

	return getFinalAck(GET_LIGHT_CENTER);
}	
int	Scribbler::readScribblerRightLight(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_LIGHT_RIGHT, data))
		return 0;

 	value = mRightLight = getIntBEnd(); 

	return getFinalAck(GET_LIGHT_RIGHT);
}	
int	Scribbler::readScribblerLineLeft(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_LINE_LEFT, data))
		return 0;

 	value = mLineLeft = getCharBlocking(); 

	return getFinalAck(GET_LINE_LEFT);
}		
int	Scribbler::readScribblerLineRight(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_LINE_RIGHT, data))
		return 0;

 	value = mLineRight = getCharBlocking(); 

	return getFinalAck(GET_LINE_RIGHT);
}		
int	Scribbler::readScribblerStall(int &value)
{
	char	data[8];

	memset(data, '*', 8);
	if (!sendScribblerCommand(GET_STALL, data))
		return 0;

 	value = mStall = getCharBlocking(); 

	return getFinalAck(GET_STALL);
}	


/************************************************************************/
/* setEchoMode								*/
/* Turn on or off the echo mode						*/
/* if echo mode is on Scribbler echos the command code after processing	*/
/* if echo mode is on Scribbler does not echo the command after		*/
/* processing								*/
/************************************************************************/
int	Scribbler::setEchoMode(int mode)
{
	char	data[8];

	memset(data, '*', 8);
	data[0] = mode ? 1 : 0;

	if (!sendScribblerCommand(SET_ECHO_MODE, data))
		return 0;
	mEchoMode = data[0];
	if (gDebugging & thisModule)
	{
		fprintf(stderr, "echoMode set %d\n", mEchoMode);
	}

        if (!loadScribblerSensors())
		return 0;

	return getFinalAck(SET_ECHO_MODE);
}

/************************************************************************/
/* stop									*/
/* stops both motors							*/
/* same effect as drive(0, 0)						*/
/************************************************************************/
int Scribbler::stop()
{	char	data[8];

	memset(data, '*', 8);
        if (!sendScribblerCommand(SET_MOTORS_OFF, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(SET_MOTORS_OFF);
}
/************************************************************************/
/* setScribblerLeds							*/
/* Turns on/off the three LEDS on the back of the Scribbler		*/
/* Note: left/right in Scribbler sense not Fluke sense			*/
/************************************************************************/
int 	Scribbler::setScribblerLeds(int left, int center, int right)
{
	char	data[8];

	memset(data, '*', 8);
	data[0] = left   ? 1 : 0;
	data[1] = center ? 1 : 0;
	data[2] = right  ? 1 : 0;
        if (!sendScribblerCommand(SET_LED_ALL, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(SET_LED_ALL);
}
/************************************************************************/
/* setScribblerLeftLed							*/
/* Turns on/off the left LED on the back of the Scribbler		*/
/* Note: left/right in Scribbler sense not Fluke sense			*/
/************************************************************************/
int	Scribbler::setScribblerLeftLed(int value)
{
	char	data[8];
	char	cmd;

	memset(data, '*', 8);
	cmd = value ? SET_LED_LEFT_ON : SET_LED_LEFT_OFF;

        if (!sendScribblerCommand(cmd, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(cmd);
}
/************************************************************************/
/* setScribblerCenterLed						*/
/* Turns on/off the center LED on the back of the Scribbler		*/
/* Note: left/right in Scribbler sense not Fluke sense			*/
/************************************************************************/
int	Scribbler::setScribblerCenterLed(int value)
{
	char	data[8];
	char	cmd;

	memset(data, '*', 8);
	cmd = value ? SET_LED_CENTER_ON : SET_LED_CENTER_OFF;

        if (!sendScribblerCommand(cmd, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(cmd);
}
/************************************************************************/
/* setScribblerRightLed							*/
/* Turns on/off the left LED on the back of the Scribbler		*/
/* Note: left/right in Scribbler sense not Fluke sense			*/
/************************************************************************/
int	Scribbler::setScribblerRightLed(int value)
{
	char	data[8];
	char	cmd;

	memset(data, '*', 8);
	cmd = value ? SET_LED_RIGHT_ON : SET_LED_RIGHT_OFF;


        if (!sendScribblerCommand(cmd, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(cmd);
}
/************************************************************************/
/* setScribblerAllLeds							*/
/* Turns on/off all three of the  LEDs on the back of the Scribbler	*/
/************************************************************************/
int	Scribbler::setScribblerAllLeds(int value)
{
	char	data[8];
	char	cmd;

	memset(data, '*', 8);
	cmd = value ? SET_LED_ALL_ON : SET_LED_ALL_OFF;

        if (!sendScribblerCommand(cmd, data))
		return 0;
        
        if (!loadScribblerSensors())
		return 0;

        return getFinalAck(cmd);
}

/************************************************************************/
/* The Scribbler has 8 bytes available for customizing			*/
/* currently IPRE uses the first 4 bytes to try the match the motors	*/
/* The following functions access these bytes				*/
/************************************************************************/
/************************************************************************/
/* setScribblerData							*/
/* writes the data bytes						*/
/************************************************************************/
int	Scribbler::setScribblerData(char data[8])
{
       	if (!sendScribblerCommand(SET_DATA, data))
		return 0;

       	if (!loadScribblerSensors())
		return 0;

	return getFinalAck(SET_DATA);
}
/************************************************************************/
/* setScribblerName							*/
/* Scribbler has a Name upto 16 bytes					*/
/* This function writes the Name					*/
/************************************************************************/
int	Scribbler::setScribblerName(char name[16])
{
       	if (!sendScribblerCommand(SET_NAME1, name))
		return 0;

      	if (!loadScribblerSensors())
		return 0;

 
        if (!getFinalAck(SET_NAME1))
		return 0;

      	if (!sendScribblerCommand(SET_NAME2, name + 8))
		return 0;

      	if (!loadScribblerSensors())
		return 0;

        return getFinalAck(SET_NAME2);
}
/************************************************************************/
/* setScribblerName							*/
/* Scribbler has a Name upto 16 bytes					*/
/* This function reads the Name						*/
/* BUG! Due to a bug in the Scribbler firmware GET_NAME1 and GET_NAME2	*/
/* do not return the correct FinalAck					*/
/* instead the ack byte is the first byte of their data			*/
/* not sure what I want to do about this				*/
/************************************************************************/
int	Scribbler::getScribblerName(char name[])
{
	char	data[8];

	memset(data, '*', 8);
    	if (!sendScribblerCommand(GET_NAME1, data))
		return 0;

	for (int i = 0;i < 8;i++)
		name[i] = getCharBlocking(); 

//        getFinalAck(GET_NAME1);
	getFinalAck(name[0]);

      	if (!sendScribblerCommand(GET_NAME2, data))
		return 0;

	for (int i = 0;i < 8;i++)
		name[i + 8] = getCharBlocking(); 

//        getFinalAck(GET_NAME2);
	getFinalAck(name[8]);
	name[16]='\0';

	return 1;
}
int	Scribbler::setScribblerPass(char pass[16])
{
     	if (!sendScribblerCommand(SET_PASS1, pass))
		return 0;

      	if (!loadScribblerSensors())
		return 0;
 
        if (!getFinalAck(SET_PASS1))
		return 0;

      	if (!sendScribblerCommand(SET_PASS2, pass + 8))
		return 0;

      	if (!loadScribblerSensors())
		return 0;
 
        return getFinalAck(SET_PASS2);
}
/************************************************************************/
/* setScribblerPass							*/
/* 									*/
/* This function writes the Pass					*/
/* BUG! Due to a bug in the Scribbler firmware GET_PASS1 and GET_PASS2	*/
/* do not return the correct FinalAck					*/
/* instead the ack byte is the first byte of their data			*/
/* not sure what I want to do about this				*/
/************************************************************************/
int	Scribbler::getScribblerPass(char pass[16])
{
	char	data[8];

	memset(data, '*', 8);
     	if (!sendScribblerCommand(GET_PASS1, data))
		return 0;

	for (int i = 0;i < 8;i++)
		pass[i] = getCharBlocking(); 

//        getFinalAck(GET_PASS1);
	getFinalAck(pass[0]);

      	if (!sendScribblerCommand(GET_PASS2, data))
		return 0;

	for (int i = 0;i < 8;i++)
		pass[i + 8] = getCharBlocking(); 

//        getFinalAck(GET_PASS2);
	getFinalAck(pass[8]);

	return 1;
}
		
/************************************************************************/
/* getScribblerData							*/
/* reads the data bytes							*/
/* BUG! Due to a bug in the Scribbler firmware GET_DATA			*/
/* do not return the correct FinalAck					*/
/* instead the ack byte is the first byte of their data			*/
/* not sure what I want to do about this				*/
/************************************************************************/
/************************************************************************/
int	Scribbler::getScribblerData(char ipre_data[8])
{
	char	data[8];

	memset(data, '*', 8);
      	if (!sendScribblerCommand(GET_DATA, data))
		return 0;

	for (int i = 0;i < 8;i++)
		ipre_data[i] = getCharBlocking(); 

//        getFinalAck(GET_DATA);
	getFinalAck(ipre_data[0]);

	return 1;
}
/****************************************************************/
/* Functions that access the dongle, The Fluke			*/
/****************************************************************/
/****************************************************************/
/* Set the orientation of the scribbler				*/
/* 0: Scribbler forward						*/
/* 1: Fluke forward						*/
/****************************************************************/
int	Scribbler::setForwardness(int d)
{
	send(SET_FORWARDNESS);
	send(d & 0xFF);
	flushOutputBuffer();
	mDirection = d;

	return 1;	
}
int	Scribbler::setDongleLED(int d)
{
	if (d)
		send(SET_DONGLE_LED_ON);
	else
		send(SET_DONGLE_LED_OFF);
	flushOutputBuffer();

	return 1;
}
/****************************************************************/
/* getVersion							*/
/* get s string describing the version of the Fluke		*/
/****************************************************************/
int	Scribbler::getVersion(char *buf)
{
	int	ch;
	char	*s = buf;

	send(GET_VERSION);
	flushOutputBuffer();
	try
	{
		ch = getCharBlocking();        	while (ch != 0x0A)
        	{
                	*s++ = ch; 
                	ch = getCharBlocking();
		}
 	        *s = '\0';
		return 1;
	}
	catch (DataError err)
	{
		*s = '\0';
	}


	return 1; 
}
/****************************************************************/
/* setWhiteBalance						*/
/* pretend we know what this does				*/
/****************************************************************/
int	Scribbler::setWhiteBalance(int bal)
{
	switch (bal)
	{
	  case 0:
		send(SET_NO_WHITE_BALANCE);
		break;
	  case 1:
		send(SET_WHITE_BALANCE);
		break;
	  default:
		printf("bad parameter in setWhiteBallance(%d)\n", bal);
		return 0;
	}
	flushOutputBuffer();
	return 1;
}

#ifdef UNDEFINED
/****************************************************************/
/* setEmittersIR						*/
/* set the IR fluke IR emitters value				*/
/****************************************************************/
int	Scribbler::setEmittersIR(int emitters)
{
	send(SET_IR_EMITTERS);
	send(emitters);
	flushOutputBuffer();


	return 1;
}
int	Scribbler::getMessageIR(char *buf)
{
	int	len;
	int	ch;

	send(GET_IR_MESSAGE);
	flushOutputBuffer();

	ch = getCharBlocking();
	assert(ch == GET_IR_MESSAGE);
	try
	{
/*
		len = getIntBEnd();
printf("getMessageIR len = %d\n", len);
		for (int i;i < len;i++)
		{
			buf[i] = getCharBlocking();
		}
 */
		return 1;
	}
	catch (DataError err)
	{
		return 0;
	}
}

/****************************************************************/
/* sendMessageIR						*/
/* send a message via IR					*/
/****************************************************************/
int	Scribbler::sendMessageIR(char const buf[])
{
	int len = strlen(buf);

	send(SEND_IR_MESSAGE);
	send(len & 0xFF);
	for (int i = 0;i < len;i++)
	{
		send(buf[i]);
	}
	flushOutputBuffer();

// what is the reply?
		
	int	ch;

// we get back the message we sent	
	ch = getCharBlocking();
	assert(ch == SEND_IR_MESSAGE);
	ch = getCharBlocking();
	assert(ch == len);
	for (int i = 0;i < len;i++)
	{
		ch = getCharBlocking();
		assert(ch == buf[i]);
	}

// then one byte either 0 or 1	
	ch = getCharBlocking();
	assert(ch == 0 || ch == 1);
// and then the command
	ch = getCharBlocking();
	assert(ch == SEND_IR_MESSAGE);

printf("good sendMessageIR reply\n");


	return 1;
}
#endif
int	Scribbler::getCamParam(int id, int &val)
{
	send(GET_CAM_PARAM);
	send(id & 0xFF);
	flushOutputBuffer();
	val = getCharBlocking();

	return 1;
}
int	Scribbler::setCamParam(int id, int val)
{
	send(SET_CAM_PARAM);
	send(id & 0xFF);
	send(val & 0xFF);
	flushOutputBuffer();
	usleep(200000);

	return 1;
}
/************************************************************************/
/* getImage								*/
/* returns a large Data object (about 50K Bytes) specifing the image	*/
/* (not sure of the format and don't expect this to be used much)	*/
/************************************************************************/
Data	*Scribbler::getImage()
{
	int	i;
	int	ch;
	Data	*data;

	mInSync = 0;
	flushInputBuffer();

	send(GET_IMAGE);
	flushOutputBuffer();
	data = new Data(256*192);
	try
	{
		for (i = 0;i < 256*192;i++)
		{
			ch = getCharBlocking();
			data->append(ch);
		}
		return data;
	}
	catch (DataError err)
	{
		if (gDebugging & thisModule)
		{
			fprintf(stderr, "Data Error in getImage char #%d\n", i);
		}
//		delete data;
//		return 0;
		return data;
	}	
}
Data	*Scribbler::readJpegHeader()
{
	Data	*data;

	mInSync = 0;
	flushInputBuffer();

	send(GET_JPEG_COLOR_HEADER);
	flushOutputBuffer();
	data = new Data();
	try
	{
		data->read(mPort, LITTLENDIAN);
		return data;
	}
	catch (DataError err)
	{
		delete data;
		return 0;
	}
}
Data	*Scribbler::readJpegScan()
{
	unsigned char	ch;
	unsigned char	lastChar = 0;

	Data	*scan;
	
	send(GET_JPEG_COLOR_SCAN);
	send(1); // reliable
	flushOutputBuffer();

	scan = new Data();
	try
	{
		for (;;)
		{
			ch = getCharBlocking();
			scan->append(ch);
			if (ch == 0xD9 && lastChar == 0xFF)
				break;
			lastChar = ch;
		}
	}
	catch (DataError err)
	{
		delete scan;
		return 0;
	}
	return scan;
}
Data	*Scribbler::readJpegGrayHeader()
{
	Data	*data;

	mInSync = 0;
	flushInputBuffer();

	send(GET_JPEG_GRAY_HEADER);
	flushOutputBuffer();
	data = new Data();
	try
	{
		data->read(mPort, LITTLENDIAN);
		return data;
	}
	catch (DataError err)
	{
		delete data;
		return 0;
	}
}
Data	*Scribbler::readJpegGrayScan()
{
	unsigned char	ch;
	unsigned char	lastChar = 0;

	Data	*scan;
	
	send(GET_JPEG_GRAY_SCAN);
	send(1); // reliable
	flushOutputBuffer();

	scan = new Data();
	try
	{
		for (;;)
		{
			ch = getCharBlocking();
			scan->append(ch);
			if (ch == 0xD9 && lastChar == 0xFF)
				break;
			lastChar = ch;
		}
	}
	catch (DataError err)
	{
		delete scan;
		return 0;
	}
	return scan;
}
Data	*Scribbler::takePhotoGrayJPEG()
{
	Data	*jpeg;
	Data	*body;

	jpeg = readJpegGrayHeader();
	if (!jpeg)
		return 0;
	body = readJpegGrayScan();
	if (!body)
	{
		delete jpeg;
		return 0;
	}
	jpeg->append(*body);
	delete body;

	return jpeg;
}

/* N.B. delay should always be 90 or "Bad Things Happen" Keith O'Hara	*/
int 	Scribbler::setBitmapParams(unsigned char delay,
			  unsigned char thresh, YUVRange &range	)
{
	unsigned char	from, to;

	if (delay != 90)
		delay = 90;

	send(SET_RLE);
	send(delay);
	send(thresh);
	range.getY(from, to);
	send(from);
	send(to);	
	range.getU(from, to);
	send(from);
	send(to);
	range.getV(from, to);
	send(from);
	send(to);
	flushOutputBuffer();

	return 1;
}
/************************************************************************/
/* getCompressedBitmap							*/
/* user the params set in setBitmapParams to construct a 		*/
/* Black-and-White Image. This is then compressed by Run Length Encoding*/
/* and returned as Data Object typically a few hundred bytes at most	*/
/************************************************************************/
Data	*Scribbler::getCompressedBitmap()
{
	send(GET_RLE);
	flushOutputBuffer();
	try 
	{
		return new Data(mPort, BIGENDIAN);
	}
	catch (DataError err) 
	{
		return 0;
	}
}
/************************************************************************/
/* setWindow								*/
/* The Fluke supports 4 "windows", rectangular areas of the screen	*/
/* and some functions restrict their attention to one of these windows	*/
/* Note the step parameters which allow restricting the bytes sampled	*/
/* We keep the parameters to calculate the amount of data returned	*/
/* by window functions							*/
/************************************************************************/
int 	Scribbler::setWindow(int id, VidWindow &win, 
			     int xstep, int ystep)
{
	unsigned char	loX, hiX;
	unsigned char	loY, hiY;
	
	if (id < 0 || id > 3)
		id = 0;

	send(SET_WINDOW);
	send(id);
	win.getX(loX, hiX);
	win.getY(loY, hiY);
	send(loX);
	send(191 - hiY);
	send(hiX);
	send(191 - loY);	
	send(xstep);
	send(ystep);
	flushOutputBuffer();

	if (!mWin[id])
		delete mWin[id];
	mWin[id] = new VidWindow(win);
	mXstep[id] = xstep;
	mYstep[id] = ystep;

	return 1;
}
/************************************************************************/
/* getWindow								*/
/* returns a Data object describing the state of the previously specifed*/
/* window.								*/
/* Note it is always an error to access a window if that window		*/
/* has not been set via setWindow					*/
/************************************************************************/
Data	*Scribbler::getWindow(int id)
{
	unsigned char	ch;
	Data	*data;
	unsigned char	loX, hiX;
	unsigned char	loY, hiY;
	int	hight, width;
	int	len;
	int	i;

	if (id < 0 || id > 3)
		id = 0;

	if (!mWin[id])
	{
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "getWindow(%d) window not set\n", id);
			log(logchunk);
		}
		return 0;
	}

	send(GET_WINDOW);
	send(id);
	flushOutputBuffer();
	
	mWin[id]->getX(loX, hiX);
	mWin[id]->getY(loY, hiY);
	hight = hiY - loY;
	width = hiX - loX;
	len = (hight/mYstep[id] + 1)*(width/mXstep[id] + 1);
	data = new Data(len);

	try
	{
		for (i = 0;i < len;i++)
		{
			mPort.getCharBlocking(ch);
			data->append(ch);
		}

		return data;
	}
	catch (DataError err)
	{
		if (gDebugging & thisModule)
		{
			fprintf(stderr, "DataError in getWindow(%d)\n", id);
			fprintf(stderr, "only %d bytes read\n", i);
		}

		delete data;
		return 0;
	}
	return data;
}
/************************************************************************/
/* getDongleIR								*/
/* returns the number of IR hits received when sending IR pulses	*/
/* in a specified direction.						*/
/* Note. Left and Right are from the Dongles point of view		*/
/* which is the reverse of the Scribblers				*/
/************************************************************************/
int	Scribbler::getDongleIR(int which)
{
	char	cmd;
	int	hits = 0;

	switch (which)
	{
	  case leftIR:
		send(cmd = GET_DONGLE_L_IR);
		break;
	  case centerIR:
		send(cmd = GET_DONGLE_C_IR);
		break;
	  case rightIR:
		send(cmd = GET_DONGLE_R_IR);
		break;
	  default:
		return -1;
	}
	flushOutputBuffer();


	try
	{
		hits = getIntBEnd();
	}
	catch (DataError err)
	{
		return -1;
	}

	return hits;
}
/************************************************************************/
/* setDonglePowerIR							*/
/* sets the power to be used for the IR emitter				*/
/* if it is too low  getDongleIR will always return 0			*/
/* if it is too high getDongleIR will always return a large value	*/
/* a short range of values around 132 getDongleIR returns useful values	*/
/************************************************************************/
int	Scribbler::setDonglePowerIR(int power)
{
	send(SET_DONGLE_IR);
	send(power & 0xFF);
	flushOutputBuffer();

	return 1;
}
int	Scribbler::getBlob(int &x, int &y)
{
	int	hits;

	send(GET_BLOB);
	flushOutputBuffer();
	try
	{
		hits = getIntBEnd();

		x = getCharBlocking();
		y = getCharBlocking();
	}
	catch (DataError err)
	{
		return -1;
	}
	return hits;
}

int	Scribbler::getBlobWindow(int id, int &x, int &y)
{
	int	hits;

	if (!mWin[id])
	{
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "getBlobWindow(%d) window not set\n", id);
			log(logchunk);
		}
		return 0;
	}

	send(GET_BLOB_WINDOW);
	send(id & 0xFF);
	flushOutputBuffer();

	try
	{

		hits = getIntBEnd();
/* I think the following are the coordinates of the	*/
/* center of gravity of the blob			*/
		x = getCharBlocking();
		y = getCharBlocking();

	}
	catch (DataError err)
	{
		return -1;
	}
	return hits;
}

long	Scribbler::getWindowLight(int id)
{
	unsigned char	ch;
	int		i;
	long		hits;

	if (!mWin[id])
	{
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "getWindowLight(%d) window not set\n", id);
			log(logchunk);
		}
		return 0;
	}

	send(GET_WINDOW_LIGHT);
	send(id & 0xFF);
	flushOutputBuffer();

	try
	{
		hits = 0;

		for (i = 0;i < 3;i++)
		{
			ch = getCharBlocking();
			hits <<= 8;
			hits |= ch;
		}

		
		return hits;
	}
	catch (DataError err)
	{
		return -1;
	}	
}


int	Scribbler::getBattery()
{
	send(GET_BATTERY);
	flushOutputBuffer();

	try
	{
		return getIntBEnd();
	}
	catch (DataError err)
	{
		return -1;
	}
}
 
int	Scribbler::setDimmerLED(int val)
{
	send(SET_DIMMER_LED);
	send(val & 0xFF);
	flushOutputBuffer();

// nothing comes back so we trust

	return 1;
}

/****************************************************************/
/* getScribblerIR read the Scribblers IR sensors		*/
/* returns 1 to indicate clear 					*/
/* returns 0 to indicate obstacle is near			*/
/****************************************************************/
int	Scribbler::getScribblerIR(int ir)
{
	if (!updateScribblerSensors())
		return -1;

	switch (ir)
	{
	  case leftIR:
		return mLeftIR; 
	  case centerIR:
		break;
	  case rightIR:
		return mRightIR;
	}
	return -1;
}

		



		
		




		
/************************************************************************/
