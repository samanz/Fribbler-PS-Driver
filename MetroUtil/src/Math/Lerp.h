#ifndef METROBOTICS_LERP_H
#define METROBOTICS_LERP_H

#include <map>
#include <algorithm>
#include <stdexcept>

#include "RealVectorN.h"
#include "RealEquality.h"
#include "RealLessThan.h"

namespace metrobotics
{
	/**
	 * \class   Lerp
	 *
	 * \brief   A linear interpolation class for any N-dimensional vector space over the reals.
	 *
	 * \details The purpose of this class is to provide an interface that allows the user to
	 *          interpolate an N-dimensional point (represented by an N-dimensional vector) given
	 *          one of its components (entries) such that there exists a pair of bounding data
	 *          points in the set of all recorded data points. Put simply, this class can be used to
	 *          interpolate (i.e. "fill in") the empty points on a graph by using the set of points
	 *          that are already known together with one component (key) from those unknown points.
	 *
	 * \anchor  dimension
	 * \tparam  dimension is the \em dimension of the vector space that we're working with; all
	 *          points (vectors) in the set must be of the same dimension
	 *
	 * \anchor  position
	 * \tparam  position is the \em position of the key (component) within the vector that is used
	 *          for interpolation; a value of zero (the default value) corresponds to the first
	 *          entry in the vector
	 *
	 * \author  Mark Manashirov <mark.manashirov@gmail.com>
	 */
	template <size_t dimension, size_t position = 0>
	class Lerp
	{
		public:
			// [Conform to the C++ Standard Template Library type definition syntax.]
			/**
			 * \brief   The type of the individual entries (components) in the vector.
			 *
			 * \details This type represents the elements of the field that lie under our vector
			 *          space; because our field is the set of real numbers, this type is equivalent
			 *          to the built-in type \c double.
			 */
			typedef typename RealVectorN<dimension>::value_type key_type;

			/**
			 * \brief   The function object that compares two keys (components) for ordering.
			 */
			typedef RealLessThan key_compare;

			/**
			 * \brief   The function object that compares two keys (components) for equality.
			 */
			typedef RealEquality key_equality;

			/**
			 * \brief   The type of the elements (vectors) in the vector space.
			 *
			 * \details This type represents the actual individual vectors of the N-dimensional
			 *          vector space over the real numbers.
			 */
			typedef RealVectorN<dimension>  value_type;
			typedef RealVectorN<dimension>* pointer;
			typedef RealVectorN<dimension>& reference;
			typedef const RealVectorN<dimension>& const_reference;

			/**
			 * \brief   An unsigned integral type.
			 *
			 * \details A type that is capable of representing the number of data points that
			 *          are presently in the set as well as the maximum number of data points
			 *          that the set may contain.
			 */
			typedef unsigned long size_type;

			/**
			 * \brief   A signed integral type.
			 */
			typedef signed long difference_type;

			/**
			 * \brief   Construct a linear interpolation object with no data points (empty).
			 *
			 * \details The linear interpolation object behaves like a container: it keeps a record
			 *          of all inserted data points (vectors). Once all of the data points have been
			 *          recorded, the process of interpolation is done automatically using the
			 *          information that is already stored in the object itself.
			 *
			 * \warning Because this linear interpolation class operates on an N-dimensional vector
			 *          space over the real numbers it takes into account the limited precision that
			 *          is inherent to representing real numbers on a computer. To that end, at
			 *          object creation time, the value of \ref RealVectorN<N>::marginOfError() will
			 *          determine the acceptable margin of error for \em all of the operations on
			 *          \em that object. Once the object has been created, subsequent changes to
			 *          \ref RealVectorN<N>::marginOfError() will have no effect on pre-existing
			 *          linear interpolation objects. That said, make sure to set \ref
			 *          RealVectorN<N>::marginOfError() accordingly \em before creating a new linear
			 *          interpolation object.
			 */
			Lerp():
			_eq(value_type::marginOfError()),
			_lt(value_type::marginOfError()),
			_data(key_compare(value_type::marginOfError()))
			{
			}

