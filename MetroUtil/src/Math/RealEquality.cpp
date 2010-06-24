#include <cmath>
using namespace std;

#include "RealEquality.h"
using namespace metrobotics;

RealEquality::RealEquality(double epsilon)
:RealPredicate(epsilon)
{
}

bool RealEquality::operator()(const double& lhs, const double& rhs) const
{
	return fabs(lhs - rhs) <= marginOfError();
}
