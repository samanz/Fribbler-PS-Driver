#include "VirtScribbler.h"
#include <cmath>

extern 	int	gDebugging;
const	int	thisModule  = 0x0008;



char	version[]  	= "VirtScribbler 0.1";



/****************************************************************/
/* flukeCommand							*/
/* Process a command intended for the Fluke Dongle 		*/
/* These commands are format free				*/
/****************************************************************/ 
int	VirtScribbler::flukeCommand(char ch)
{
	int	i;
	int	win;
	unsigned char	xLo, xHi, yLo, yHi;
	int	width, hight, len;
	
	switch (ch)
	{
	  case SET_FORWARDNESS:
		mForwardess = getCharBlocking();
		break;
  	  case GET_VERSION:
		for (i = 0;version[i];i++)
			mPort->takeChar(version[i]);
		mPort->takeChar(0x0A);
		break;
	  case SET_NO_WHITE_BALANCE:
		break;
	  case SET_WHITE_BALANCE:
		break;
 	  case GET_CAM_PARAM:
		getCharBlocking();
		mPort->takeChar(0xFF);	// just send a dummy response
		break;
	  case SET_CAM_PARAM:
		getCharBlocking();	// ignore id
		getCharBlocking();	// ignore param
		break;
	  case GET_JPEG_COLOR_HEADER:
		log("GET_JPEG_COLOR_HEADER\n");
		if (mJpegColorHeader)
		{
			log("about to send\n");
			sprintf(logchunk, " %ld bytes\n", mJpegColorHeader->getSize());
			log(logchunk); 
			mJpegColorHeader->write(*mPort, LITTLENDIAN);
		}
		else
			log("no color header\n");
		break;
	  case GET_JPEG_COLOR_SCAN:
		log("GET_JPEG_COLOR_SCAN\n");
		getCharBlocking();	// ignore reliable
		if (mJpegColorScan)
			mJpegColorScan->write(*mPort, 0);
		else
			log("no color scan\n");
		break;
	  case GET_JPEG_GRAY_HEADER:
		log("GET_JPEG_GRAY_HEADER\n");
		if (mJpegGrayHeader)
		{
			log("about to send\n");
			sprintf(logchunk, " %ld bytes\n", mJpegGrayHeader->getSize());
			log(logchunk); 
			mJpegGrayHeader->write(*mPort, LITTLENDIAN);
		}
		else
			log("no gray header\n");
		break;
	  case GET_JPEG_GRAY_SCAN:
		log("GET_JPEG_GRAY_SCAN\n");
		getCharBlocking();	// ignore reliable
		if (mJpegGrayScan)
		{
			log("about to send\n");
			sprintf(logchunk, " %ld bytes\n", mJpegGrayScan->getSize());
			log(logchunk); 
			mJpegGrayScan->write(*mPort, 0);
		}
		else
			log("no gray scan\n");
		break;
	  case SET_RLE:
		log("SET_RLE\n");
		for (i = 0;i < 8;i++)
			getCharBlocking();
		break;
	  case GET_RLE:
		log("GET_RLE\n");
		if (mBitmap)
			mBitmap->write(*mPort, BIGENDIAN);
		else
			log("no Bitmap scan\n");
		break;
	  case GET_IMAGE:
		log("GET_IMAGE\n");
		if (mImage)
			mImage->write(*mPort, 0);
		else
			log("no image\n");
		break;
	  case GET_WINDOW:
		log("GET_WINDOW\n");
		win = getCharBlocking();
	
		if (win == 0)
		{
			log("sending canned window\n");
			if (mImage)
				mImage->write(*mPort, 0);
			else
				log("no image\n");
		}
		else
		{
			mWin[win].mW.getX(xLo, xHi);
			mWin[win].mW.getY(yLo, yHi);
			hight = yHi - yLo;
			width = xHi - xLo;
			len = (hight/mWin[win].mStepY + 1)*(width/mWin[win].mStepX + 1);
			for (i = 0;i < len;i++)
				mPort->takeChar(i & 0xFF);
		}
		break;
	  case GET_BATTERY:
		mPort->takeChar(0);
		mPort->takeChar(167);	/* say 8 volts	*/
		break;
	  case GET_DONGLE_L_IR:
		log("GET_DONGLE_L_IR\n");
		len = getDistance(mOrientation + 7 * M_PI/6);
		mFlukeLeftIR = 2000/len;
		mPort->takeChar(mFlukeLeftIR >> 8);
		mPort->takeChar(mFlukeLeftIR & 0xFF);	
		break;
	  case GET_DONGLE_C_IR:
		log("GET_DONGLE_C_IR\n");
		len = getDistance(mOrientation + M_PI);
		mFlukeCenterIR = 2000/len;
		mPort->takeChar(mFlukeCenterIR >> 8);
		mPort->takeChar(mFlukeCenterIR & 0xFF);	
		break;
	  case GET_DONGLE_R_IR:
		log("GET_DONGLE_R_IR\n");
		len = getDistance(mOrientation + 5 * M_PI/6);
		mFlukeRightIR = 2000/len;
		mPort->takeChar(mFlukeRightIR >> 8);
		mPort->takeChar(mFlukeRightIR & 0xFF);	
		break;	
	  case SET_DIMMER_LED:
		log("SET_DIMMER_LED\n");
		mFlukeBackLED = getCharBlocking();
		break; 
	  case SET_DONGLE_IR:
		log("SET_DONGLE_IR\n");
		mFlukePowerIR = getCharBlocking();
		break; 	
	  case SET_DONGLE_LED_ON:
		log("SET_DONGLE_LED_ON\n");
		mFlukeLED = 1;
		break;
	  case SET_DONGLE_LED_OFF:
		log("SET_DONGLE_LED_OFF\n");	
		mFlukeLED = 0;
		break;
	  case SET_IR_EMITTERS:
		log("SET_IR_EMITTERS\n");
		mFlukeUseEmitters = getCharBlocking();
		break;
      	  case SET_WINDOW:
		log("SET_WINDOW\n");
		win = getCharBlocking();
		if (win >= 4)
			win = 0;
		xLo = getCharBlocking();
		yHi =  191 -  getCharBlocking();
		xHi = getCharBlocking(); 
		yLo =  191 -  getCharBlocking();
		mWin[win].mW.setX(xLo, xHi);
		mWin[win].mW.setY(yLo, yHi);
		mWin[win].mStepX = getCharBlocking(); 
		mWin[win].mStepY = getCharBlocking(); 
		break;
         case GET_WINDOW_LIGHT:
		log("GET_WINDOW_LIGHT\n");
		win = getCharBlocking();
		if (win >= 4)
			win = 0;
		mPort->takeChar(0x01);
		mPort->takeChar(0x02);
		mPort->takeChar(0x03);
		break;
	  case GET_BLOB:
		log("GET_BLOB\n");
		/* count */
		mPort->takeChar(0x01);
		mPort->takeChar(0x02);
		/* (x, y) of center	*/
		mPort->takeChar(0x10);
		mPort->takeChar(0x20);
		break;
	  case GET_BLOB_WINDOW:
		log("GET_BLOB_WINDOW\n");
		win = getCharBlocking();
		if (win >= 4)
			win = 0;
		/* count */
		mPort->takeChar(0x01);
		mPort->takeChar(0x02);
		/* (x, y) of center	*/
		mPort->takeChar(0x10);
		mPort->takeChar(0x20);
		break;
	  case SET_RESET_SCRIBBLER:
		log("SET_RESET_SCRIBBLER\n");
		/* do a reboot		*/
		break;
	  case GET_SERIAL_MEM:
		log("GET_SERIAL_MEM\n");
		/* page 	*/
		getCharBlocking();
		getCharBlocking();
		/* offset 	*/
		getCharBlocking();
		getCharBlocking();
		/* Byte		*/
		mPort->takeChar(0x20);
		break;
	  case SET_SERIAL_MEM:
		log("SET_SERIAL_MEM\n");
		/* page 	*/
		getCharBlocking();
		getCharBlocking();
		/* offset 	*/
		getCharBlocking();
		getCharBlocking();
		/* Byte		*/
		getCharBlocking();
		break;
	  case SET_SERIAL_ERASE:
		log("SET_SERIAL_ERASE\n");
		/* page 	*/
		getCharBlocking();
		getCharBlocking();
		break;
	  case SET_SCRIB_PROGRAM:
		/* offset 	*/
		getCharBlocking();
		getCharBlocking();
		/* Byte		*/
		getCharBlocking();
		break;
	  case GET_SCRIB_PROGRAM:
		/* offset 	*/
		getCharBlocking();
		getCharBlocking();
		/* Byte		*/
		mPort->takeChar(0x20);
		break;
	  case SET_START_PROGRAM:
		/* size		*/
		getCharBlocking();
		getCharBlocking();
		break;
	/* Unimplemented Fluke Commands			*/
	/* PASSTHROUGH not being used on Scribbler	*/
	  case SET_PASSTHROUGH:
	  case SET_PASS_N_BYTES:
	  case GET_PASS_N_BYTES:
	  case GET_PASS_BYTES_UNTIL:
	  case SET_PASSTHROUGH_ON:
	  case SET_PASSTHROUGH_OFF:

	  case UPDATE_FIRMWARE:
		/* magic code	*/
		getCharBlocking(); // should be 0x01
		getCharBlocking(); // should be 0x23
		/* update goes here	*/
		break;

	/* IR_MESSAGE not implemented yet		*/
	  case GET_IR_MESSAGE:
	  case SEND_IR_MESSAGE:

	  case SAVE_EEPROM:
	  case RESTORE_EEPROM:
	  case WATCHDOG_RESET:
	  case SET_UART0:
	  case SET_PASS_BYTE:
		if (gDebugging & thisModule)
		{
			sprintf(logchunk, "Unimplemented Fluke cmd %d\n", ch);
			log(logchunk);
		}
		break;
	  default:
		return 0;
	}
	mPort->flushOutput();
	return 1;
}
	
