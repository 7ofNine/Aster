#include "Power10.h"


Power10::CacheType Power10::cache;

Power10::Power10(){}


Power10::~Power10(){}

long double Power10::operator()(int const power)
{
	RefElement ref = cache.find(power);

	if (ref == cache.end())
	{
		ref = cache.insert({ power, value(power) }).first;
		//TODO: error handling
	}

	return ref->second;
}

long double Power10::value(int const power)
{
	long double start = 1.0;
	long double factor;
	int end;
	if (power < 0)
	{
		factor = 1.0E-1;
		end = - power;
	}else if (power > 0)
	{
		factor = 10.0;
		end = power;
	}else
	{
		return 1.0;
	}

	for (int i = 0; i < end; ++i)
	{
		start *= factor;
	}

	return start;
}
