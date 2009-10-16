#include "fluke.h"
#include "ov7649.h"
#include "uart.h"
#include "vision.h"

// delay value used for camera
int delay = 21590;
uint8_t thresh = 1;

/* The windowing parameters
 *  - used to grab a rectangle of the image 
 *  - used to skip pixels for instance for gray scale images
 */
#define NUM_WINDOWS 4
unsigned char lower_x[NUM_WINDOWS];
unsigned char lower_y[NUM_WINDOWS];
unsigned char upper_x[NUM_WINDOWS];
unsigned char upper_y[NUM_WINDOWS];
unsigned char xstep[NUM_WINDOWS];
unsigned char ystep[NUM_WINDOWS];

// The color segementation parameters - by default track orange things
unsigned char Y_LOW = 0;
unsigned char Y_HIGH = 254;

unsigned char U_LOW = 126;
unsigned char U_HIGH = 145;

unsigned char V_LOW = 90;
unsigned char V_HIGH =120;


unsigned char rle[RLE_BUFFER_SIZE+4] __attribute__((aligned(4)));

int rle_counter = 0;

/*
 * Take and image, segment it based on YUV bounds,
 * run-length encode it, and send it over the wire
 */

void get_rle()
{
  // temp variables used for color segmentation and RLE
  int counter = 0;
  int inside = 0;
  int in_counter = 0;
  int out_counter = 0;
  int i;
  
  rle_counter = 0;
  inside = 0;
  counter = 0;
  
  grab_image_vote(IMG_VOTES, img, delay);
  
  for (i = 0; i < WIDTH_SIZE*HEIGHT_SIZE; i += 4)		
    {
      if (rle_counter >= RLE_BUFFER_SIZE) break;
      
      if ((img[i] >= V_LOW)   &&
	  (img[i] <= V_HIGH) &&
	  
	  (img[i+1] >= Y_LOW)   &&
	  (img[i+1] <= Y_HIGH) &&
	  (img[i+3] >= Y_LOW)   &&
	  (img[i+3] <= Y_HIGH) &&
	  
	  (img[i+2] >= U_LOW)   &&
	  (img[i+2] <= U_HIGH))
	{
	  // we are inside the box
	  in_counter++;
	  out_counter = 0;
	  
	  //but are we inside enough
	  if (!inside && in_counter >= thresh)
	    {
	      rle[rle_counter++] = counter >> 8;
	      rle[rle_counter++] = counter;  
	      inside = 1;
	      counter = 0; 
	    }
	}
      else
	{
	  // we are outside the box
	  in_counter = 0;
	  out_counter ++;
	  // but are we outside enough
	  if (inside && out_counter >= thresh)
	    {
	      rle[rle_counter++] = counter >> 8;
	      rle[rle_counter++] = counter; 
	      inside = 0;
	      counter = 0;			      
	    }
	}		  
      counter++;
    }
  
  // store as 2 byte values - first high byte then low byte
  rle[rle_counter++] = counter >> 8;
  rle[rle_counter++] = counter;			      
}
void get_blob(int *pix_counter, int *x_loc, int *y_loc)
{
  int i;

  (*pix_counter) = 0;
  (*x_loc) = 0;
  (*y_loc) = 0;
  
  grab_image_vote(IMG_VOTES, img, delay);	      
  
  for (i = 0; i < WIDTH_SIZE*HEIGHT_SIZE; i += 4)		
    {
      if ((img[i] >= V_LOW)   &&
	  (img[i] <= V_HIGH) &&
	  (img[i+1] >= Y_LOW)   &&
	  (img[i+1] <= Y_HIGH) &&
	  (img[i+3] >= Y_LOW)   &&
	  (img[i+3] <= Y_HIGH) &&
	  (img[i+2] >= U_LOW)   &&
	  (img[i+2] <= U_HIGH))
	{
	  (*pix_counter) ++;
	  (*y_loc) += (i / WIDTH_SIZE);
	  (*x_loc) += (i % WIDTH_SIZE);
	}
    }
  
  if (*pix_counter > 0)
    {
      (*x_loc) /= (*pix_counter);
      (*y_loc) /= (*pix_counter);
    }  
}


void get_blob_window(int *pix_counter, int *x_loc, int *y_loc)
{
  int window;
  int i,j;
  
  (*pix_counter) = 0;
  (*x_loc) = 0;
  (*y_loc) = 0;
  
  // add up the Y pixels of the window	      
  window = getchblock(); //which window to grab
  if (window < 0 || window >= NUM_WINDOWS) window = 0;
  
  grab_image_vote(IMG_VOTES, img, delay);	      	      
  
  for (i = 0; i < WIDTH_SIZE*HEIGHT_SIZE; i += 4)		
    {
      j = i % WIDTH_SIZE;
      if (i >= lower_y[window] && 
	  i <= upper_y[window] && 
	  j >= lower_x[window] && 
	  j <= upper_x[window])
	{		      
	  if ((img[i] >= V_LOW)   &&
			  (img[i] <= V_HIGH) &&
	      (img[i+1] >= Y_LOW)   &&
	      (img[i+1] <= Y_HIGH) &&
	      (img[i+3] >= Y_LOW)   &&
	      (img[i+3] <= Y_HIGH) &&
	      (img[i+2] >= U_LOW)   &&
	      (img[i+2] <= U_HIGH))
	    {
	      (*pix_counter) ++;
	      (*y_loc) += (i / WIDTH_SIZE);
	      (*x_loc) += (i % WIDTH_SIZE);
	    }
	}
    }
  
  if (*pix_counter > 0)
    {
      (*x_loc) /= (*pix_counter);
      (*y_loc) /= (*pix_counter);
    }
}


