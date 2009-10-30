#ifndef INC_POSIXSERIAL_H
#define INC_POSIXSERIAL_H

#include <termios.h>
#include "Serial.h"

namespace metro {
	/**
	 * POSIX-specific Serial communication device.
	 */
	class PosixSerial : public Serial {
		public:
			PosixSerial();
			~PosixSerial();

			int open(const char *device, bool fBlockIO);
			int close();
			int read(byte *buffer, size_t nBytes);
			int write(const byte *buffer, size_t nBytes);

		protected:
			int _fd; // file descriptor of the serial device
			struct termios _attr; // serial device attributes
	};
}

#endif
