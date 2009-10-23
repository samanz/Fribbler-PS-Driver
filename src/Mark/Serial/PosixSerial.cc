// Standard C++ Library
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

// Unix System Library
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Interface
#include "PosixSerial.h"

using namespace std;
using namespace metro;

/**
 * The constructor just initializes all members to a 'default' state (usually zero).
 */
PosixSerial::PosixSerial()
{
	_device = 0; // null
	_fd = -1;    // 0 is a valid file descriptor!
	::memset((void *)&_attr, sizeof(_attr), 0);
}

/**
 * Release any resources that may have been allocated.
 */
PosixSerial::~PosixSerial()
{
	if (_device) {
		delete _device;
		_device = 0;
	}

	if (_fd != -1) {
		PosixSerial::close();
	}
}

/**
 * Open a connection to a serial communication device.
 */
int PosixSerial::open(const char *device, bool fBlockIO)
{
	// make sure that there are no open connections
	if (_fd != -1) {
		cerr << "failed to open serial device: " << device << "\n"
		     << "\terrno: " << "a device is already opened: " << _device << endl;
		return -1;
	}

	// attempt to open the serial device file
	_fd = ::open(device, O_RDWR | O_NOCTTY | (fBlockIO ? 0 : O_NDELAY));
	if (_fd == -1) {
		cerr << "failed to open serial device: " << device << "\n"
             << "\terrno: " << ::strerror(errno) << endl;
		return -1;
	}

	// keep a copy of the device filename for internal purposes
	size_t len = ::strlen(device) + 1;
	_device = new char[len];
	::strncpy(_device, device, len);
	_device[len] = '\0';

	// get serial device's attributes
	if (::tcgetattr(_fd, &_attr) < 0) {
		cerr << "failed to retrieve serial device attributes" << "\n"
		     << "\terrno: " << ::strerror(errno) << endl;
		PosixSerial::close();
		return -1;
	}

	// configure the serial device for raw I/O
	_attr.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	_attr.c_oflag &= ~OPOST;
	// configure blocking I/O
	_attr.c_cc[VMIN] = fBlockIO ? 1 : 0;
	_attr.c_cc[VTIME] = 0;
	// apply the configuration settings
	if (::tcsetattr(_fd, TCSANOW, &_attr) < 0) {
		cerr << "failed to configure serial device attributes" << "\n"
		     << "\terrno: " << ::strerror(errno) << endl;
		PosixSerial::close();
		return -1;
	} else {
		// get the serial device's newly set attributes
		if (::tcgetattr(_fd, &_attr) < 0) {
			cerr << "failed to retrieve serial device attributes" << "\n"
				 << "\terrno: " << ::strerror(errno) << endl;
			PosixSerial::close();
			return -1;
		}
		// make sure the attributes we requested were actually set properly!
		/* FIXME: this needs work!
		if ((_attr.c_oflag & ICANON) ||
		    (_attr.c_lflag & ECHO)   ||
		    (_attr.c_lflag & ECHOE)  ||
		    (_attr.c_lflag & ISIG)   ||
		    (_attr.c_oflag & OPOST)){
			cerr << "failed to configure serial device attributes" << "\n"
				 << "\terrno: " << ::strerror(errno) << endl;
			PosixSerial::close();
			return -1;
		} */
	}

	return 0;
}

/**
 * Close an existing connection.
 */
int PosixSerial::close()
{
	if (_device) {
		delete _device;
	}

	if (_fd != -1) {
		::close(_fd);
		_fd = -1;
	}

	return 0;
}

int PosixSerial::read(byte *buffer, size_t nBytes)
{
	size_t total = 0; // total number of bytes read
	ssize_t r = 0;    // temporary number of bytes read
	if (_fd == -1) {
		cerr << "failed to read: connection closed" << endl;
		return -1;
	} else if (buffer && nBytes > 0) {
		while ((total < nBytes) && // main condition
				(((r = ::read(_fd, buffer + total, nBytes - total)) > 0) || // try to read
		          (r == -1 && (errno == EAGAIN || errno == EINTR)))) { // don't stop for temporary errors
			total += r;
		}
		if (r == -1) {
			cerr << "read failure: " << ::strerror(errno) << endl;
			return -1;
		}
	}
	return total;
}

int PosixSerial::write(const byte *buffer, size_t nBytes)
{
	int r = 0; // temporary return status from system call
	if (_fd == -1) {
		cerr << "failed to write: connection closed" << endl;
		return -1;
	} else if (buffer && nBytes > 0) {
		r = ::write(_fd, buffer, nBytes);
	}
	return r;
}
