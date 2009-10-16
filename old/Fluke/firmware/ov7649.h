 /******************************************************************************
 *
 * OV7649 Camera Header File
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/


#ifndef  __OV7649__
#define  __OV7649__

#include "inttypes.h"

#define OV_GAIN      0x00
#define OV_BLUE      0x01
#define OV_RED       0x02
#define OV_SAT       0x03
#define OV_HUE       0x04
#define OV_CWF       0x05
#define OV_BRT       0x06
#define OV_PID       0x0A
#define OV_VER       0x0B
#define OV_AECH      0x10
#define OV_CLKRC     0x11
#define OV_COMA      0x12
#define OV_COMB      0x13
#define OV_COMC      0x14
#define OV_COMD      0x15
#define OV_HSTART    0x17
#define OV_HSTOP     0x18
#define OV_VSTART    0x19
#define OV_VSTOP     0x1A
#define OV_PSHFT     0x1B
#define OV_MIDH      0x1C
#define OV_MIDL      0x1D
#define OV_FACT      0x1F
#define OV_COME      0x20
#define OV_COMF      0x26
#define OV_COMG      0x27
#define OV_COMH      0x28
#define OV_COMI      0x29
#define OV_FRARH     0x2A
#define OV_FRARL     0x2B
#define OV_COMJ      0x2D

#define OV_SPCB      0x60
#define OV_RMCO      0x6C
#define OV_GMCO      0x6D
#define OV_BMCO      0x6E

#define OV_COMK      0x70
#define OV_COML      0x71
#define OV_HSDYR     0x72
#define OV_HSDYF     0x73
#define OV_COMM      0x74
#define OV_COMN      0x75
#define OV_COM0      0x76
#define OV_AVGYG     0x7E
#define OV_AVGRV     0x7F
#define OV_AVGBU     0x80

//#define QVGA
#define VGA

#ifdef VGA
#define TRUE_WIDTH  640
#define TRUE_HEIGHT 480 
#else
#ifdef QVGA
#define TRUE_WIDTH  320
#define TRUE_HEIGHT 240
#endif /* QVGA */
#endif /* VGA */

#define WIDTH_SIZE  256
#define HEIGHT_SIZE 192

/*
 *  Reads one of the registers of the camera and puts result in data
 */
uint8_t read_camera(uint8_t address, uint8_t *data);


/*
 *  Write one of the registers of the camera
 */

uint8_t write_camera(uint8_t address, uint8_t data);

/*
 * Grab a frame from the camera, and fill imgbuf format is VYUYVYUY 
 * Image is 256 wide and 192 high
 * 
 * This is stored in RAM not flash
 */
__attribute__ ((section (".data"), long_call))  
void grab_frame(unsigned char *imgbuf, int delay);


#endif /* __OV7649__ */
