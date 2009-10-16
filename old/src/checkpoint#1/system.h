
/****************************************************************/
/* A system.h file						*/
/*								*/
/* system.h files specify the environment for the software	*/
/* generally the OS						*/
/*								*/
/* This one is for :-						*/
/* 	*nix Operating System					*/
/*	         						*/
/* we specify ADDRESS as NULL so the address of the Physical	*/
/* Robot is found in the environment variable ROBOT_ADDRESS	*/
/****************************************************************/
#include "PosixSerial.h"
#include "SocSerial.h"
#include "PosixFile.h"
#include <unistd.h>



#define ADDRESS		NULL

