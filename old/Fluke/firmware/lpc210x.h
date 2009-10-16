/******************************************************************************
 *
 * LPC2106 is the Fluke's microcontroller
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/
 

// ***********************************************************************
//
//  LPC210X.H:  Header file for Philips LPC2104 / LPC2105 / LPC2106
//
// ***********************************************************************

#ifndef __LPC210x_H
#define __LPC210x_H

/* Vectored Interrupt Controller (VIC) */
#define VICIRQStatus   (*((volatile unsigned long *) 0xFFFFF000))
#define VICFIQStatus   (*((volatile unsigned long *) 0xFFFFF004))
#define VICRawIntr     (*((volatile unsigned long *) 0xFFFFF008))
#define VICIntSelect   (*((volatile unsigned long *) 0xFFFFF00C))
#define VICIntEnable   (*((volatile unsigned long *) 0xFFFFF010))
#define VICIntEnClr    (*((volatile unsigned long *) 0xFFFFF014))
#define VICSoftInt     (*((volatile unsigned long *) 0xFFFFF018))
#define VICSoftIntClr  (*((volatile unsigned long *) 0xFFFFF01C))
#define VICProtection  (*((volatile unsigned long *) 0xFFFFF020))
#define VICVectAddr    (*((volatile unsigned long *) 0xFFFFF030))
#define VICDefVectAddr (*((volatile unsigned long *) 0xFFFFF034))
#define VICVectAddr0   (*((volatile unsigned long *) 0xFFFFF100))
#define VICVectAddr1   (*((volatile unsigned long *) 0xFFFFF104))
#define VICVectAddr2   (*((volatile unsigned long *) 0xFFFFF108))
#define VICVectAddr3   (*((volatile unsigned long *) 0xFFFFF10C))
#define VICVectAddr4   (*((volatile unsigned long *) 0xFFFFF110))
#define VICVectAddr5   (*((volatile unsigned long *) 0xFFFFF114))
#define VICVectAddr6   (*((volatile unsigned long *) 0xFFFFF118))
#define VICVectAddr7   (*((volatile unsigned long *) 0xFFFFF11C))
#define VICVectAddr8   (*((volatile unsigned long *) 0xFFFFF120))
#define VICVectAddr9   (*((volatile unsigned long *) 0xFFFFF124))
#define VICVectAddr10  (*((volatile unsigned long *) 0xFFFFF128))
#define VICVectAddr11  (*((volatile unsigned long *) 0xFFFFF12C))
#define VICVectAddr12  (*((volatile unsigned long *) 0xFFFFF130))
#define VICVectAddr13  (*((volatile unsigned long *) 0xFFFFF134))
#define VICVectAddr14  (*((volatile unsigned long *) 0xFFFFF138))
#define VICVectAddr15  (*((volatile unsigned long *) 0xFFFFF13C))
#define VICVectCntl0   (*((volatile unsigned long *) 0xFFFFF200))
#define VICVectCntl1   (*((volatile unsigned long *) 0xFFFFF204))
#define VICVectCntl2   (*((volatile unsigned long *) 0xFFFFF208))
#define VICVectCntl3   (*((volatile unsigned long *) 0xFFFFF20C))
#define VICVectCntl4   (*((volatile unsigned long *) 0xFFFFF210))
#define VICVectCntl5   (*((volatile unsigned long *) 0xFFFFF214))
#define VICVectCntl6   (*((volatile unsigned long *) 0xFFFFF218))
#define VICVectCntl7   (*((volatile unsigned long *) 0xFFFFF21C))
#define VICVectCntl8   (*((volatile unsigned long *) 0xFFFFF220))
#define VICVectCntl9   (*((volatile unsigned long *) 0xFFFFF224))
#define VICVectCntl10  (*((volatile unsigned long *) 0xFFFFF228))
#define VICVectCntl11  (*((volatile unsigned long *) 0xFFFFF22C))
#define VICVectCntl12  (*((volatile unsigned long *) 0xFFFFF230))
#define VICVectCntl13  (*((volatile unsigned long *) 0xFFFFF234))
#define VICVectCntl14  (*((volatile unsigned long *) 0xFFFFF238))
#define VICVectCntl15  (*((volatile unsigned long *) 0xFFFFF23C))

