/******************************************************************************
 *
 * UART Ring-Buffer
 * 
 * Ben Johnson <circuitben@gmail.com>
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#include "uart_queue.h"
#include "vic.h"
#include "lpc210x.h"
#include "lpcUART.h"

#define UART1_TX_QUEUE_SIZE     256
char uart1_tx_queue[UART1_TX_QUEUE_SIZE];

// The index in uart1_tx_queue of the next character to be sent
// by uart1_tx_handler().
// This may only be modified by uart1_tx_handler().
volatile int uart1_tx_read;

// The index in uart1_tx_queue of the next character to be written
// by uart1_write().  This must not be changed by any interrupt handler.
volatile int uart1_tx_write;

int uart1_tx_full()
{
    // Since the queue is empty when read==write,
    // the queue is full when the next character written would
    // produce an incorrect queue-empty indication
    // (write+1==read).
    int next_write = uart1_tx_write + 1;
    if (next_write == UART1_TX_QUEUE_SIZE)
    {
        next_write = 0;
    }
    return next_write == uart1_tx_read;
}

void uart1_queue_write(char ch)
{
    // Wait for space to become available
    while (uart1_tx_full());
    
    // Add this character to the queue
    uart1_tx_queue[uart1_tx_write] = ch;
    
    // Advance to the next write position
    uart1_tx_write++;
    if (uart1_tx_write == UART1_TX_QUEUE_SIZE)
    {
        uart1_tx_write = 0;
    }
    
    // Enable THRE and status change interrupts
    UART1_IER = 0x0a;
}

void uart1_tx_handler()
{
    if (uart1_tx_read != uart1_tx_write && (UART1_LSR & ULSR_THRE) && (UART1_MSR & UMSR_CTS))
    {
        // Send this byte
        UART1_THR = uart1_tx_queue[uart1_tx_read];
        
        // Move to the next byte in the queue
        uart1_tx_read++;
        if (uart1_tx_read == UART1_TX_QUEUE_SIZE)
        {
            uart1_tx_read = 0;
        }
        
        // If the queue is now empty, disable this interrupt
        if (uart1_tx_read == uart1_tx_write)
        {
            UART1_IER = 0;
        }
    }
}

// UART1 interrupt handler
void vic_uart1() __attribute__ ((interrupt("IRQ")));
void vic_uart1()
{
    int id = UART1_IIR & 0x0f;
    if (id == 0 || id == 2)
    {
        // THRE or CTS change
        uart1_tx_handler();
        
        // Reading UART1_IIR and UART1_MSR cleared the interrupt
    }
    
    VICVectAddr = 0;
}

void uart1_queue_init()
{
    vic_add_handler(VIC_UART1, vic_uart1);
}
