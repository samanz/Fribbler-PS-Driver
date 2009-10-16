/******************************************************************************
 *
 * UART Ring-Buffer
 * 
 * Ben Johnson <circuitben@gmail.com>
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#ifndef _UART_QUEUE_H_
#define _UART_QUEUE_H_

void uart1_queue_write(char ch);
void uart1_queue_init(void);

#endif /* _UART_QUEUE_H_ */
