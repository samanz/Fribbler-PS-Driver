#ifndef METROBOTICS_TIMER_H
#define METROBOTICS_TIMER_H

namespace metrobotics
{
	/**
	 * \class   Timer
	 *
	 * \brief   An abstract class for a general-purpose high resolution timer.
	 *
	 * \details The purpose of this abstract class is to provide an interface that allows the user to
	 *          count intervals of time using floating point values to represent the length of the
	 *          interval in seconds. Furthermore, the goal is to be able to represent this interval
	 *          with a precision that is greater than just one second. However, because this is an
	 *          abstract class, the precision of the timer will depend on the specific implementation.
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	class Timer
	{
		public:
			/**
			 * \brief   Start the timer.
			 *
			 * \details Sets (or resets) the reference point to the current time.
			 */
			virtual void start() = 0;

			/**
			 * \brief   Time that has elapsed since the last start (or reset) of the timer.
			 *
			 * \details Returns the time (in seconds) that has elapsed from the reference point.
			 *
			 * \return  time in seconds since the last start (or reset) of the timer
			 */
			virtual double elapsed() const = 0;

		protected:
			/**
			 * \brief   Reference point of timer.
			 *
			 * \details Represents the time in seconds as a floating point number.
			 */
			double _ref;
	};
}

#endif