/* Pin Connect Block */
#define PINSEL0        (*((volatile unsigned long *) 0xE002C000))
#define PINSEL1        (*((volatile unsigned long *) 0xE002C004))

/* General Purpose Input/Output (GPIO) */
#define IOPIN          (*((volatile unsigned long *) 0xE0028000))
#define IOSET          (*((volatile unsigned long *) 0xE0028004))
#define IODIR          (*((volatile unsigned long *) 0xE0028008))
#define IOCLR          (*((volatile unsigned long *) 0xE002800C))

/* Memory Accelerator Module (MAM) */
#define MAMCR          (*((volatile unsigned char *) 0xE01FC000))
#define MAMTIM         (*((volatile unsigned char *) 0xE01FC004))
#define MEMMAP         (*((volatile unsigned char *) 0xE01FC040))

/* Phase Locked Loop (PLL) */
#define PLLCON         (*((volatile unsigned char *) 0xE01FC080))
#define PLLCFG         (*((volatile unsigned char *) 0xE01FC084))
#define PLLSTAT        (*((volatile unsigned short*) 0xE01FC088))
#define PLLFEED        (*((volatile unsigned char *) 0xE01FC08C))

/* VPB Divider */
#define VPBDIV         (*((volatile unsigned char *) 0xE01FC100))

/* Power Control */
#define PCON           (*((volatile unsigned char *) 0xE01FC0C0))
#define PCONP          (*((volatile unsigned long *) 0xE01FC0C4))

/* External Interrupts */
#define EXTINT         (*((volatile unsigned char *) 0xE01FC140))
#define EXTWAKE        (*((volatile unsigned char *) 0xE01FC144))

/* Timer 0 */
#define TIMER0_IR      (*((volatile unsigned long *) 0xE0004000))
#define TIMER0_TCR     (*((volatile unsigned long *) 0xE0004004))
#define TIMER0_TC      (*((volatile unsigned long *) 0xE0004008))
#define TIMER0_PR      (*((volatile unsigned long *) 0xE000400C))
#define TIMER0_PC      (*((volatile unsigned long *) 0xE0004010))
#define TIMER0_MCR     (*((volatile unsigned long *) 0xE0004014))
#define TIMER0_MR0     (*((volatile unsigned long *) 0xE0004018))
#define TIMER0_MR1     (*((volatile unsigned long *) 0xE000401C))
#define TIMER0_MR2     (*((volatile unsigned long *) 0xE0004020))
#define TIMER0_MR3     (*((volatile unsigned long *) 0xE0004024))
#define TIMER0_CCR     (*((volatile unsigned long *) 0xE0004028))
#define TIMER0_CR0     (*((volatile unsigned long *) 0xE000402C))
#define TIMER0_CR1     (*((volatile unsigned long *) 0xE0004030))
#define TIMER0_CR2     (*((volatile unsigned long *) 0xE0004034))
#define TIMER0_CR3     (*((volatile unsigned long *) 0xE0004038))
#define TIMER0_EMR     (*((volatile unsigned long *) 0xE000403C))

/* Timer 1 */
#define TIMER1_IR      (*((volatile unsigned long *) 0xE0008000))
#define TIMER1_TCR     (*((volatile unsigned long *) 0xE0008004))
#define TIMER1_TC      (*((volatile unsigned long *) 0xE0008008))
#define TIMER1_PR      (*((volatile unsigned long *) 0xE000800C))
#define TIMER1_PC      (*((volatile unsigned long *) 0xE0008010))
#define TIMER1_MCR     (*((volatile unsigned long *) 0xE0008014))
#define TIMER1_MR0     (*((volatile unsigned long *) 0xE0008018))
#define TIMER1_MR1     (*((volatile unsigned long *) 0xE000801C))
#define TIMER1_MR2     (*((volatile unsigned long *) 0xE0008020))
#define TIMER1_MR3     (*((volatile unsigned long *) 0xE0008024))
#define TIMER1_CCR     (*((volatile unsigned long *) 0xE0008028))
#define TIMER1_CR0     (*((volatile unsigned long *) 0xE000802C))
#define TIMER1_CR1     (*((volatile unsigned long *) 0xE0008030))
#define TIMER1_CR2     (*((volatile unsigned long *) 0xE0008034))
#define TIMER1_CR3     (*((volatile unsigned long *) 0xE0008038))
#define TIMER1_EMR     (*((volatile unsigned long *) 0xE000803C))

