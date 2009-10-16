/*******************************************************************************
 *
 * Fluke/Scribbler Byte Codes
 * 
 * See main.c for better descriptions of each
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#define SOFT_RESET            33

#define UPDATE_FIRMWARE	      40
#define SAVE_EEPROM	      41
#define RESTORE_EEPROM	      42
#define WATCHDOG_RESET	      43


#define GET_PASS1             50
#define GET_PASS2             51
#define SET_PASS1             55
#define SET_PASS2             56

#define GET_NAME2             64
#define GET_ALL               65  
#define GET_ALL_BINARY        66  
#define GET_LIGHT_LEFT        67  
#define GET_LIGHT_CENTER      68  
#define GET_LIGHT_RIGHT       69  
#define GET_LIGHT_ALL         70  
#define GET_IR_LEFT           71  
#define GET_IR_RIGHT          72  
#define GET_IR_ALL            73  
#define GET_LINE_LEFT         74  
#define GET_LINE_RIGHT        75  
#define GET_LINE_ALL          76  
#define GET_STATE             77  
#define GET_NAME              78  
#define GET_STALL             79  
#define GET_INFO              80  
#define GET_DATA              81  


#define GET_RLE               82  
#define GET_IMAGE             83  
#define GET_WINDOW            84  
#define GET_DONGLE_L_IR       85  
#define GET_DONGLE_C_IR       86  
#define GET_DONGLE_R_IR       87  
#define GET_WINDOW_LIGHT      88  
#define GET_BATTERY           89  
#define GET_SERIAL_MEM        90  
#define GET_SCRIB_PROGRAM     91  
#define GET_CAM_PARAM         92  

#define GET_IMAGE_COMPRESSED  93 
#define GET_BLOB_WINDOW       94
#define GET_BLOB              95


#define SET_SINGLE_DATA       96  
#define SET_DATA              97  
#define SET_ECHO_MODE         98  
#define SET_LED_LEFT_ON       99 
#define SET_LED_LEFT_OFF      100
#define SET_LED_CENTER_ON     101
#define SET_LED_CENTER_OFF    102
#define SET_LED_RIGHT_ON      103
#define SET_LED_RIGHT_OFF     104
#define SET_LED_ALL_ON        105
#define SET_LED_ALL_OFF       106
#define SET_LED_ALL           107 
#define SET_MOTORS_OFF        108 
#define SET_MOTORS            109 
#define SET_NAME              110 
#define SET_LOUD              111
#define SET_QUIET             112
#define SET_SPEAKER           113 
#define SET_SPEAKER_2         114 


#define SET_DONGLE_LED_ON     116
#define SET_DONGLE_LED_OFF    117
#define SET_RLE               118
#define SET_NAME2             119 
#define SET_DONGLE_IR         120
#define SET_SERIAL_MEM        121
#define SET_SCRIB_PROGRAM     122
#define SET_START_PROGRAM     123
#define SET_RESET_SCRIBBLER   124
#define SET_SERIAL_ERASE      125
#define SET_DIMMER_LED        126
#define SET_WINDOW            127
#define SET_FORWARDNESS       128
#define SET_WHITE_BALANCE     129
#define SET_NO_WHITE_BALANCE  130
#define SET_CAM_PARAM         131

#define SET_UART0             132
#define SET_PASS_BYTE         133
#define SET_PASSTHROUGH       134

#define GET_JPEG_GRAY_HEADER  135
#define GET_JPEG_GRAY_SCAN    136
#define GET_JPEG_COLOR_HEADER 137
#define GET_JPEG_COLOR_SCAN   138

#define SET_PASS_N_BYTES      139
#define GET_PASS_N_BYTES      140
#define GET_PASS_BYTES_UNTIL  141

#define GET_VERSION           142
#define SET_PASSTHROUGH_ON    143
#define SET_PASSTHROUGH_OFF   144

#define GET_IR_MESSAGE        150
#define SEND_IR_MESSAGE       151
#define SET_IR_EMITTERS       152
