#include "RealPredicate.h"
using namespace metrobotics;

RealPredicate::RealPredicate(double epsilon)
:_epsilon(epsilon)
{
}

void RealPredicate::marginOfError(double epsilon)
{
	_epsilon = epsilon;
}

double RealPredicate::marginOfError() const
{
	return _epsilon;
}