/* Pulse Width Modulator (PWM) */
#define PWM_IR         (*((volatile unsigned long *) 0xE0014000))
#define PWM_TCR        (*((volatile unsigned long *) 0xE0014004))
#define PWM_TC         (*((volatile unsigned long *) 0xE0014008))
#define PWM_PR         (*((volatile unsigned long *) 0xE001400C))
#define PWM_PC         (*((volatile unsigned long *) 0xE0014010))
#define PWM_MCR        (*((volatile unsigned long *) 0xE0014014))
#define PWM_MR0        (*((volatile unsigned long *) 0xE0014018))
#define PWM_MR1        (*((volatile unsigned long *) 0xE001401C))
#define PWM_MR2        (*((volatile unsigned long *) 0xE0014020))
#define PWM_MR3        (*((volatile unsigned long *) 0xE0014024))
#define PWM_MR4        (*((volatile unsigned long *) 0xE0014040))
#define PWM_MR5        (*((volatile unsigned long *) 0xE0014044))
#define PWM_MR6        (*((volatile unsigned long *) 0xE0014048))
#define PWM_CCR        (*((volatile unsigned long *) 0xE0014028))
#define PWM_CR0        (*((volatile unsigned long *) 0xE001402C))
#define PWM_CR1        (*((volatile unsigned long *) 0xE0014030))
#define PWM_CR2        (*((volatile unsigned long *) 0xE0014034))
#define PWM_CR3        (*((volatile unsigned long *) 0xE0014038))
#define PWM_EMR        (*((volatile unsigned long *) 0xE001403C))
#define PWM_PCR        (*((volatile unsigned long *) 0xE001404C))
#define PWM_LER        (*((volatile unsigned long *) 0xE0014050))

/* Universal Asynchronous Receiver Transmitter 0 (UART0) */
#define UART0_RBR      (*((volatile unsigned char *) 0xE000C000))
#define UART0_THR      (*((volatile unsigned char *) 0xE000C000))
#define UART0_IER      (*((volatile unsigned char *) 0xE000C004))
#define UART0_IIR      (*((volatile unsigned char *) 0xE000C008))
#define UART0_FCR      (*((volatile unsigned char *) 0xE000C008))
#define UART0_LCR      (*((volatile unsigned char *) 0xE000C00C))
#define UART0_MCR      (*((volatile unsigned char *) 0xE000C010))
#define UART0_LSR      (*((volatile unsigned char *) 0xE000C014))
#define UART0_MSR      (*((volatile unsigned char *) 0xE000C018))
#define UART0_SCR      (*((volatile unsigned char *) 0xE000C01C))
#define UART0_DLL      (*((volatile unsigned char *) 0xE000C000))
#define UART0_DLM      (*((volatile unsigned char *) 0xE000C004))

/* Universal Asynchronous Receiver Transmitter 1 (UART1) */
#define UART1_RBR      (*((volatile unsigned char *) 0xE0010000))
#define UART1_THR      (*((volatile unsigned char *) 0xE0010000))
#define UART1_IER      (*((volatile unsigned char *) 0xE0010004))
#define UART1_IIR      (*((volatile unsigned char *) 0xE0010008))
#define UART1_FCR      (*((volatile unsigned char *) 0xE0010008))
#define UART1_LCR      (*((volatile unsigned char *) 0xE001000C))
#define UART1_MCR      (*((volatile unsigned char *) 0xE0010010))
#define UART1_LSR      (*((volatile unsigned char *) 0xE0010014))
#define UART1_MSR      (*((volatile unsigned char *) 0xE0010018))
#define UART1_SCR      (*((volatile unsigned char *) 0xE001001C))
#define UART1_DLL      (*((volatile unsigned char *) 0xE0010000))
#define UART1_DLM      (*((volatile unsigned char *) 0xE0010004))

