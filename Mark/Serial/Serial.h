#ifndef INC_SERIAL_H
#define INC_SERIAL_H

namespace metro {
	typedef unsigned char byte;

	/**
	 * Abstract class for a Serial communication device.
	 */
	class Serial {
		public:
			virtual int open(const char *device, bool fBlockIO) = 0;
			virtual int close() = 0;
			virtual int read(byte *buffer, size_t nBytes) = 0;
			virtual int write(const byte *buffer, size_t nBytes) = 0;

		protected:
			char *_device; // the serial device file
	};
}

#endif
