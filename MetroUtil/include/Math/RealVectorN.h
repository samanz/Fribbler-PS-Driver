#ifndef METROBOTICS_REALVECTORN_H
#define METROBOTICS_REALVECTORN_H

#include "VectorN.h"
#include "RealEquality.h"

namespace metrobotics
{
	/**
	 * \class   RealVectorN
	 *
	 * \brief   A generic vector implementation over the real numbers.
	 *
	 * \details This class is capable of representing vectors of any dimension (size) over 
	 *          the real numbers, that is values represented by type \c double.
	 *
	 * \tparam  N is an unsigned integral value that represents the dimension (size) of the vector
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	template <size_t N>
	class RealVectorN : public VectorN<double, N>
	{
		public:
			/**
			 * \brief   Set/change the acceptable margin of error.
			 *
			 * \details The acceptable margin of error is used in relational operations involving
			 *          vectors. For example, when determining whether two vectors are equal to each
			 *          other, the acceptable margin of error will determine the maximum variance
			 *          of equal numbers and the minimum variance of unequal numbers. See \ref
			 *          equality for more details.
			 *
			 * \arg     epsilon is the acceptable margin of error
			 *
			 * \remarks The acceptable margin of error applies to the entire vector space, that is
			 *          to all vectors of the same dimension. The acceptable margin of error is
			 *          initialized to zero by default.
			 */
			static void marginOfError(double epsilon)
			{
				_equal_to.marginOfError(epsilon);
			}

			/**
			 * \brief   Get the acceptable margin of error.
			 *
			 * \returns the acceptable margin of error
			 */
			static double marginOfError()
			{
				return _equal_to.marginOfError();
			}

			/**
			 * \brief   Construct a new empty vector.
			 *
			 * \details All new vectors are initialized to zero by default.
			 */
			RealVectorN()
			{
				// [Initialize new vectors to the zero vector.]
				for (typename RealVectorN<N>::size_type i = 0; i < N; ++i) {
					(*this)[i] = 0.0;
				}
			}

			/**
			 * \brief   Copy constructor.
			 */
			RealVectorN(const RealVectorN& v)
			{
				// [Delegate work to the assignment operator.]
				*this = v;
			}

			/**
			 * \brief   Copy constructor for \em upcasting from the base class.
			 */
			RealVectorN(const VectorN<double, N>& v)
			{
				// [Copy the vector entry by entry.]
				for (typename RealVectorN<N>::size_type i = 0; i < N; ++i) {
					(*this)[i] = v[i];
				}
			}

			/**
			 * \brief   Assignment operator.
			 */
			RealVectorN& operator=(const RealVectorN& v)
			{
				if (this != &v) {
					// [Copy the vector entry by entry.]
					for (typename RealVectorN<N>::size_type i = 0; i < N; ++i) {
						(*this)[i] = v[i];
					}
				}
				return *this;
			}

		protected:
			//! @cond INTERNAL
			// Determine whether two vectors are equivalent.
			virtual bool _equals(const VectorN<double, N>& v) const
			{
				// [Check each entry one at a time.]
				for (typename RealVectorN<N>::size_type i = 0; i < N; ++i) {
					if (!(_equal_to((*this)[i], v[i]))) {
						return false;
					}
				}
				return true;
			}
			//! @endcond

		private:
			//! @cond INTERNAL
			// A binary predicate that is used to determine whether two real numbers are equivalent.
			// This binary predicate will be used for the entire vector space, that is for all
			// vectors of the same dimension.
			static RealEquality _equal_to;
			//! @endcond
	};

	//! @cond INTERNAL
	// Default-initialize the binary predicate.
	template <size_t N>
	RealEquality RealVectorN<N>::_equal_to = RealEquality();
	//! @endcond

	// [Handy type definitions.]
	/**
	 * \brief   A two-dimensional vector over the real numbers.
	 */
	typedef RealVectorN<2> RealVector2;

	/**
	 * \brief   A three-dimensional vector over the real numbers.
	 */
	typedef RealVectorN<3> RealVector3;
}

#endif
