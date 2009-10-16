#include "lpc210x.h"
#include "lpcUART.h"
#include "vic.h"
#include "fluke.h"
#include "infrared.h"

#include "vision.h"

extern unsigned char rle[RLE_BUFFER_SIZE+4] __attribute__((aligned(4)));

#define MSG_BUFFER_SIZE (RLE_BUFFER_SIZE)

volatile int producer_index = 0;
volatile int consumer_index = 0;
volatile int msg_size = 0;

uint8_t* msg_buffer;

void ir_rx_init()
{

  msg_buffer = (uint8_t*)(rle);
 
  // enable capture on IRIN (pin 0.10) uses timer 1
  PINSEL0 |= (2 << 20);
  
  // setup prescalar of TIMER1 such that a tick is 0.1 usecs (or 10 ticks per usec)
  TIMER1_PR   = 0x5;     // set prescalar 
  
  // reset TIMER 1
  TIMER1_TCR  = 0x2;     // reset counter

  // configure capture register
  
  // 0 Capture on capture[0] rising edge When one, a sequence of 0
  // then 1 on capture[0] will cause CR0 to be loaded with the
  // contents of the TC. When zero this feature is disabled.
  
  // 1 Capture on capture[0] falling edge When one, a sequence of 1
  // then 0 on capture[0] will cause CR0 to be loaded with the
  // contents of TC. When zero this feature is disabled. 

  // 2 Interrupt on capture[0] event When one, a CR0 load due to a
  // capture[0] event will generate an interrupt. When zero this
  // feature is disabled.

  TIMER1_CCR |= 0x7;     

  // start TIMER1
  TIMER1_TCR  = 0x1;      

  // add the handler for TIMER1
  vic_add_handler(VIC_TIMER1, ir_irq);

}

void ir_rx_disable()
{
  // disable capture on IRIN (pin 0.10) uses timer 1
  PINSEL0 &= ~(2 << 20);
  TIMER1_CCR &= ~0x7;
}


void ir_irq()
{  
  static uint32_t current_time = 0;
  static uint32_t last_time = 0;
  static uint32_t temp;
  
  static enum {FALLING = 0, RISING = 1} pinstate = FALLING;
  static enum {START_BIT, DATA_BIT, PARITY_BIT, STOP_BIT} bitstate = STOP_BIT;  
  static uint8_t data_bit = 0;
  static int data = 0;
  static int parity = 0;
  static int bits = 0;
  static uint8_t reset = 0;
  static uint32_t delay = 0;

  bits = 0;
  reset =0;
  delay = 0;

  if (TIMER1_TC >= PACKET_TIMEOUT)
    {
      delay = TIMER1_TC;        

      reset = 1;
      
      TIMER1_TCR  = 0x2;       // reset counter
      TIMER1_TCR  = 0x1;       // start counter	  
      current_time = 0;
      last_time = 0;
      
      // disable the interrupt 
      temp = TIMER1_MCR & ~0x1;   
      TIMER1_MCR = temp;  

      TIMER1_IR = (1 << 0);    // clear the interrupt for match 0
    }
  else
    {
      current_time = TIMER1_CR0;  
      delay = current_time - last_time;
      last_time = current_time;

      reset = 0;
      
      TIMER1_IR = (1 << 4);    // clear the interrupt for capture 1.0
    }

  
  bits = 0;
  
  // Hiccup, ignore this pulse
  if (delay > MIN_BIT_TIME) 
    {
      
      if (pinstate == FALLING)
	{
	  if (delay <= NOMINAL_HIGH_BIT)
	    bits = 1;
	  else
	    bits = (delay + NOMINAL_HIGH_BIT/2) / NOMINAL_HIGH_BIT;
	}
      else
	{
	  if (delay <= NOMINAL_LOW_BIT)
	    bits = 1;
	  else
	    bits = (delay + NOMINAL_LOW_BIT/2) / NOMINAL_LOW_BIT;
	}
      
      if (bitstate == START_BIT && pinstate && bits)
	{
	  
	  bits --;
	  bitstate = DATA_BIT;
	  parity = 0;	  
	  data_bit = 0;
	  data = 0;
	  
	  // interrupt us every 5 milliseconds to make sure we catch
	  // the end of the packet
	  TIMER1_MR0 = PACKET_TIMEOUT;      
	  
	  // interrupt when packet should be done  
	  temp = TIMER1_MCR | 0x1; 
	  TIMER1_MCR = temp;   

	}
      
      if (bitstate == DATA_BIT && bits)
	{
	  while (data_bit < 8 && bits > 0)
	    {
	      if (!pinstate)
		{
		  data |= (1 << data_bit);		  
		  parity ++;
		  
		}
	      bits --;
	      data_bit++;
	    }
	  
	  if (data_bit == 8)
	    {
	      bitstate = PARITY_BIT;
	    }
	}
      
      if (bitstate == PARITY_BIT && bits)
	{	      
	  
	  if ((parity & 0x1) == pinstate)
	    {
	      data_bit = 9;
	    }
	  else
	    {
	    }
	  
	  bitstate = STOP_BIT;
	  bits --;
	}	  
      
      if (bitstate == STOP_BIT && bits && !pinstate)
	{	      
	  bitstate = START_BIT;
	  bits --;
	  
	  if (data_bit == 9)
	    {
	      process_packet(data);
	      data_bit = 0;
	    }
	  data = 0;	  

	  // disable the interrupt 
	  temp = TIMER1_MCR & ~0x1;   
	  TIMER1_MCR = temp;  

	  TIMER1_TCR  = 0x2;       // reset counter
	  TIMER1_TCR  = 0x1;       // start counter
	  current_time = 0;
	  last_time = 0;	  
	}
      
      if (!reset)  
	{
	  pinstate = !pinstate;
	}
      else
	{
	  bitstate = STOP_BIT;
	  pinstate = FALLING;
	}
    }
      
      VICVectAddr = 0;
}

