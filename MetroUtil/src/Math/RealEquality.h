#ifndef METROBOTICS_REALEQUALITY_H
#define METROBOTICS_REALEQUALITY_H

#include "RealPredicate.h"

namespace metrobotics
{
	/**
	 * \class   RealEquality
	 *
	 * \brief   A function object that compares two real numbers using the equality relation.
	 *
	 * \details This comparison takes into account the limited precision of floating point values by
	 *          using an arbitrarily chosen acceptable margin of error.
	 *
	 * \remark  Given two floating point numbers <b>a</b> and <b>b</b>, and an acceptable margin of
	 *          error <b>e</b>, then <b>a == b if and only if |a - b| <= e</b>.
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	class RealEquality : public RealPredicate
	{
		public:
			/**
			 * \brief   Construct a new predicate for comparing two real numbers using the equality
			 *          relation.
			 *
			 * \details Optionally provide an acceptable margin of error to be used in comparisons;
			 *          if no acceptable margin of error is specified then it is assumed that there
			 *          is no acceptable margin of error, that is that the margin of error is zero,
			 *          which is no different than using the built-in comparison for floating point
			 *          values.
			 *
			 * \arg     epsilon is the acceptable margin of error
			 */
			RealEquality(double epsilon = 0.0);

			/**
			 * \brief   Function call operator.
			 *
			 * \details Treats the predicate as a function that takes two real numbers and returns
			 *          the result of their comparison. For example, given two real numbers <b>a</b>
			 *          and <b>b</b> and \c RealEquality <b>f(e)</b> where <b>e</b> is the
			 *          acceptable margin of error, then <b>f(a, b) == [fabs(a - b) <= e]</b>.
			 */
			bool operator()(const double& lhs, const double& rhs) const;
	};
}

#endif
