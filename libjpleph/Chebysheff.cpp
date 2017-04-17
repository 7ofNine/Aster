#include "Chebysheff.h"

Chebysheff::Chebysheff(): twotc(0.0){ }


//
// t : interpolations zeitpunkt in range 0 <= t <= 1
//
// TODO: can the order be determined from the number of provided coefficients. i.e. size of vector?
void Chebysheff::operator()(long double const & t, int const order)
{
	bool velocity = true; // TODO: should come in as parameter
	int nsub = 1;// number of subintervals. TODO: To be determined from input!


	long double tScaled = nsub * t; // stretch to intervall length n
	int sub = (int)floor(tScaled);  // index of sub intervall (we are using only small positive integers!)
	bool endOfIntervall = false;
	if (sub == nsub)
	{
		sub -= 1;  // we hit the end of the last sub intervall . This has to be mapped to 1.0
		endOfIntervall = true;

	}

	long double tFraction = tScaled - (long)tScaled; // should be okay we are only using small integers for nsub

	// normalize input time t to standard chebysheff range -1.0 <= tc <= 1.0
	long double tc;
	if (!endOfIntervall)
	{
		tc = 2.0L*tFraction - 1.0L;
	}
	else
	{
		tc = 2.0L*(tFraction + 1.0L) - 1.0L;
	}


	// create chebysheff polynomial values
	if (p.size() >= 2 && p[1] != tc || p.size() < 2) // new value for tc i.e. we have to calculate new Chebysheff values
	{
		p.clear();
		p.reserve(order);
		p = { 1.0L };    // p[0]
		p.push_back(tc); // p[1]
		twotc = tc + tc;
		if (velocity)
		{
			v.clear();
			v.reserve(order);
			v = { 0.0L, 1.0 };
			v.push_back(twotc + twotc);
		}
	}

	// long double const twotc = tc + tc;
	if (p.size() - 1 < order)
	{
		for (int i = p.size(); i < order + 1; ++i)
		{
			p.push_back((twotc * p[i - 1]) - p[i - 2]);
		}
	}

	// calculate interpolated position

	if (velocity) // if speeds requested (has to become parameter)
	{
		// claculate the derivative Chebysheff values
		// calculate velocity factor
		//	v = { 0.0, 1.0 }; // [0], [1]
		if (v.size() - 1 < order)
		{
			for (int i = v.size(); i < order + 1; ++i)
			{
				v.push_back(twotc * v[i - 1] + p[i - 1] + p[i - 1] - v[i - 2]);
			}
		}

	}
}