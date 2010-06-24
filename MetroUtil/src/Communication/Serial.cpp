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
using namespace metrobotics;

#include <string>
using namespace std;

void Serial::flush()
{
	this->flushOutput();
	this->flushInput();
}

string Serial::getLine(char delimiter)
{
	string ret;
	for (int i = 0; (ret += this->getByte())[i] != delimiter; ++i);
	return ret;
}
