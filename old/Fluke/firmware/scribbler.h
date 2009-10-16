/*******************************************************************************
 *
 * Scribbler programming interface
 * 
 * keith.ohara@gatech.edu
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/

#ifndef __SCRIBBLER__
#define __SCRIBBLER__

#include "fluke.h"

#define MAX_PROG_SIZE 2000
#define INIT_PROG_SIZE 1728
//#define PROG_SIZE 36

void reset_scribbler();
int program_scribbler(unsigned char* buffer, int size);



#endif /* __SCRIBBLER__ */


