#include "PosixTimer.h"
using namespace metrobotics;

#include <sys/time.h>

/**
 * Convert POSIX timeval to a double that represents the corresponding time in seconds.
 */
static inline double tvToDouble(const struct timeval &tv)
{
	return tv.tv_sec + (tv.tv_usec / 1000000.0);
}

PosixTimer::PosixTimer()
{
	// Initialize the reference point to the current time.
	start();
}

void PosixTimer::start()
{
	// Read the current time.
	struct timeval tv;
	gettimeofday(&tv, 0);
	// Reset the reference point.
	_ref = tvToDouble(tv);
}

double PosixTimer::elapsed() const
{
	// Read the current time.
	struct timeval tv;
	gettimeofday(&tv, 0);
	// Compute the difference from the reference point.
	return tvToDouble(tv) - _ref;
}
