#ifndef _VIC_H_
#define _VIC_H_

// VIC interrupt sources for the LPC210(4-6)
typedef enum
{
    VIC_WDT         = 0,
    VIC_SW          = 1,
    VIC_DEBUG_RX    = 2,
    VIC_DEBUG_TX    = 3,
    VIC_TIMER0      = 4,
    VIC_TIMER1      = 5,
    VIC_UART0       = 6,
    VIC_UART1       = 7,
    VIC_PWM0        = 8,
    VIC_I2C         = 9,
    VIC_SPI         = 10,
    // 11 is reserved
    VIC_PLL         = 12,
    VIC_RTC         = 13,
    VIC_EINT0       = 14,
    VIC_EINT1       = 15,
    VIC_EINT2       = 16
} vic_source_t;

typedef void (__attribute__((interrupt("IRQ"))) *irq_handler_t)();

/* Configures the VIC and enables interrupts */
void vic_init(void);

/* Sets the next available VIC vector to handler.
 * The handler must be declared like this:
 *      void handler() __attribute__ ((interrupt("IRQ")))
 *      {
 *          // Handle the interrupt here and clear it as appropriate
 *          VICVectAddr = 0;    // Update VIC logic
 *      }
 *
 * The compiler will not issue a warning or error if the attribute is wrong,
 * even though the attribute is given in the type.
 *
 * If this is called again for the same source, the new handler will replace the old one.
 */
void vic_add_handler(vic_source_t source, irq_handler_t handler);

static inline void sei()
{
    asm("mrs r0, cpsr; bic r0, r0, #0x80; msr cpsr, r0" : : : "r0");
}

static inline void cli()
{
    asm("mrs r0, cpsr; orr r0, r0, #0x80; msr cpsr, r0" : : : "r0");
}

#endif /* _VIC_H_ */
