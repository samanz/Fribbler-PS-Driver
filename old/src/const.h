#ifndef SCR_CONST
#define SCR_CONST
const int NONE = -1;




// scribbler constants

const int SOFT_RESET = 33;

const int UPDATE_FIRMWARE    	=40;
const int SAVE_EEPROM		=41;
const int RESTORE_EEPROM	=42;
const int WATCHDOG_RESET	=43;



const int GET_PASS1		=50;
const int GET_PASS2		=51;

const int SET_PASS1		=55;
const int SET_PASS2		=56;
const int GET_ALL		=65;
const int GET_ALL_BINARY	=66;
const int GET_LIGHT_LEFT	=67;
const int GET_LIGHT_CENTER	=68;
const int GET_LIGHT_RIGHT	=69;

const int GET_LIGHT_ALL		=70;
const int GET_IR_LEFT		=71;
const int GET_IR_RIGHT		=72;
const int GET_IR_ALL		=73;
const int GET_LINE_LEFT		=74;
const int GET_LINE_RIGHT	=75;
const int GET_LINE_ALL		=76;  
const int GET_STATE		=77;  
const int GET_NAME1		=78;
const int GET_NAME2		=64;
const int GET_STALL		=79; 

const int GET_INFO		=80;  
const int GET_DATA		=81;  
const int GET_RLE		=82; // a segmented and run-length encoded image
const int GET_IMAGE		=83; //  the entire 256 x 192 image in YUYV format
const int GET_WINDOW		=84; // the windowed image (followed by which window)
const int GET_DONGLE_L_IR	=85; // number of returned pulses when left emitter is turned on
const int GET_DONGLE_C_IR	=86; // number of returned pulses when center emitter is turned on
const int GET_DONGLE_R_IR	=87; // number of returned pulses when right emitter is turned on
const int GET_WINDOW_LIGHT	=88; // average intensity in the user defined region
const int GET_BATTERY		=89; // battery voltage

const int GET_SERIAL_MEM	=90; // with the address returns the value in serial memory
const int GET_SCRIB_PROGRAM	=91; // with offset, returns the scribbler program buffer
const int GET_CAM_PARAM		=92; // with address, returns the camera parameter at that address
const int GET_BLOB_WINDOW 	=94;
const int GET_BLOB		=95;
const int SET_SINGLE_DATA	=96;
const int SET_DATA		=97;
const int SET_ECHO_MODE		=98;
const int SET_LED_LEFT_ON	=99;

const int SET_LED_LEFT_OFF	=100;
const int SET_LED_CENTER_ON	=101;
const int SET_LED_CENTER_OFF	=102;
const int SET_LED_RIGHT_ON	=103;
const int SET_LED_RIGHT_OFF	=104;
const int SET_LED_ALL_ON	=105;
const int SET_LED_ALL_OFF	=106;
const int SET_LED_ALL		=107;
const int SET_MOTORS_OFF	=108;
const int SET_MOTORS		=109; 

const int SET_NAME1		=110; 
const int SET_NAME2		=119; // set name2 byte
const int SET_LOUD		=111;
const int SET_QUIET		=112;
const int SET_SPEAKER		=113;
const int SET_SPEAKER_2		=114;
const int SET_DONGLE_LED_ON	=116; // turn binary dongle led on
const int SET_DONGLE_LED_OFF	=117; // turn binary dongle led off
const int SET_RLE		=118; // set rle parameters 

const int SET_DONGLE_IR		=120; // set dongle IR power
const int SET_SERIAL_MEM	=121; // set serial memory byte
const int SET_SCRIB_PROGRAM	=122; // set scribbler program memory byte
const int SET_START_PROGRAM	=123; // initiate scribbler programming process
const int SET_RESET_SCRIBBLER	=124; // hard reset scribbler
const int SET_SERIAL_ERASE	=125; // erase serial memory
const int SET_DIMMER_LED	=126; // set dimmer led
const int SET_WINDOW		=127; // set user defined window
const int SET_FORWARDNESS	=128; // set direction of scribbler
const int SET_WHITE_BALANCE	=129; // turn on white balance on camera 

const int SET_NO_WHITE_BALANCE	=130; // diable white balance on camera (default)
const int SET_CAM_PARAM		=131; // with address and value, sets the camera parameter at that address
const int SET_UART0 		=132;
const int SET_PASS_BYTE 	=133;
const int SET_PASSTHROUGH 	=134;
const int GET_JPEG_GRAY_HEADER	=135;
const int GET_JPEG_GRAY_SCAN	=136;
const int GET_JPEG_COLOR_HEADER	=137;
const int GET_JPEG_COLOR_SCAN	=138;
const int SET_PASS_N_BYTES	=139;

const int GET_PASS_N_BYTES	=140;
const int GET_PASS_BYTES_UNTIL	=141;
const int GET_VERSION		=142;
const int SET_PASSTHROUGH_ON	=143;
const int SET_PASSTHROUGH_OFF	=144;

const int GET_IR_MESSAGE 	=150;
const int SEND_IR_MESSAGE 	=151;
const int SET_IR_EMITTERS 	=152;



const int PACKET_LENGTH=9;


#endif
