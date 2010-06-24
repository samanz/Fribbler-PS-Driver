#ifndef METROBOTICS_POSIX_TIMER_H
#define METROBOTICS_POSIX_TIMER_H

#include "Timer.h"

namespace metrobotics
{
	/**
	 * \class   PosixTimer
	 *
	 * \brief   POSIX implementation of the Timer interface.
	 *
	 * \details This implementation of the high resolution timer is able to count one-second
	 *          intervals of time precise to the microsecond.
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	class PosixTimer : public Timer
	{
		public:
			/**
			 * \brief   Default constructor.
			 *
			 * \details Initializes the reference point to the time of object creation.
			 */
			PosixTimer();

			// [Implement/override the interface that we inherited from Timer.]
			void start();
			double elapsed() const;
	};
}

#endif