			/**
			 * \brief   Destructor.
			 */
			virtual ~Lerp()
			{
			}

			/**
			 * \brief   Insert a new data point (vector) into the set.
			 *
			 * \arg     vec is the N-dimensional vector that represents the N-dimensional point
			 *          to be added to the set of data points
			 *
			 * \anchor  unique
			 * \warning Inserting the same data point twice has no effect, but the entry at \ref
			 *          position must be \ref unique so that for each possible value in that entry
			 *          there can be only \em one correspending vector. That said, inserting a \em
			 *          new vector containing a value at entry \ref position that already exists in
			 *          the set of points will cause the old vector containing that same value at
			 *          entry \ref position to be \em completely \em overwritten.
			 */
			void insert(const value_type& vec)
			{
				_data[vec[position]] = vec;
			}

			/**
			 * \brief   Erase a data point (vector) from the set.
			 *
			 * \details Attempting to erase a point that is not already in the set has no effect.
			 *
			 * \arg     vec is the N-dimensional vector that represents the N-dimensional point
			 *          to be erased from the set of data points
			 */
			void erase(const value_type& vec)
			{
				// [Find the vector in the set.]
				typename map_t::iterator iter = _data.lower_bound(vec[position]);
				if (iter != _data.end() && iter->second == vec) {
					_data.erase(iter);
				}
			}

			/**
			 * \brief   Erase a data point (vector) from the set using just its key (keys are \ref
			 *          unique).
			 *
			 * \details Attempting to erase a point that is not already in the set has no effect.
			 *
			 * \arg     key is the value at entry \ref position of the point (vector) to be erased
			 *          from the set of data points
			 */
			void erase(const key_type& key)
			{
				// [Find the key in the set.]
				typename map_t::iterator iter = _data.lower_bound(key);
				if (iter != _data.end() && _eq(iter->first, key)) {
					_data.erase(iter);
				}
			}

			/**
			 * \brief   Erase all data points from the set.
			 */
			void clear()
			{
				_data.clear();
			}

			/**
			 * \brief   Test whether the set contains any data points.
			 *
			 * \returns true if the set has no data points; false if the set contains at least one
			 *          data point
			 */
			bool empty() const
			{
				return _data.empty();
			}

			/**
			 * \brief   Test whether a specific data point is contained within the set of data
			 *          points.
			 *
			 * \arg     vec is the N-dimensional vector that represents the N-dimensional point
			 *          to find in the set of data points
			 *
			 * \returns true if \c vec is found in the set of data points; false if \c vec is not in
			 *          the set of data points
			 */
			bool exists(const value_type& vec) const
			{
				// [Find the vector in the set.]
				typename map_t::const_iterator iter = _data.lower_bound(vec[position]);
				if (iter != _data.end() && iter->second == vec) {
					return true;
				} else {
					return false;
				}
			}

			/**
			 * \brief   Test whether a specific key exists within the set of data points.
			 *
			 * \arg     key is the value at entry \ref position of the point (vector)
			 *
			 * \returns true if \c key is found in the set of data points; false if \c key is not in
			 *          the set of data points
			 */
			bool exists(const key_type& key) const
			{
				// [Find the key in the set.]
				typename map_t::const_iterator iter = _data.lower_bound(key);
				if (iter != _data.end() && _eq(iter->first, key)) {
					return true;
				} else {
					return false;
				}
			}

			/**
			 * \brief     Retrieve the data point whose key is the greatest from the set of all points.
			 *
			 * \exception std::logic_error is thrown when the set of points is empty
			 */
			value_type max() const
			{
				if (_data.empty()) {
					throw std::logic_error("Lerp: no points from which to retrieve maximum");
				}
				return (_data.rbegin())->second;
			}

			/**
			 * \brief     Retrieve the data point whose key is the lowest from the set of all points.
			 *
			 * \exception std::logic_error is thrown when the set of points is empty
			 */
			value_type min() const
			{
				if (_data.empty()) {
					throw std::logic_error("Lerp: no points from which to retrieve minimum");
				}
				return (_data.begin())->second;
			}

