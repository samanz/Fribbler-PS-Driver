/*******************************************************************************
 *
 * Fluke library calls
 * 
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/
 

#include "lpc210x.h"
#include "uart.h"
#include "ov7649.h"
#include "fluke.h"
#include "infrared.h"

int getchblock2b()
{
  int addr;
  addr = getchblock();
  addr = (addr << 8);
  addr = addr | getchblock();	      
  return addr;
}

void putch2b(int data)
{
  putch(data >> 8);
  putch(data); 
}

void write_word(uint32_t word)
{
  putch(word & 0xff);
  putch((word >> 8) & 0xff);
  putch((word >> 16) & 0xff);
  putch((word >> 24) & 0xff);
}


void d2a_wait()
{
  usleep(100);
}

void set_d2a(int power, int channel)
{
  int i, data;

  data = (power & 0xFF) | channel;
  
  IOCLR=D2A_CS; d2a_wait();
  for (i = 11; i >= 0; i--)
    {
      IOCLR=MCLK; d2a_wait();
      if (data & (1 << i))
	{
	  IOSET = MDIN;
	}
      else
	{
	  IOCLR = MDIN;
	}	
      d2a_wait();
      IOSET=MCLK; d2a_wait();
    }
  
  IOCLR = MCLK; d2a_wait();  
  IOSET = D2A_CS;  d2a_wait();
}

void set_led(int power)
{
  set_d2a(power, D2A_CHAN_LED);
}

void led_on()
{
  IOSET = LED;
}

void led_off()
{
  IOCLR = LED;
}

void set_ir(int power)
{
  set_d2a(power, D2A_CHAN_IR);
}

void set_bluetooth(int power)
{
  set_d2a(power, D2A_CHAN_BT);
}

void setup_pins()
{
  IODIR = (S_RST  | MCLK   | MDIN   | LED   | D2A_CS | A2D_CS | MCS | 
	   IROUT1 | IROUT2 | IROUT3 | B_TXD | B_RTS);
  IOSET = (D2A_CS | A2D_CS | MCS | B_TXD);
}

void setup_pll()
{
  // --- enable and connect the PLL (Phase Locked Loop) ---
  // a. set multiplier and divider
  //     = MSEL | (1 << 6) | (0 << 5)
  //PLLCFG = MSEL | (1<<PSEL1) | (0<<PSEL0);
  PLLCFG = MSEL | (PSEL << 5);
  // b. enable PLL
  PLLCON = (1<<PLLE);
  // c. feed sequence
  PLLFEED = PLL_FEED1;
  PLLFEED = PLL_FEED2;
  // d. wait for PLL lock (PLOCK bit is set if locked)
  while (!(PLLSTAT & (1<<PLOCK)));
  // e. connect (and enable) PLL
  PLLCON = (1<<PLLE) | (1<<PLLC);
  // f. feed sequence
  PLLFEED = PLL_FEED1;
  PLLFEED = PLL_FEED2;

  VPBDIV = 1;
}

void setup_mam()
{  
  // --- setup and enable the MAM (Memory Accelerator Module) ---
  // a. start change by turning of the MAM (redundant)
  MAMCR = 0;	
  // b. set MAM-Fetch 
  MAMTIM = 2;
  // c. enable MAM 
  MAMCR = 2;
}

unsigned int temp = 0;
inline void spi_wait()
{
  while (!(SPI_SPSR & (1<<7)))
    {
    }
  temp = SPI_SPSR;
}

inline void spi_init()
{
  PINSEL0 = 
    (PINSEL0 & 
     ~((3 << 8) | (3 << 10) | (3 << 12)))
    | ((1 << 8) | (1 << 10) | (1 << 12));
}

inline void spi_fini()
{
  PINSEL0 &= ~((3 << 8) | (3 << 10) | (3 << 12));
}

int read_battery_spi()
{
  int voltage = 0;

  spi_init();
  IOCLR = A2D_CS;
  IOCLR = S_RST;
  SPI_SPCCR = 16;
  SPI_SPCR = 0x20;
  SPI_SPDR = 0;
  spi_wait();
  SPI_SPDR = 0;
  spi_wait();
  voltage = SPI_SPDR;

  IOSET = A2D_CS;
  spi_fini();

  return voltage;
}


int read_a2d(int channel)
{
  int voltage = 0;
  int i, j;

  IOCLR = A2D_CS;    
  // have to do this twice
  for (j = 0; j < 2; j++)
    {
      voltage = 0;
      for (i = 0; i < 16; i++)
	{
	  IOCLR = MCLK;usleep(1);
	  if ((i == 4) && (channel == 1))
	  {
	    IOSET = MDIN;usleep(1);
	  }
	  else
	    {
	      IOCLR = MDIN;usleep(1);
	    }
	  IOSET = MCLK;usleep(1);
	  if (i > 3 && i < 12)
	    {
	      voltage <<= 1;
	      if (IOPIN & MDOUT)
		{
		  voltage |=  1;
		}
	    }
	}
    }

  IOCLR = MCLK;
  IOSET = A2D_CS;
  return voltage;
}


void wb_spi(int byte)
{  
  int i;

  for (i = 7; i >= 0; i--)
    {
      IOCLR = MCLK; usleep(MEM_DELAY);
      if (byte & (1 << i))
	{	  
	  IOSET = MDIN;
	}
      else
	{	  
	  IOCLR = MDIN;
	}
      usleep(MEM_DELAY);
      IOSET = MCLK; usleep(MEM_DELAY);
    } 

  IOCLR = MCLK; usleep(MEM_DELAY);
}

unsigned char rb_spi()
{  
  int i;
  unsigned char byte = 0;

  for (i = 7; i >= 0; i--)
    {
      byte <<= 1;
      IOCLR = MCLK; usleep(MEM_DELAY);
      IOSET = MCLK; usleep(MEM_DELAY);
      if (IOPIN & MDOUT)
	{
	  byte |=  1;
	}            
    }
  
  IOCLR = MCLK; usleep(MEM_DELAY);
  
  return byte;
}

void read_mem_buffer(int offset, unsigned char* buf, int size)
{
  int i;
  unsigned char data = 0;
  int address;

  IOCLR = MCS; 
  usleep(MEM_DELAY);
   
  // 8 command bits
  wb_spi(MEM_BUF_READ);

  // 15 don't care : 9 address : 8 don't care = 32
  address = (offset << 8);
  
  // 32 bits of the address
  for (i = 24; i >= 0; i-=8)
    { 
      data = 0xFF & (address >> i);
      wb_spi(data);
    }
  
  // read the data
  for (i = 0; i < size; i++)
    {
      *buf = rb_spi();
      buf ++;
    }
  
  IOSET = MCS;  
}

void write_mem_buffer(int offset, unsigned char* buf, int size)
{
  int i;
  unsigned char data;

  IOCLR = MCS;
  usleep(MEM_DELAY);
  
  // write data to buffer 
  wb_spi(MEM_BUF_WRITE);
  
  for (i = 16; i >= 0; i-= 8)
    { 
      data = 0xFF & (offset >> i);
      wb_spi(data);
    }

  for (i = 0; i < size; i++)
    {
      //putstr("write = ");
      //printdec(*buf);
      //putstr("\r\n");
      wb_spi(*buf);
      buf++;
    }

  IOSET = MCS;
  usleep(MEM_DELAY);

}

__attribute__ ((section (".data"), long_call)) 
void read_mem(int page, int offset, unsigned char* buf, int size)
{
  int i;
  unsigned char data = 0;
  int address;

  IOCLR = MCS; 
  usleep(MEM_DELAY);
  
  // 8 command bits
  wb_spi(MEM_PAGE_READ);

  // 6 reserved : 9 page address : 9 address = 24 bits
  address = (page << 9) | (offset);
  
  // 24 bits of the address
  for (i = 16; i >= 0; i -= 8)
    { 
      data = 0xFF & (address >> i);
      wb_spi(data);
    }
  
  // 32 don't care bits
  for (i = 0; i < 4; i++)
    { 
      wb_spi(0);
    }
  
  // read the data
  for (i = 0; i < size; i++)
    {
      *buf = rb_spi();
      buf ++;
    }
  
  IOSET = MCS;
}

void erase_mem(int page)
{
  int i;
  unsigned char data;

  // 6 reserved : 9 address : 9 don't care
  page = page << 9;
  
  IOCLR = MCS;
  usleep(MEM_DELAY);
  
  // load data from memory to buffer
  wb_spi(MEM_PAGE_ERASE);
  
  for (i = 16; i >= 0; i -= 8)
    { 
      data = 0xFF & (page >> i);
      wb_spi(data);    
    }

  IOSET = MCS;  

  // spin until the data has been written
  usleep(MEM_DELAY);
  IOCLR = MCS;
  usleep(MEM_DELAY);

  wb_spi(MEM_READ_STATUS);

  data = 0;
  while (!(data & (1<<7)))
    {     
      data = rb_spi();
    }
  IOSET = MCS;  
}

void load_mem(int page)
{
  int i;
  unsigned char data;

  // 6 reserved : 9 address : 9 don't care
  page = page << 9;
  
  IOCLR = MCS;
  usleep(MEM_DELAY);
  
  // load data from memory to buffer
  wb_spi(MAIN_MEMORY_PAGE_TO_BUFFER);
  
  for (i = 16; i >= 0; i -= 8)
    { 
      data = 0xFF & (page >> i);
      wb_spi(data);    
    }

  IOSET = MCS;  

  // spin until the data has been written
  usleep(MEM_DELAY);
  IOCLR = MCS;
  usleep(MEM_DELAY);

  wb_spi(MEM_READ_STATUS);

  data = 0;
  while (!(data & (1<<7)))
    {     
      data = rb_spi();
    }
  IOSET = MCS;  
}

void write_mem(int page, int offset, unsigned char* buf, int size)
{
  // load the page from memory into the buffer
  load_mem(page);
  usleep(MEM_DELAY);

  // write values into buffer
  write_mem_buffer(offset, buf, size);
  usleep(MEM_DELAY);

  // write buffer to memory
  mem_buffer_to_mem(page);
}

void mem_buffer_to_mem(int page)
{
  int i;
  unsigned char data;

  IOCLR = MCS;
  usleep(MEM_DELAY);
  
  // now transfer buffer to memory
  wb_spi(MEM_BUF_MEM);
  
  // 6 reserved : 9 page address : 9 don't care
  page = page << 9;
  
  data = 0;
  for (i = 16; i >= 0; i -= 8)
    { 
      data = 0xFF & (page >> i);
      wb_spi(data);
    }

  IOSET = MCS;
  usleep(MEM_DELAY);

  // spin until the data has been written
  IOCLR = MCS;
  usleep(MEM_DELAY);

  wb_spi(MEM_READ_STATUS);
  
  data = 0;
  while (!(data & (1<<7)))
    {     
      data = rb_spi();
    }
  
  IOSET = MCS;  
}


void printdec(int sum)
{
  char buf[16];
  int ch = 0;
  int i = 0;

  if (sum == 0) 
    {
      putch('0');
    }
  else if( sum < 0 )
    {
      sum *= -1;
      putch('-');
    }

  while (sum > 0 && i < 16)
    {
      ch = sum % 10;
      sum = sum / 10;
      buf[i] = ch + '0';
      i ++;
    }

  i --;
  while (i >= 0)
    {
      putch(buf[i]);
      i--;
    }
}


void printbin(int sum)
{
  int i = 0;
  for (i = 31; i >= 0; i--)
    {
      if (sum & (1<<i))
	{
	  putch('1');
	}
      else
	{
	  putch('0');
	}
    }
}

void putstr(char* buf)
{
  int i = 0;
  while (buf[i] != '\0')
    {
      putch(buf[i]);
      i++;
    }
}

void putstrn(char* buf, int size)
{
  int i;

  for (i = 0; i < size && buf[i] != '\0'; i++)
    {
      putch(buf[i]);
    }
}


void dbg_printdec(int sum)
{
  char buf[16];
  int ch = 0;
  int i = 0;

  if (sum == 0) dbg_putch('0');

  while (sum > 0 && i < 16)
    {
      ch = sum % 10;
      sum = sum / 10;
      buf[i] = ch + '0';
      i ++;
    }

  i --;
  while (i >= 0)
    {
      dbg_putch(buf[i]);
      i--;
    }
}

void dbg_printbin(int sum)
{
  int i = 0;
  for (i = 31; i >= 0; i--)
    {
      if (sum & (1<<i))
	{
	  dbg_putch('1');
	}
      else
	{
	  dbg_putch('0');
	}
    }
}

void dbg_putstr(char* buf)
{
  int i = 0;
  while (buf[i] != '\0')
    {
      dbg_putch(buf[i]);
      i++;
    }
}

void dbg_putstrn(char* buf, int size)
{
  int i;

  for (i = 0; i < size && buf[i] != '\0'; i++)
    {
      dbg_putch(buf[i]);
    }
}


void grab_image_vote(int times, unsigned char* img, int delay)
{
  int avg_u, navg_u;
  int avg_v, navg_v;
  int i;

  avg_u = 0;
  avg_v = 0;
  grab_frame(img, delay);

  for (i = 0; i < WIDTH_SIZE*HEIGHT_SIZE; i += 384)
    {
      avg_u += img[i+0];
      avg_v += img[i+2];
    }     	      
  
  do
    {
      navg_u = 0;
      navg_v = 0;
      grab_frame(img, delay);
      for (i = 0; i < WIDTH_SIZE*HEIGHT_SIZE; i += 384)
	{
	  navg_u += img[i+0];
	  navg_v += img[i+2];
	}
      
      if (((((navg_u - avg_u) > 1000) || ((avg_u - navg_u) > 1000)) ||
	   (((navg_v - avg_v) > 1000) || ((avg_v - navg_v) > 1000))))
	{
	  //set_led(250);
	  times --;
	  avg_u = navg_u;
	  avg_v = navg_v;
	}
      else
	{
	  //set_led(0);
	  times = 0;
	}

    }  while (times);
	
}


void emit_on(uint8_t emitters)
{
  TIMER1_TCR = 0x2;      // Reset Counter  
  
  if (emitters & 0x01) // GET_DONGLE_R_IR)
    {
      PINSEL0 |= (1 << 27);  // Match 1.1 (pin 13/IROUT1)  PROG   (R)
      TIMER1_EMR |= 0x0C2;     // set external match - toggle 1.1
    }
  
  if (emitters & 0x02) //GET_DONGLE_L_IR)
    {
      PINSEL1 |= (1 << 6);   // Match 1.3 (pin 19/IROUT2)  IPRE   (L)
      TIMER1_EMR |= 0x304;    // set external match - toggle 1.2
    }
  
  if (emitters & 0x04) //GET_DONGLE_C_IR)
    {

      PINSEL1 |= (1 << 8);   // Match 1.3 (pin 20/IROUT3)  MIDDLE (M)
      TIMER1_EMR |= 0xC08;    // set external match - toggle 1.3
    }
  
  TIMER1_TCR = 0x01;     // start timer
}


void emit_off(uint8_t emitters)
{
  TIMER1_TCR = 0x02;   // reset timer 
  
  if (emitters & 0x01) // GET_DONGLE_R_IR)
    {
      TIMER1_EMR &= ~0x0C2;     // set external match - toggle 1.1
      PINSEL0 &= ~(1 << 27);  // Match 1.1 (pin 13/IROUT1)  PROG   (R)
    }
  
  if (emitters & 0x02) //GET_DONGLE_L_IR)
    {
      TIMER1_EMR &= ~0x304;    // set external match - toggle 1.2
      PINSEL1 &= ~(1 << 6);   // Match 1.3 (pin 19/IROUT2)  IPRE   (L)
    }
  
  if (emitters & 0x04) //GET_DONGLE_C_IR)
    {
      TIMER1_EMR &= ~0xC08;    // set external match - toggle 1.3
      PINSEL1 &= ~(1 << 8);   // Match 1.3 (pin 20/IROUT3)  MIDDLE (M)
      
    }  
}


/*
 * Process an IR message - uses one or the 3 emitters and
 * the receiver to detect an obstacle. 
 * 
 * Pulses IR periodically and keeps a counter of reflected
 * IR.  If it receives IR when it itself isn't sending it
 * decrements the counter to trya nd remove false positives.
 *
 * This can definitely be improved.
 */

