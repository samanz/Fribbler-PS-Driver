#ifndef __INFRARED__
#define __INFRARED__

// best for gnats and ok for RCX tower

// in tenths of a microsecond
//#define NOMINAL_LOW_BIT    4075         // 4250 data - zero  (ir receiver on)
//#define NOMINAL_HIGH_BIT   3950         // 3900 data - one   (ir recevier off)

// in tenths of a microsecond
#define NOMINAL_LOW_BIT    4075         // data - zero  (ir receiver on)
#define NOMINAL_HIGH_BIT   4100         // data - one   (ir recevier off)

#define MIN_BIT_TIME       2000

#define PACKET_TIMEOUT     50000        // for reseting packet after pause

// in tenths of a microseconds
//#define XMIT_EMIT_TIME      3600        // data - zero (ir receiver on)
//#define XMIT_REST_TIME      4000        // data - one (ir receiver off)

#define XMIT_EMIT_TIME      4000        // data - zero (ir receiver on)
#define XMIT_REST_TIME      4400        // data - one (ir receiver off)

#define XMIT_CYCLE_OFF_TIME   130      // how long are we off for of 40khz
#define XMIT_CYCLE_ON_TIME    120      // how long are we on for of the 40Khz

void ir_rx_init();
void ir_rx_disable();
int  ir_queue_full();
int  ir_queue_read(int* data);
void ir_send_byte(uint8_t data, uint32_t emitter);

void ir_irq() __attribute__ ((interrupt("IRQ")));
void process_packet(int data);

extern uint8_t* msg_buffer;
extern volatile  int msg_size;


#endif /* __INFRARED__*/

