#include "PosixSerial.h"
#include "scribbler.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

// Globals for debugging/logging.
int gDebugging = 1;

int main()
{
	// hold the response somewhere
	char buffer[512];

	PosixSerial ps("/dev/scribbler9");
	Scribbler scribbler(ps);

	cerr << "drive: " << scribbler.drive(100,100,1) << endl;
	cerr << "beep: " << scribbler.beep(1,5) << endl;
	cerr << "led: " << scribbler.setScribblerAllLeds(1) << endl;



	return 0;
}