int ir_queue_full()
{
  // Since the queue is empty when read==write,
  // the queue is full when the next character written would
  // produce an incorrect queue-empty indication
  // (write+1==read).

  int next_write;

  next_write = producer_index + 1;  
  
  if (next_write == MSG_BUFFER_SIZE)
    {
      next_write = 0;
    }
  
  return (next_write == consumer_index); 
}


int ir_queue_write(uint32_t data)
{
  int next_write = 0;
  
  next_write = producer_index + 1;  
  
  if (next_write == MSG_BUFFER_SIZE)
    {
      next_write = 0;
    }
  
  if (next_write != consumer_index)
    { 
      msg_buffer[producer_index] = data;
      
      producer_index ++;
      msg_size ++;

      if (producer_index == MSG_BUFFER_SIZE)
	{
	  producer_index = 0;
	}
      return 0;
    }

  return -1;
}


int ir_queue_read(int *data)
{
  (*data) = 0;

  if (consumer_index != producer_index)
    {
      // read the data
      (*data) = msg_buffer[consumer_index];
      
      // Move to the next item in the queue
      consumer_index++;
      msg_size --;
      
      // might have to move to the next byte
      
      if (consumer_index == MSG_BUFFER_SIZE)
	{
	  consumer_index = 0;
	}
      
      return 1;
    }

  // empty
  return -1;
}

//#define DEBUG_IR

// high-level IR processing packet level
void process_packet(int data)
{  
  ir_queue_write(data);
}


void ir_rest(uint32_t emitter)
{
  // perscalar should be about 1 usec
  TIMER0_TCR  = 0x2;     // reset counter
  TIMER0_TCR  = 0x1;     // start counter
  while (TIMER0_TC < XMIT_REST_TIME)
    {
      // spin
    }
}

void ir_emit(uint32_t emitter)
{
  uint32_t total = 0;

  // prescalar should be .1 usec

  TIMER0_TCR  = 0x2;     // reset counter
  TIMER0_TCR  = 0x1;     // start counter

  while (total < XMIT_EMIT_TIME)
    {
      TIMER0_TCR  = 0x2;     // reset counter
      TIMER0_TCR  = 0x1;     // start counter
      while (TIMER0_TC < XMIT_CYCLE_OFF_TIME)
	{
	}
      total += TIMER0_TC;
      IOSET = emitter;

      if (total >XMIT_EMIT_TIME) break;

      TIMER0_TCR  = 0x2;     // reset counter
      TIMER0_TCR  = 0x1;     // start counter
  
      while (TIMER0_TC < XMIT_CYCLE_ON_TIME)
	{
	}
      total += TIMER0_TC;
      IOCLR = emitter;
    }
}

void ir_send_byte(uint8_t data, uint32_t emitter)
{
  uint8_t i;
  uint8_t parity = 0;
  uint8_t bit;
  uint32_t old_prescalar;
  
  old_prescalar = TIMER0_PR;

  // setup prescalar of TIMER1 such that a tick is 0.1 usecs (or 10 ticks per usec)
  TIMER0_PR = 0x5;

  // start bit
  ir_emit(emitter);

  // 8 data bits LSB->MSB
  for (i = 0; i < 8; i++)
    {
      bit = (data >> i) &  0x1;
      if (bit)
	{
	  ir_rest(emitter);
	  parity ++;
	}	
      else
	{
	  ir_emit(emitter);
	}
    }

  // Parity bit
  if (parity & 0x01)
    {
      ir_emit(emitter);
    }
  else
    {
      ir_rest(emitter);
    }

  // stop bit
  ir_rest(emitter);  

  TIMER0_PR = old_prescalar;
}