/* I2C Interface */
#define I2C_I2CONSET   (*((volatile unsigned char *) 0xE001C000))
#define I2C_I2STAT     (*((volatile unsigned char *) 0xE001C004))
#define I2C_I2DAT      (*((volatile unsigned char *) 0xE001C008))
#define I2C_I2ADR      (*((volatile unsigned char *) 0xE001C00C))
#define I2C_I2SCLH     (*((volatile unsigned short*) 0xE001C010))
#define I2C_I2SCLL     (*((volatile unsigned short*) 0xE001C014))
#define I2C_I2CONCLR   (*((volatile unsigned char *) 0xE001C018))

/* SPI (Serial Peripheral Interface) */
#define SPI_SPCR       (*((volatile unsigned char *) 0xE0020000))
#define SPI_SPSR       (*((volatile unsigned char *) 0xE0020004))
#define SPI_SPDR       (*((volatile unsigned char *) 0xE0020008))
#define SPI_SPCCR      (*((volatile unsigned char *) 0xE002000C))
#define SPI_SPTCR      (*((volatile unsigned char *) 0xE0020010))
#define SPI_SPTSR      (*((volatile unsigned char *) 0xE0020014))
#define SPI_SPTOR      (*((volatile unsigned char *) 0xE0020018))
#define SPI_SPINT      (*((volatile unsigned char *) 0xE002001C))

/* Real Time Clock */
#define RTC_ILR        (*((volatile unsigned char *) 0xE0024000))
#define RTC_CTC        (*((volatile unsigned short*) 0xE0024004))
#define RTC_CCR        (*((volatile unsigned char *) 0xE0024008))
#define RTC_CIIR       (*((volatile unsigned char *) 0xE002400C))
#define RTC_AMR        (*((volatile unsigned char *) 0xE0024010))
#define RTC_CTIME0     (*((volatile unsigned long *) 0xE0024014))
#define RTC_CTIME1     (*((volatile unsigned long *) 0xE0024018))
#define RTC_CTIME2     (*((volatile unsigned long *) 0xE002401C))
#define RTC_SEC        (*((volatile unsigned char *) 0xE0024020))
#define RTC_MIN        (*((volatile unsigned char *) 0xE0024024))
#define RTC_HOUR       (*((volatile unsigned char *) 0xE0024028))
#define RTC_DOM        (*((volatile unsigned char *) 0xE002402C))
#define RTC_DOW        (*((volatile unsigned char *) 0xE0024030))
#define RTC_DOY        (*((volatile unsigned short*) 0xE0024034))
#define RTC_MONTH      (*((volatile unsigned char *) 0xE0024038))
#define RTC_YEAR       (*((volatile unsigned short*) 0xE002403C))
#define RTC_ALSEC      (*((volatile unsigned char *) 0xE0024060))
#define RTC_ALMIN      (*((volatile unsigned char *) 0xE0024064))
#define RTC_ALHOUR     (*((volatile unsigned char *) 0xE0024068))
#define RTC_ALDOM      (*((volatile unsigned char *) 0xE002406C))
#define RTC_ALDOW      (*((volatile unsigned char *) 0xE0024070))
#define RTC_ALDOY      (*((volatile unsigned short*) 0xE0024074))
#define RTC_ALMON      (*((volatile unsigned char *) 0xE0024078))
#define RTC_ALYEAR     (*((volatile unsigned short*) 0xE002407C))
#define RTC_PREINT     (*((volatile unsigned short*) 0xE0024080))
#define RTC_PREFRAC    (*((volatile unsigned short*) 0xE0024084))

/* Watchdog */
#define WDMOD          (*((volatile unsigned char *) 0xE0000000))
#define WDTC           (*((volatile unsigned long *) 0xE0000004))
#define WDFEED         (*((volatile unsigned char *) 0xE0000008))
#define WDTV           (*((volatile unsigned long *) 0xE000000C))

#endif  // __LPC210x_H
