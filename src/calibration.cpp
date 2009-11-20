#include "PosixSerial.h"
#include "scribbler.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

// Globals for debugging/logging.
int gDebugging = 1;

int main(int argc, char* argv[]) 
{
	string name = "/dev/scribbler15";
	// hold the response somewhere
	char buffer[512];

	if(argc>1) name = argv[1];
	PosixSerial ps(name.c_str());
	Scribbler scribbler(ps);

	if (scribbler.getScribblerName(buffer) == 0) {
		cerr << "failed to get name" << endl;
	} else {
		cout << "Name:" << buffer << endl;
	}
	
	scribbler.drive(-60,60);

	return 0;
}
