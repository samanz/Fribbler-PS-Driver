/*******************************************************************************
 *
 * OV7649 Camera Interface
 * 
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#include "lpc210x.h"
#include "inttypes.h"
#include "fluke.h"

void cam_wait()
{
  usleep(20);
}

/*
 * Camera SPI interface routines
 */
void cam_input()
{
  IODIR |=  CAM_SC;
  IODIR &= ~MDIN;
}

void cam_output()
{
  IODIR |= (CAM_SC | MDIN);
}

static void clk_high()
{
  IOSET = CAM_SC; 
  cam_wait(); 
}

static void clk_low()
{
  IOCLR = CAM_SC; 
  cam_wait(); 
}

static void data_high()
{
  IOSET = MDIN; 
  cam_wait(); 
}

static void data_low()
{
  IOCLR = MDIN; 
  cam_wait(); 
}


void start_trans()
{
  cam_output();

  clk_high();
  data_high();
  data_low();  
}


void stop_trans()
{
  cam_output();

  clk_high();
  data_low();
  data_high();
}


uint8_t send_addr(uint8_t address, uint8_t read)
{
  int i;
  uint8_t status = 0;
  
  cam_output();

  // send 7 bits of address
  for (i = 6; i >= 0; i--)
    {
      clk_low();
      if (address & (1 << i))
	{
	  data_high();
	}
      else
	{
	  data_low();
	}
      clk_high();
    }

  // send R/W bit
  clk_low();
  if (read)
    {
      data_high();
    }
  else
    {
      data_low();
    }
  clk_high();

  clk_low();

  // get ack

  data_high();

  cam_input();  

  clk_low();

  if (IOPIN & MDIN)
    {
      status = -1;
    }

  clk_high();

  return status;
}

uint8_t send_subaddr(uint8_t address)
{
  int i;
  uint8_t status = 0;

  cam_output();

  // send 8 bits of command
  for (i = 7; i >= 0; i--)
    {
      clk_low();
      if (address & (1 << i))
	{
	  data_high();
	}
      else
	{
	  data_low();
	}
      clk_high();
    }

  clk_low();

  // handle ack
  data_high();
  
  cam_input();

  clk_low();

  if (IOPIN & MDIN)
    {
      status = -1;
    }
  
  clk_high();
  clk_low();

  return status;
}

uint8_t read_cdata()
{
  int i;
  uint8_t data;
  data = 0;

  data_low();
  cam_input();
  
  //read the data in 
  for (i = 7; i >= 0; i--)
    {
      clk_low();  
      clk_high();
      if (IOPIN & MDIN)
	{
	  data |= (1 << i);
	}
    }

  cam_output();
  data_high();

  // send ack
  clk_low();
  clk_high();

  data_low();
  clk_low();

  return data;
}

uint8_t read_camera(uint8_t address, uint8_t *udata)
{
  uint8_t data = 0;
  uint8_t status = 0;

  start_trans();
  status |= send_addr(0x21, 0);
  status |= send_subaddr(address);  
  stop_trans();
  
  cam_wait();
  
  start_trans();
  status |=  send_addr(0x21, 1);
  data = read_cdata();
  stop_trans();
  
  *udata = data;
  return status;
}

uint8_t write_camera(uint8_t address, uint8_t data)
{
  uint8_t status = 0;

  start_trans();
  status |= send_addr(0x21, 0);
  status |= send_subaddr(address);  
  status |= send_subaddr(data);
  stop_trans();
  
  return status;
}
