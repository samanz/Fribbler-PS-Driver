#include "VirtScribbler.h"

extern 	int	gDebugging;
const	int	thisModule  = 0x0008;



char	info[]		= "Robot-Version:1.0,Robot:VirtScribbler,Mode:Socket";


/****************************************************************/
/* sendAllSensors						*/
/* Report the state of all the Scribbler sensors to the host	*/
/****************************************************************/
void	VirtScribbler::sendAllSensors()
{
	mPort->takeChar(mLeftIR);
	mPort->takeChar(mRightIR);

	mPort->takeChar(mLeftLight >> 8);
	mPort->takeChar(mLeftLight & 0xFF);
	mPort->takeChar(mCenterLight >> 8);
	mPort->takeChar(mCenterLight & 0xFF);
	mPort->takeChar(mRightLight >> 8);
	mPort->takeChar(mRightLight & 0xFF);

	mPort->takeChar(mLineLeft);
	mPort->takeChar(mLineRight);
	mPort->takeChar(mStall);
}
void	VirtScribbler::getData(unsigned char data[8])
{
	for (int i = 0;i < 8;i++)
		data[i] = getCharBlocking();
}
void	VirtScribbler::echoCommand(char cmd, unsigned char data[8])
{
	mPort->takeChar(cmd);
	for (int i = 0;i < 8;i++)
		mPort->takeChar(data[i]);
}


