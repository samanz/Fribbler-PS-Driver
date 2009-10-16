/*******************************************************************************
 *
 * Fluke Firmware 
 * 
 * Code provides a bluetooth serial interface to scribbler and fluke.
 * Chiefly this code mediates the 2 serial ports one connected to the
 * scribbler (UART0) and the other connected to the bluetooth chip
 * (UART1). 
 *
 * The LPC2106 data sheet is necessary for understanding this code.
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/


#include "lpc210x.h"
#include "uart.h"
#include "fluke.h"
#include "interrupts.h"
#include "ov7649.h"
#include "ipre-bytecodes.h"
#include "server-helper.h"
#include "vision.h"
#include "scribbler.h"
#include "ipre-bytecodes.h"
#include "firmware_upgrade.h"
#include "constants.h"
#include "jpeg.h"
#include "vic.h"
#include "infrared.h"

void FIQ_Routine (void)   __attribute__ ((interrupt("FIQ")));
void SWI_Routine (void)   __attribute__ ((interrupt("SWI")));
void UNDEF_Routine (void) __attribute__ ((interrupt("UNDEF")));

// The size of the scribbler packets
#define SCRIBBLER_PACKET_LENGTH 9

// the current scribbler packet being transmitted
static unsigned char cur_rqst[SCRIBBLER_PACKET_LENGTH];
// our current position in the scribbler packaet
static unsigned rqst_idx = 0;

// used to remember the orientation of the scribbler
//  - is the fluke facing forwards or backwards?
//  - this is stored in location (0,0) in the serial memory
unsigned char scribbler_orientation;

int main (void) 
{
  int ch = 0;
  int i = 0;
  
  // are we ready to relay scribbler output
  // by default we wait until we get a scribbler request from
  // the user before relaying scribbler messages via bluetooth
  int relay_scrib_messages = 0;

  // is the current command a scribbler command
  int scribbler_cmd = 0;

  // are we processing a get_info packet
  int  get_info = 0;


  // setup the directionality of the pins
  setup_pins();
  
  // setup the PLL - 60 MHz
  setup_pll();

  // setup the instruction prefetch
  setup_mam();
  
  // give some time to let everything settle
  msleep(100);
  
  // The scribbler UART(0) is 38400 8N1
  uart0Init(B38400, UART_8N1, UART_FIFO_8); // setup UART 0
  
  // The CM-5 UART (0) is 57600 8N1
  //  uart0Init(B57600, UART_8N1, UART_FIFO_8); // setup UART 0
  // The Robonova UART (0) is 9600 or 115200 8N1
  //  uart0Init(B115200, UART_8N1, UART_FIFO_8); // setup UART 0

  
  // The bluetooth UART(1)
#if (VERSION_OF_BOARD == 3)
  uart1Init(B57600, UART_8N1, UART_FIFO_8); // setup UART 1
#else
  uart1Init(B460800, UART_8N1, UART_FIFO_8); // setup UART 1
#endif
  
  // synchronize with autobaud
  for (i = 1; i < 512; i++)
    {
      uart1Putch(i);
    }  

  // Set up VIC and enable interrupts
  vic_init();
  
  //set timer0 prescalaer to around 1 usec
  TIMER0_PR = 0x39;
  
  // Start timer0 for benchmarking
  TIMER0_TCR = 1;

  // Set up the transmit queue
  uart1_queue_init();

  // setup interrupts for receiving IR messages
  ir_rx_init();

  // set back configurable LED off
  set_led(0);

  // set front LED off
  led_off();

  // turn IR power to 135
  set_ir(135);

  // set external bluetooth to max; internal amplifier is throttled via pskeys
  set_bluetooth(255);
  
  // slow camera down 
  write_camera(OV_CLKRC, 1);
  
  // turn off auto white balance
  //write_camera(OV_COMA, 0x14 & ~(1<<2));
  
  // turn off auto gain and auto exposure
  //write_camera(OV_COMB, 0xA3 & ~(0x3));

  //flush out any old characters
  for (i = 0; i < 200; i++)  
    {
      getch();
    }
  

  /* read in scribbler orientation 
   *
   * - scribbler forward is stored as 0xDF in serial memory to avoid
   *   coincidental settings
   */

  read_mem(0, 0, &scribbler_orientation, 1);  
  if (scribbler_orientation == 0xDF)
    {
      scribbler_orientation = 0;
    }
  else
    {
      scribbler_orientation = 1;
    }
  
  while (1)
    {
      //led_on();
      ch = getch();     
      //led_off();
     	  
      // body of the firmware - process the byte code sent over bluetooth
      if (ch != -1)
	{
	  cur_rqst[rqst_idx] = ch;

	  // process the cmd if its intended for the fluke
	  if (rqst_idx == 0)
	    {
	      scribbler_cmd = 0; 	      
	      if      (ch == SET_WINDOW)           serve_set_window();
	      else if (ch == SET_RLE)              serve_set_rle();
	      else if (ch == GET_RLE)	           serve_rle();
	      else if (ch == GET_BLOB)		   serve_blob();
	      else if (ch == GET_BLOB_WINDOW)	   serve_blob_window();
	      else if (ch == GET_IMAGE)		   serve_image();
	      else if (ch == GET_WINDOW)	   serve_image_window();
	      else if (ch == GET_WINDOW_LIGHT)	   serve_image_window_sum();
	      else if (ch == GET_BATTERY)	   serve_battery();
	      else if (ch == SET_DONGLE_LED_ON)	   led_on();
	      else if (ch == SET_DONGLE_LED_OFF)   led_off();
	      else if (ch == SET_DIMMER_LED)	   serve_back_led();
	      else if (ch == SET_DONGLE_IR)	   serve_ir_power();
	      else if (ch == SET_RESET_SCRIBBLER)  serve_reset_scribbler();
	      else if (ch == GET_SERIAL_MEM)	   serve_get_serial_mem();
	      else if (ch == SET_SERIAL_MEM)	   serve_set_serial_mem();
	      else if (ch == SET_SERIAL_ERASE)	   serve_erase_serial_mem();
	      else if (ch == GET_DONGLE_R_IR)	   serve_ir(0x1);
	      else if (ch == GET_DONGLE_L_IR)	   serve_ir(0x2);
	      else if (ch == GET_DONGLE_C_IR)	   serve_ir(0x4);
	      else if (ch == SET_SCRIB_PROGRAM)	   serve_set_scrib_program();
	      else if (ch == GET_SCRIB_PROGRAM)	   serve_get_scrib_program();
	      else if (ch == SET_START_PROGRAM)	   serve_start_scrib_program();
	      else if (ch == SET_UART0)		   serve_set_uart0();
	      else if (ch == SET_PASS_BYTE)	   serve_send_byte();
	      else if (ch == SET_PASSTHROUGH)	   serve_set_passthrough();
	      else if (ch == SET_PASS_N_BYTES)	   serve_set_pass_n_bytes();
	      else if (ch == SET_PASSTHROUGH_ON)   relay_scrib_messages = 1;
	      else if (ch == SET_PASSTHROUGH_OFF)  relay_scrib_messages = 0;
	      else if (ch == GET_PASS_N_BYTES)	   serve_get_pass_n_bytes();
	      else if (ch == GET_PASS_BYTES_UNTIL) serve_get_pass_bytes_until();
	      else if (ch == SET_FORWARDNESS)	   serve_set_forwardness();
	      else if (ch == SET_WHITE_BALANCE)	   serve_set_wb();
	      else if (ch == SET_NO_WHITE_BALANCE) serve_unset_wb();
	      else if (ch == GET_CAM_PARAM)	   serve_read_camera();	      
	      else if (ch == SET_CAM_PARAM)	   serve_write_camera();
	      else if (ch == SAVE_EEPROM)	   serve_save_eeprom();
	      else if (ch == RESTORE_EEPROM)	   serve_restore_eeprom(); 
	      else if (ch == UPDATE_FIRMWARE)	   serve_update_firmware();
	      else if (ch == WATCHDOG_RESET)	   serve_fluke_reset();
	      else if (ch == GET_JPEG_GRAY_HEADER) serve_jpeg_gray_header();
	      else if (ch == GET_JPEG_COLOR_HEADER)serve_jpeg_color_header();
	      else if (ch == GET_JPEG_GRAY_SCAN)   serve_jpeg_gray_scan();
	      else if (ch == GET_JPEG_COLOR_SCAN)  serve_jpeg_color_scan();
	      else if (ch == GET_VERSION)          serve_version();
	      else if (ch == GET_IR_MESSAGE)       serve_get_ir_message();
	      else if (ch == SEND_IR_MESSAGE)      serve_send_ir_message();
	      else if (ch == SET_IR_EMITTERS)      serve_set_ir_emitters();
	      else scribbler_cmd = 1;
	    }
	  
	  if (scribbler_cmd)	    
	    {	      
	      // Flush if this is the first time we have  talked to the scribbler 
	      if (relay_scrib_messages == 0)
		{
		  for (i = 0; i < 100; i++)
		    {
		      uart0Getch();
		    }
		}

	      relay_scrib_messages = 1;
	      
	      // we just got a GET_INFO request; pass it on to the scribbler
	      if ((rqst_idx == 0) && (ch == GET_INFO))
		{
		  get_info = 1;
		}
	      // scribbler should now have the whole GET_INFO request
	      else if ((rqst_idx == 8) && get_info == 1)
		{
		  get_info = 2;
		}
	      
	      // flip motors forward to backwards
	      if ((cur_rqst[0] == SET_MOTORS) && scribbler_orientation && rqst_idx < 3)
		{
		  // now send the packet with left/right swapped
		  if ((rqst_idx == 2))
		    {
		      uart0Putch(cur_rqst[0]);
		      //msleep(10);
		      
		      uart0Putch(200-cur_rqst[2]);
		      //msleep(10);
		      
		      uart0Putch(200-cur_rqst[1]);
		    }
		}
	      // pass on character to the scribbler
	      else
		{
		  uart0Putch(ch);
		}
	     
	      // take care of scribbler packet indexing
	      rqst_idx ++;
	      if (rqst_idx > 8)
		{
		  rqst_idx = 0;
		}
	    }
	} /* end if it received a request */
      
      /*
       * if we are talking to the scribbler see if it has anything
       * for us to relay over the bluetooth link
       */
      if (relay_scrib_messages)
	{
	  ch = uart0Getch();
	  if (ch != -1)
	    {
	      // Process GET_INFO request we got the last character
	      if (get_info > 500 && ch == 0x0A)
		{
		  get_info = 0;
		}

	      // Process GET_INFO we just sent our GET_INFO string
	      if (get_info >= 500 && get_info < 50000)
		{
		  putch(',');
		  get_info = 50000;
		}
	      putch(ch);	      
	    }
	}
      
      
      /* 
       * send our GET_INFO information 
       *
       * the GET_INFO handling is pretty hideous to preserve backward
       * compatibility with other bluetooth serial and serial links.
       */
      
      if (get_info > 1)
	{
	  get_info ++;
	  
	  if (get_info == 400)
	    {
	      putstr(VERSION);
	      get_info = 500;
	    }
	  
	  if (get_info == 100000)
	    {
	      putch(0x0A);
	      get_info = 0;
	    }	  	  
	}

      update_low_battery_light();
    }
}

// Ununsed IRQs

void SWI_Routine (void)  
{
  //set_led(200);
  //putstr("\r\nSWI_Routine");
}

void UNDEF_Routine (void) 
{
  //set_led(200);
  //putstr("\r\nUndef_Routine");
}

void FIQ_Routine (void)  
{
  //set_led(200);
  //putstr("\r\nFIQ_Routine");
}
