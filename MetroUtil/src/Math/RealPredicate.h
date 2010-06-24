#ifndef METROBOTICS_REALPREDICATE_H
#define METROBOTICS_REALPREDICATE_H

#include <functional>

namespace metrobotics
{
	/**
	 * \class   RealPredicate
	 *
	 * \brief   A function object that compares two real numbers using some relation.
	 *
	 * \details This comparison takes into account the limited precision of floating point values by
	 *          using an arbitrarily chosen acceptable margin of error.
	 *
	 * \note    This is just an abstract class that doesn't correspond to any actual relation.
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	class RealPredicate : public std::binary_function<double, double, bool>
	{
		public:
			/**
			 * \brief   Construct a new predicate for comparing two real numbers.
			 *
			 * \details Optionally provide an acceptable margin of error to be used in comparisons;
			 *          if no acceptable margin of error is specified then it is assumed that there
			 *          is no acceptable margin of error, that is that the margin of error is zero,
			 *          which is no different than using the built-in comparison for floating point
			 *          values.
			 *
			 * \arg     epsilon is the acceptable margin of error
			 */
			RealPredicate(double epsilon = 0.0);

			/**
			 * \brief   Set/change the acceptable margin of error.
			 *
			 * \arg     epsilon is the acceptable margin of error
			 */
			void marginOfError(double epsilon);

			/**
			 * \brief   Get the acceptable margin of error.
			 *
			 * \returns the acceptable margin of error
			 */
			double marginOfError() const;

			/**
			 * \brief   Function call operator.
			 *
			 * \details Treats the predicate as a binary function that takes two real numbers and
			 *          returns the result of their comparison, whatever it may be, and taking into
			 *          account the acceptable margin of error.
			 */
			virtual bool operator()(const double& lhs, const double& rhs) const = 0;

		private:
			/**
			 * \brief   The acceptable margin of error.
			 */
			double _epsilon;
	};
}

#endif
