#ifndef __SERVER_HELPER__
#define __SERVER_HELPER__

#include "fluke.h"
#include "uart.h"
#include "vision.h"
#include "infrared.h"

void update_low_battery_light();


/*
 * Configure the windowing - the height bounds are reversed
 * x ranges from 0-255
 * y ranges from 0-191
 * step sizes can be used to subsample image or to grab only Y,
 * U, or V pixels
 */	  
void serve_set_window();

/*
 * Configure the segmentation routines. 
 *
 * Don't mess with delay. This is a vestigial organ
 *
 * Thresh is a filter that will tell the RLE routine to
 * ignore runs of size less than thresh.
 */	  
void serve_set_rle();


/*
 * Grab an image and transmit it in raw format
 * 256 x 192 - VYUYVYUY ...
 */	  
void serve_image();

/*
 * Grab an image and a window of it
 */
void serve_image_window();

/*
 * Grab an image, select a window of it, and return
 * the sum of all the pixels. For instance to find
 * the average intensity of the left half of the
 * image
 */
void serve_image_window_sum();

/*
 * Send the RLE image over bluetooth
 */
void serve_rle();

/*
 * Sesnd the sum of the pixels in a window
 */
void serve_image_window_sum();

/*
 * Take and image, segment it based on YUV bounds,
 * and find the average X and Y locations of the
 * "on" pixels. Also return the # of on pixels.
 */
void serve_blob();

/*
 * Take and image, segment the desired window based on YUV
 * bounds, and find the average X and Y locations of the
 * "on" pixels. Also return the # of on pixels.
 */
void serve_blob_window();

/*
 * Serve the gray scale JPEG header information
 */
void serve_jpeg_gray_header();

/*
 * Serve the color JPEG header information
 */
void serve_jpeg_color_header();

/*
 * Take and image, jpeg encode, and send
 */
void serve_jpeg_gray_scan();

/*
 * Take a color image, jpeg encode, and send
 */
void serve_jpeg_color_scan();


/*
 * Return the battery voltage a 2 byte value
 * to convert to volts: value / 20.9813
 */
void serve_battery();

/*
 * Set backward (configurable) led
 */
void serve_back_led();

/*
 * Set IR transmit power
 */
void serve_ir_power();

/*
 * Send over bluetooth the last IR message received
 */
void serve_get_ir_message();

/*
 * Send an IR message 
 */
void serve_send_ir_message();

/*
 * Set IR emitters used for transmission
 */
void serve_set_ir_emitters();

/*
 * Read a serial memory location
 */
void serve_get_serial_mem();

/*
 * Write a serial memory location (doesn't validate address)
 */
void serve_set_serial_mem();

/*
 * Erase a page of the serial memory
 */
void serve_erase_serial_mem();

/*
 * Process an IR message - uses one or the 3 emitters and the receiver
 * to detect an obstacle.
 * 
 * Pulses IR periodically and keeps a counter of reflected IR.  If it
 * receives IR when it itself isn't sending it decrements the counter
 * to trya nd remove false positives.
 *
 * This can definitely be improved.
 */
void serve_ir(uint8_t emitters);

/*
 * Begin filling the scribbler program buffer. (Also the image buffer)
 */
void serve_set_scrib_program();

/*
 * Read the scribbler program buffer. (Also the image buffer)
 */
void serve_get_scrib_program();

/*
 * Begin programming the scribbler. Assumes you have filled the buffer
 * with a valid program or are using the one stored in this firmware
 * (indicated by and program size of 0).
 *
 * A magic code is expected to enter program mode. Used to reduce
 * unintended reprogramming.
 */
void serve_start_scrib_program();

/*
 * reset the scribbler
 */
void serve_reset_scribbler();

/*
 * Used to reconfigure UART0
 *  - Intended for use with other robots 
 */
void serve_set_uart0();

/*
 * Pass a byte to the serial port (UART0)
 */
void serve_send_byte();

/*
 * Enter a passthrough mode where no commands will interpretted by the
 * fluke. Need a magic code of 0x01 to enter after the command code
 */
void serve_set_passthrough();


/*
 * Set the orientation of the scribbler
 * 0: Scribbler forward
 * 1: Fluke forward
 */
void serve_set_forwardness();

/*
 * Turn on white balance
 */
void serve_set_wb();

/*
 * Turn off white balance
 */
void serve_unset_wb();

/*
 * Read a camera configuration register
 */
void serve_read_camera();

/*
 * Write a camera configuration register
 */
void serve_write_camera();

      
/*
 * Dump the serial memory in preparation for a firmware upgrade
 */
void serve_save_eeprom();

/*
 * Reload the serial memory (after a firmware upgrade)
 */
void serve_restore_eeprom();

/*
 * Initiate Firmware Upgrade - uses a magic code to prevent unintended
 * programming
 */
void serve_update_firmware();

/*
 * Reset Fluke using watchdog
 */
void serve_fluke_reset();

#endif /* __SERVER_HELPER__ */