int check_ir_bounce(uint8_t emitters)
{
  int i, j, pulses;

  ir_rx_disable();

  TIMER1_PR =  0x22;     // Load Prescalar
  TIMER1_MR1 = 0x14;     // set MR1 to 40Khz
  TIMER1_MCR = (1 << 4); // On match (MR1) reset the counter

  i = 0;
  pulses = 0;	      
  for (j = 0; j < 15000; j++)
    {  		  
      if (i == 0) 
	{
	  emit_on(emitters);
	}		  
      else if (i < 2000)
	{
	  // we received some IR - as expected
	  if (!(IOPIN & IRIN)) 
	    {
	      pulses ++;
	    }		      
	}
      else if (i == 2000)
	{
	  emit_off(emitters);
	}
      else if (i < 4000) 
	{		     
	  // we received some IR - but we shouldn't have
	  if (!(IOPIN & IRIN))
	    {
	      if (pulses > 0) 
		{
		  pulses --;
		}
	    } 	      
	}
      else 
	{
	  i = 0;
	}
      
      i++;
    }    
  
  // turn off all emitters
  emit_off(emitters);

  TIMER1_MCR &= ~(1 << 4); // DISABLE On match (MR1) reset the counter

  // turn this back on
  ir_rx_init();

  return pulses;
}
