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
	string name = "/dev/scribbler9";
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
	
	cerr << "drive: " << scribbler.drive(100,100,200) << endl;
	cerr << "turn: " << scribbler.drive(-100,100,18) << endl;
	cerr << "Take picture: " << endl;
	
	ofstream picFile("test.jpg");
	Data * pic = scribbler.takePhotoJPEG();
	picFile.write(pic->getData(), pic->getDataSize() );
	cout << "Size in bytes: " << pic->getDataSize() << endl;
	picFile.close();
	
	cerr << "drive: " << scribbler.drive(100,100,200) << endl;
	cerr << "turn: " << scribbler.drive(-100,100,18) << endl;
	
	cerr << "drive: " << scribbler.drive(100,100,200) << endl;
	cerr << "turn: " << scribbler.drive(-100,100,18) << endl;
	
	cerr << "drive: " << scribbler.drive(100,100,200) << endl;	
	

	return 0;
}