			/**
			 * \brief   The number of recorded data points.
			 *
			 * \returns the number of data points that are currently in the set
			 */
			size_type size() const
			{
				return _data.size();
			}

			/**
			 * \brief     Interpolate an N-dimensional vector.
			 *
			 * \details   Interpolate an N-dimensional vector with respect to the key (component)
			 *            at \ref position using the data points that already exist in the set.
			 *
			 * \arg       key is the entry (component) of the \em unknown vector (i.e. the vector
			 *            that you're trying to find), which will be used to interpolate its
			 *            remaining entries (components) with respect to the existing data points in
			 *            the set
			 *
			 * \returns   an N-dimensional vector that contains the value of \c key at entry \ref
			 *            position, and whose remaining entries have been interpolated using the
			 *            data points in the set
			 *
			 * \exception std::domain_error is thrown when the component used for interpolation is
			 *            either lower than all the existing data points in the set or it is greater
			 *            than all the existing data points in the set; in other words, the
			 *            component would represent a vector that is outside the interval of
			 *            recorded data points
			 */
			value_type interpolate(const key_type& key) const
			{
				// [Try to find the key in the set.]
				typename map_t::const_iterator iter = _data.lower_bound(key);
				if (iter == _data.end() ||
				   (iter == _data.begin() && !_eq(iter->first, key))) {
					throw std::domain_error("Lerp: failed to interpolate; out of bounds");
				} else if (_eq(iter->first, key)) {
					return iter->second;
				} else {
					// [Interpolate!]
					// We need the two bounding points.
					const value_type rhs = iter->second;
					--iter;
					// Remarks: we're certain that "--iter" is legal and safe here because it's
					// impossible to enter this branch unless "iter != _data.begin()", that is,
					// we're certain that we're not at the very first point in the set.
					const value_type lhs = iter->second;
					// Find and return the linear interpolant.
					const value_type diff = rhs - lhs;
					const key_type   delta = (key - lhs[position]) / (rhs[position] - lhs[position]);
					return lhs + delta * diff;
				}
			}

			/**
			 * \brief     Interpolate an N-dimensional vector with possible truncation.
			 *
			 * \details   Interpolate an N-dimensional vector with respect to the key (component)
			 *            at \ref position using the data points that already exist in the set. If
			 *            the given key is outside the interval specified by the points, then the
			 *            key is truncated (clamped) to the nearest endpoint, which is subsequently
			 *            returned to the caller.
			 *
			 * \arg       key is the entry (component) of the \em unknown vector (i.e. the vector
			 *            that you're trying to find), which will be used to interpolate its
			 *            remaining entries (components) with respect to the existing data points in
			 *            the set; if key falls outside the interval specified by the points then it
			 *            will be truncated to the nearest endpoint in the set
			 *
			 * \returns   an N-dimensional vector that contains the possibly truncated value of \c
			 *            key at entry \ref position, and whose remaining entries have been
			 *            interpolated using the data points in the set
			 *
			 * \exception std::logic_error is thrown when the set of points is empty
 			 */
			value_type truncate(const key_type& key) const
			{
				value_type leftEndpt  = min();
				value_type rightEndpt = max();
				// Truncate from the left?
				if (_lt(key, leftEndpt[position]) || _eq(key, leftEndpt[position])) {
					return leftEndpt;
				// Truncate from the right?
				} else if (_lt(rightEndpt[position], key) || _eq(rightEndpt[position], key)) {
					return rightEndpt;
				// Neither: we're within the interval.
				} else {
					return interpolate(key);
				}
			}

		private:
			//! @cond INTERNAL
			// Function object for comparing to real numbers for equality.
			const key_equality _eq;
			//! @endcond

			//! @cond INTERNAL
			// Function object for comparing to real numbers for ordering.
			const key_compare  _lt;
			//! @endcond

			//! @cond INTERNAL
			// For convenience.
			typedef std::map<key_type, value_type, key_compare> map_t;
			//! @endcond

			//! @cond INTERNAL
			// The set (collection) of data points (vectors).
			map_t _data;
			//! @endcond
	};
}

#endif