/****************************************************************/
/* ScribblerCmd							*/
/* Process a command intended for the underling Scribbler robot	*/
/* These commands are formatted differently from the Fluke's	*/
/****************************************************************/ 
int	VirtScribbler::scribblerCommand(char cmd)
{
	unsigned char	data[9];
	unsigned char	ch;
	int		i;

	/* the SET functions return all the sensors 	*/
	switch (cmd)
	{
	  case SET_ECHO_MODE:
		if (gDebugging & thisModule)
			log("SET_ECHO_MODE\n");
		getData(data);
		echoCommand(cmd, data);
		mEchoMode  = data[0];
		sendAllSensors();
		break;		
	  case SET_MOTORS:
		if (gDebugging & thisModule)
			log("SET_MOTORS\n");
		getData(data);
		/* modify values before echo for compatability	*/
		if (mForwardess)
		{
			log("FLUKE_FORWARD\n");
			ch = data[0];
			data[0] = 200 - data[1];
			data[1] = 200 - ch;
		}
		echoCommand(cmd, data);
		mLeftMotor  = data[0];
		mRightMotor = data[1];
		mTorque = (float)(mRightMotor - mLeftMotor)/200.0;
		mSpeed = (float)(mLeftMotor - 100 + mRightMotor - 100)/10.0;
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "left = %d, right = %d\n", mLeftMotor, mRightMotor);
			log(logchunk);
			sprintf(logchunk, "Torque = %f, Speed = %f\n",
					mTorque, mSpeed);
			log(logchunk);
		}
		sendAllSensors();
		break;
	  case SET_MOTORS_OFF:
		if (gDebugging & thisModule)
			log("SET_MOTORS_OFF\n");
		getData(data);
		echoCommand(cmd, data);
		mLeftMotor = mRightMotor = 100;
		mTorque = 0;
		mSpeed = 0;
		sendAllSensors();
		break;
	  case SET_SPEAKER:
	  case SET_SPEAKER_2:
		/* should probably do something different for SET_SPEAKER_2		*/
		if (gDebugging & thisModule)
			log("SET_SPEAKER\n");
		getData(data);
		echoCommand(cmd, data);
		if (mBeep && !mSilent)
			Mix_PlayMusic(mBeep, 1);
		sendAllSensors();
		break;
	  case SET_LED_ALL:
		if (gDebugging & thisModule)
			log("SET_LED_ADD\n");
		getData(data);
		echoCommand(cmd, data);
		mLeftLED   = data[0];
		mCenterLED = data[1];
		mRightLED  = data[2];
		sendAllSensors();
		break;
	  case SET_DATA:
		if (gDebugging & thisModule)
			log("SET_DATA\n");
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			IPRE_data[i] = data[i];
		sendAllSensors();
		break;
	  case GET_ALL:
		if (gDebugging & thisModule)
			log("GET_ALL\n");
		getData(data);
		echoCommand(cmd, data);
		sendAllSensors();
		break;
	  case GET_DATA:
		if (gDebugging & thisModule)
			log("GET_DATA\n");
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			mPort->takeChar(IPRE_data[i]);
		break;
	  case SET_LED_LEFT_ON:
		getData(data);
		echoCommand(cmd, data);
		mLeftLED = 1;
		sendAllSensors();
		break;
	  case SET_LED_LEFT_OFF:
		getData(data);
		echoCommand(cmd, data);
		mLeftLED = 0;
		sendAllSensors();
		break;
	  case SET_LED_CENTER_ON:
		getData(data);
		echoCommand(cmd, data);
		mCenterLED = 1;
		sendAllSensors();
		break;
	  case SET_LED_CENTER_OFF:
		getData(data);
		echoCommand(cmd, data);
		mCenterLED = 0;
		sendAllSensors();
		break;
	  case SET_LED_RIGHT_ON:
		getData(data);
		echoCommand(cmd, data);
		mRightLED = 1;
		sendAllSensors();
		break;
	  case SET_LED_RIGHT_OFF:
		getData(data);
		echoCommand(cmd, data);
		mLeftLED = 0;
		sendAllSensors();
		break;
	  case SET_LED_ALL_ON:
		getData(data);
		echoCommand(cmd, data);
		mLeftLED = mCenterLED = mRightLED = 1;
		sendAllSensors();
		break;
	  case SET_LED_ALL_OFF:
		getData(data);
		echoCommand(cmd, data);
		mLeftLED = mCenterLED = mRightLED = 0;
		sendAllSensors();
		break;
	  case GET_PASS1:
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			mPort->takeChar(mPass[i]);
		break;
	  case GET_PASS2:
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			mPort->takeChar(mPass[i + 8]);
		break;
	  case SET_PASS1:
		getData(data);
		echoCommand(cmd, data);
		memcpy(mPass, data, 8);
		sendAllSensors();
		break;
	  case SET_PASS2:
		getData(data);
		echoCommand(cmd, data);
		memcpy(mPass + 8, data, 8);
		sendAllSensors();
		break;
	  case GET_NAME1:
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			mPort->takeChar(mName[i]);
		break;
	  case GET_NAME2:
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;i < 8;i++)
			mPort->takeChar(mName[i + 8]);
		break;
	  case SET_NAME1:
		getData(data);
		echoCommand(cmd, data);
		memcpy(mName, data, 8);
		sendAllSensors();
		break;
	  case SET_NAME2:
		getData(data);
		echoCommand(cmd, data);
		memcpy(mName + 8, data, 8);
		sendAllSensors();
		break;
	  case GET_LIGHT_LEFT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLeftLight >> 8);
		mPort->takeChar(mLeftLight & 0xFF);
		break;
	  case GET_LIGHT_CENTER:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mCenterLight >> 8);
		mPort->takeChar(mCenterLight & 0xFF);
		break;
	  case GET_LIGHT_RIGHT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mRightLight >> 8);
		mPort->takeChar(mRightLight & 0xFF);
		break;
	  case GET_LIGHT_ALL:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLeftLight >> 8);
		mPort->takeChar(mLeftLight & 0xFF);
		mPort->takeChar(mCenterLight >> 8);
		mPort->takeChar(mCenterLight & 0xFF);
		mPort->takeChar(mRightLight >> 8);
		mPort->takeChar(mRightLight & 0xFF);
		break;
	  case GET_IR_LEFT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLeftIR);
		break;
	  case GET_IR_RIGHT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mRightIR);
		break;
	  case GET_LINE_LEFT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLineLeft);
		break;
	  case GET_LINE_RIGHT:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLineRight);
		break;
	  case GET_LINE_ALL:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mLineLeft);
		mPort->takeChar(mLineRight);
		break;
	  case GET_STALL:
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(mStall);
		break;
	  case SET_LOUD:
		getData(data);
		echoCommand(cmd, data);
		mSilent = 0;
		sendAllSensors();
		break;
	  case SET_QUIET:
		getData(data);
		echoCommand(cmd, data);
		mSilent = 1;
		sendAllSensors();
		break;
	  case SET_SINGLE_DATA:
		getData(data);
		echoCommand(cmd, data);
		i = data[0];
		if (i < 8)
			IPRE_data[i] = data[1];
		sendAllSensors();
		break;
	  case GET_ALL_BINARY:
		getData(data);
		echoCommand(cmd, data);
		ch = 0;
		if (mLeftIR)
			ch |= 0x10;
		if (mRightIR)
			ch |= 0x08;
		if (mStall)
			ch |= 0x04;
		if (mLineLeft)
			ch |= 0x02;
		if (mLineRight)
			ch |= 0x01;
		mPort->takeChar(ch);
		break;
	  case GET_INFO:
		getData(data);
		echoCommand(cmd, data);
		for (i = 0;info[i];i++) 
			mPort->takeChar(info[i]);
		mPort->takeChar(0x0A);
		break;
	  case GET_STATE:
		/* I don't know what this does		*/
		/* but I do know it returns two bytes	*/
		getData(data);
		echoCommand(cmd, data);
		mPort->takeChar(0x01);
		mPort->takeChar(0x02);
		break;
	
	/* unimplemented Scribbler Commands		*/
	  case SOFT_RESET:
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "Unknown Scribbler Cmd %d\n", cmd);
			log(logchunk);
		}
		break;
	  default:
		/* not a Scribbler Command		*/
		return 0;

	}
	if (mEchoMode)
		mPort->takeChar(cmd);
	mPort->flushOutput();
	
	return 1;
}
