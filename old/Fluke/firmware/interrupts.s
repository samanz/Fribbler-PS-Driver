/******************************************************************************
 *
 * interrupt utilities
 *
 *   http://osdir.com/ml/gcc.cross-compiling.arm/2007-01/msg00042.html
 *
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

	
.text
.code 32
.align  0
.global interrupts_get_and_disable
.global interrupts_enable
.global fiq_get_and_disable
.global fiq_enable


/* int interrupts_get_and_disable
  Disables interrupts.
  Returns zero if interupts were previously off */

interrupts_get_and_disable:
	                mrs     r0, cpsr
	                ands    r0, r0, #0x80
	                movne   r0, #0
	                bxne    lr
	                mrs     r0, cpsr
	                orr     r0,r0,#0x80
	                msr     cpsr_c,r0
	                mov     r0,#1
	                bx      lr

/* void interrupts_enable */
interrupts_enable:
	                mrs     r0, cpsr
	                bic     r0,r0,#0x80
	                msr     cpsr_c,r0
	                bx      lr


/* int fiq_get_and_disable
 Disables interrupts.
 Returns zero if interupts were previously off*/

fiq_get_and_disable:
	                mrs     r0, cpsr
	                ands    r0, r0, #0x40
	                movne   r0, #0
	                bxne    lr
	                mrs     r0, cpsr
	                orr     r0,r0,#0x80
	                msr     cpsr_c,r0
	                mov     r0,#1
	                bx      lr

/*@ void fiq_enable */
fiq_enable:
	                mrs     r0, cpsr
	                bic     r0,r0,#0x40
	                msr     cpsr_c,r0
	                bx      lr

	
