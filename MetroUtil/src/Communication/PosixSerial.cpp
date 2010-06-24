/************************************************************************/
/* This was developed by John Cummins at Brooklyn College under the     */
/* supervision of Professor Sklar.                                      */
/*                                                                      */
/* It was adopted into the MetroUtil library, and is currently          */
/* maintained by Mark Manashirov from Brooklyn College and the entire   */
/* MetroBotics team at CUNY.                                            */
/*                                                                      */
/* It is released under the copyleft understanding. That is, any one is */
/* free to use, and modify, any part of it so long as it continues to   */
/* carry this notice.                                                   */
/************************************************************************/

#include "PosixSerial.h"
#include "Timer/PosixTimer.h"
using namespace metrobotics;

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
using namespace std;

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

// Debugging is off by default.
static bool fDebugPosixSerial = false;
static void __dbg(const string& msg)
{
	if (fDebugPosixSerial) {
		cerr << msg << endl;
	}
}
void PosixSerial::debugging(bool flag)
{
	fDebugPosixSerial = flag;
}

// Internal signal handler for any I/O events on the device.
static void __sighandler(int sig)
{
	// Not sure if this can be used for something useful.
}

// A persistent write function.
static ssize_t write_r(int fd, const void *buf, size_t count, size_t ms)
{
	double tMax = ms / 1000.0; // convert to seconds
	PosixTimer t;
	int r;
	unsigned int written = 0;
	while (written < count) {
		r = write(fd, ((const unsigned char *)buf) + written, count - written);
		// did something seriously go wrong?
		if (r < 0 && errno != EINTR) {
			__dbg(string("PosixSerial: failed to write: ") + string(strerror(errno)));
			throw PosixSerial::WriteFailure();
		} else if (r > 0) {
			// progress!
			written += r;
			t.start(); // restart the timer
		}
		// are we out of time?
		if (ms > 0 && t.elapsed() > tMax) {
			__dbg(string("PosixSerial: write timed out"));
			throw PosixSerial::WriteTimeout();
		}
	}
	return written;
}

// A persistent read function.
static ssize_t read_r(int fd, void *buf, size_t count, size_t ms)
{
	double tMax = ms / 1000.0; // convert to seconds
	PosixTimer t;
	int r;
	unsigned int total = 0;
	while (total < count) {
		r = read(fd, ((unsigned char *)buf) + total, count - total);
		// did something seriously go wrong?
		if (r < 0 && errno != EINTR) {
			__dbg(string("PosixSerial: failed to read: ") + string(strerror(errno)));
			throw PosixSerial::ReadFailure();
		} else if (r > 0) {
			// progress!
			total += r;
			t.start(); // restart the timer
		}
		// are we out of time?
		if (ms > 0 && t.elapsed() > tMax) {
			__dbg(string("PosixSerial: read timed out"));
			throw PosixSerial::ReadTimeout();
		}
	}
	return total;
}

PosixSerial::PosixSerial(const char *devName, unsigned int baudRate)
:mFunctional(false),
 mDevFD(-1),
 mLineSpeed(baudRate)
{
	if (devName == 0) {
		__dbg(string("PosixSerial: device name cannot be null"));
		throw InvalidDeviceName();
	} else {
		mDevName = devName;
	}
	connect();
}

PosixSerial::~PosixSerial()
{
	if (mFunctional && mDevFD > 0) {
		close(mDevFD);
	}
}

void PosixSerial::connect()
{
	if (!mFunctional) {
		// Intercept I/O events on the port.
		struct sigaction sa;
		if (sigaction(SIGIO, 0, &sa) < 0) {
			__dbg(string("PosixSerial: failed to retrieve sigaction"));
			throw ConnectionFailure();
		}
		sa.sa_handler = __sighandler;
		if (sigaction(SIGIO, &sa, 0) < 0) {
			__dbg(string("PosixSerial: failed to override sigaction"));
			throw ConnectionFailure();
		}

		// Open the port.
		if ((mDevFD = open(mDevName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
			__dbg(string("PosixSerial: failed to open port ") + mDevName);
			throw ConnectionFailure();
		}

		// Redirect all signals for the port to the current process.
		if (fcntl(mDevFD, F_SETOWN, getpid()) < 0) {
			__dbg(string("PosixSerial: failed to redirect signals: ") + string(strerror(errno)));
			throw ConnectionFailure();
		}

		// Enable asynchronous I/O for the port.
		if (fcntl(mDevFD, F_SETFL, FASYNC) < 0) {
			__dbg(string("PosixSerial: failed to enable asynchronous I/O: ") + string(strerror(errno)));
			throw ConnectionFailure();
		}

		// Get the port's current attributes.
		struct termios attr;
		memset((void *)&attr, 0, sizeof(attr));
		if (tcgetattr(mDevFD, &attr) < 0 ) {
			__dbg(string("PosixSerial: failed to acquire port attributes: ") + string(strerror(errno)));
			throw ConnectionFailure();
		}

		// Set our attributes.
		// TODO: attributes should probably be parameterized; just assume "raw" mode for now.
		cfmakeraw(&attr);
		cfsetspeed(&attr, mLineSpeed);
		attr.c_cc[VTIME] = 0; // timeout in tenths of a second
		attr.c_cc[VMIN] = 0;  // allow read to return 0
		if (tcsetattr(mDevFD, TCSAFLUSH, &attr) < 0) {
			__dbg(string("PosixSerial: failed set port attributes: ") + string(strerror(errno)));
			throw ConnectionFailure();
		}

		mFunctional = true;
	}
}

void PosixSerial::putByte(const unsigned char b)
{
	putBlock(&b, 1);
}

void PosixSerial::putBlock(const unsigned char *buf, unsigned long nBytes)
{
	if (buf == 0) {
		throw NullPointer();
	}
	if (!mFunctional) {
		connect();
	}
	write_r(mDevFD, buf, nBytes, mTimeOut);
}

unsigned char PosixSerial::getByte()
{
	unsigned char b;
	getBlock(&b, 1);
	return b;
}

void PosixSerial::getBlock(unsigned char *buf, unsigned long nBytes)
{
	if (buf == 0) {
		throw NullPointer();
	}
	if (!mFunctional) {
		connect();
	}
	read_r(mDevFD, buf, nBytes, mTimeOut);
}

void PosixSerial::flushOutput()
{
	if (!mFunctional) {
		connect();
	}
	if (tcflush(mDevFD, TCOFLUSH) < 0) {
		__dbg(string("PosixSerial: failed to flush output: ") + string(strerror(errno)));
	}
}

void PosixSerial::flushInput()
{
	if (!mFunctional) {
		connect();
	}
	if (tcflush(mDevFD, TCIFLUSH) < 0) {
		__dbg(string("PosixSerial: failed to flush input: ") + string(strerror(errno)));
	}
}

void PosixSerial::timeout(unsigned int ms)
{
	if (!mFunctional) {
		connect();
	}
	mTimeOut = ms;
}
