#include "PosixSerial.h"
#include "scribbler.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
using namespace std;

// Globals for debugging/logging.
int gDebugging = 1;

int rgb2yuv(int R, int G, int B, int &Y,
											int &U,
											int &V) {
	double tY = 0.299 * R + 0.587 * G + 0.114 * B;
	double tU = -0.14713 * R - 0.28886 * G + 0.436 * B + 128;
	double tV = 0.615 * R - 0.51499 * G - 0.10001 * B + 128;

	tY = std::max(std::min(tY, 255.0), 0.0);
	tU = std::max(std::min(tU, 255.0), 0.0);
	tV = std::max(std::min(tV, 255.0), 0.0);

	Y = (int)tY;
	U = (int)tU;
	V = (int)tV;
	return 0;
}
unsigned char * getBlobImage(unsigned char * data_buffer, int size) {
	int width = 256;
	int height = 192;

	unsigned char * blobs 
		= (unsigned char *)malloc(sizeof(unsigned char) * (width+1) 
				* (height + 1));
	unsigned char * rgb_blob
		= (unsigned char *)malloc(sizeof(unsigned char) * width * height * 3);
	
	int inside = 0;
	int counter = 0;
	int val = 128;
	int px = 0;

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j+=4) {
			if(counter < 1 && px < size) {
				counter = (int)data_buffer[px];
				px += 1;
				counter = (counter << 8) | data_buffer[px];
				px += 1;
				//printf("Counter %i\n", counter);
				if(inside) {
					val = 0;
					inside = 0;
				}
				else {
					val = 255;
					inside = 1;
				}
			}
			for(int k = 0; k < 4; k++) {
				blobs[(i * width) + j + k] = val;
			}
			counter -=  1;
		}
	}

	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			for(int k = 0; k < 3; k++)
				rgb_blob[(i * width) * 3 + (j * 3) + k] 
					= blobs[(i * width) + j];
		}
	}
	free(blobs);
	return rgb_blob;	
}

int main(int argc, char* argv[]) 
{
	string name = "/dev/scribbler9";
	// hold the response somewhere
	char buffer[512];
	unsigned char * picture;
	if(argc>1) name = argv[1];
	PosixSerial ps(name.c_str());
	Scribbler scribbler(ps);

	if (scribbler.getScribblerName(buffer) == 0) {
		cerr << "failed to get name" << endl;
	} else {
		cout << "Name:" << buffer << endl;
	}
	/*
	enum { STRAIGHT, TURNING } state;
	state = STRAIGHT;
	int stateChanges = 0;
	
	while(stateChanges < 8) {
		if(state == STRAIGHT) {
			cerr << "drive: " << scribbler.drive(100,93,300) << endl;
			state = TURNING;
		} else {
			cerr << "turn: " << scribbler.drive(-65,65,55) << endl;
			state = STRAIGHT;
		}
		usleep(5000);
		stateChanges++;	
	} */
	int R,G,B;
	int y,u,v;
	R = 255; G = 0; B = 92;
	rgb2yuv(R,G,B, y,u,v);
	cout << "YUV: " << y << " " << u << " " << v << endl;
	YUVRange cRange;
	cRange.setY((char)0, (char)255);
	cRange.setU((char)114, (char)137);
	cRange.setV((char)175, (char)253);
	cout << cRange.say() << endl;
	scribbler.setBitmapParams(90, 2, cRange);
	time_t secs = time(NULL);
	Data * rle = scribbler.getCompressedBitmap();
	cout << "Size: " << rle->getDataSize() << " Time taken: " << (time(NULL)-secs) << endl; 
	ofstream myfile ("bandw.raw");
	myfile.write((char *)getBlobImage((unsigned char *)rle->getData(), rle->getDataSize()), 256*192*3);	
  	myfile.close();
	RLE theRLE(*rle);
	FILE * output;
	output = fopen("rletalk.txt", "w");
	theRLE.talk(output);
	fclose (output);
	return 0;
}