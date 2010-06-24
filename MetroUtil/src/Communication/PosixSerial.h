#ifndef METROBOTICS_POSIXSERIAL_H
#define METROBOTICS_POSIXSERIAL_H
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

#include "Serial.h"
#include <string>
#include <termios.h> // needed for baud rate

namespace metrobotics
{

	/**
	 * \brief   POSIX implementation of the Serial interface
	 * \details A class of objects that simulate serial communication in a
	 *          Posix-compliant environment.
	 * \author  John Cummins
	 * \author  Mark Manashirov
	 */
	class PosixSerial : public Serial
	{
		public:
			PosixSerial(const char *devName, unsigned int baudRate);
			~PosixSerial();

			// [Implement input capabilities.]
			void flushInput();
			unsigned char getByte();
			void getBlock(unsigned char *buf, unsigned long nBytes);


			// [Implement output capabilities.]
			void flushOutput();
			void putByte(const unsigned char);
			void putBlock(const unsigned char *buf, unsigned long nBytes);

			// [Class-specific capabailites.]

			/**
			 * \brief   Toggle class-wide debugging.
			 */
			static void debugging(bool);

			/**
			 * \brief   Set a timeout (in milliseconds) for all I/O operations.
			 * \details A timeout of 0 (the default) may block I/O indefinitely.
			 */
			void timeout(unsigned int ms = 0);


		private:
			// Disable copying and assignment for PosixSerial objects.
			PosixSerial(const PosixSerial&);
			PosixSerial& operator=(const PosixSerial&);

			// Establish (or re-establish) a connection.
			void connect();

			// Internal state members.
			bool mFunctional;
			std::string mDevName;
			int mDevFD; // Posix file descriptor corresponding to the serial device
			unsigned int mLineSpeed; // Baud rate
			unsigned int mTimeOut; // in milliseconds
	};

}

#endif
