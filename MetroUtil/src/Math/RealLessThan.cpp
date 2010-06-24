#include "RealLessThan.h"
using namespace metrobotics;

RealLessThan::RealLessThan(double epsilon)
:RealPredicate(epsilon)
{
}

bool RealLessThan::operator()(const double& lhs, const double& rhs) const
{
	return (rhs - lhs) > marginOfError();
}
