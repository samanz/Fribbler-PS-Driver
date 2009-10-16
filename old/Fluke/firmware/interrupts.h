 /*****************************************************************************
 *
 * Interrupts Utilities Header 
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#ifndef __INTERRUPTS__
#define __INTERRUPTS__

int interrupts_get_and_disable();
void interrupts_enable();
int fiq_get_and_disable();
void fiq_enable();
#endif


