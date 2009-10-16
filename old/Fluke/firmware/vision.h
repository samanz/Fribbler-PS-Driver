/******************************************************************************
 *
 * Vision routines
 * 
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#ifndef __VISION__
#define __VISION__

#include "ov7649.h"

// the image buffer; also used in other RAM intensive operations
// like upgrading the firwmare
extern unsigned char img[HEIGHT_SIZE*WIDTH_SIZE] __attribute__((aligned(4)));

// delay value used for camera
extern int delay;
extern uint8_t thresh;

/* The windowing parameters
 *  - used to grab a rectangle of the image 
 *  - used to skip pixels for instance for gray scale images
 */
#define NUM_WINDOWS 4
extern unsigned char lower_x[NUM_WINDOWS];
extern unsigned char lower_y[NUM_WINDOWS];
extern unsigned char upper_x[NUM_WINDOWS];
extern unsigned char upper_y[NUM_WINDOWS];
extern unsigned char xstep[NUM_WINDOWS];
extern unsigned char ystep[NUM_WINDOWS];

// The color segementation parameters - by default track orange things
extern unsigned char Y_LOW;
extern unsigned char Y_HIGH;

extern unsigned char U_LOW;
extern unsigned char U_HIGH;

extern unsigned char V_LOW;
extern unsigned char V_HIGH;

// RLE buffer is used when sending an RLE blob image
#define RLE_BUFFER_SIZE 2048
extern unsigned char rle[RLE_BUFFER_SIZE+4] __attribute__((aligned(4)));

extern int rle_counter;


/*
 * Take and image, segment it based on YUV bounds,
 * run-length encode it
 */
void get_rle();

/*
 * Take and image, segment it based on YUV bounds,
 * and find the average X and Y locations of the
 * "on" pixels. Also return the # of on pixels.
 */
void get_blob(int *pix_counter, int *x_loc, int *y_loc);

/*
 * Take and image, segment the desired window based on YUV
 * bounds, and find the average X and Y locations of the
 * "on" pixels. Also return the # of on pixels.
 */
void get_blob_window();


#endif /* __VISION__ */

