//
// class representing the values of the different chebysheff polynomials
// up to specific order
// we have a dynamic handling in order to minimize recalcluations
//
#pragma once
#include <vector>

class Chebysheff 
{
    public:
    Chebysheff();
    void operator()(long double const & interpolPoint, int const order);

    private:
	long double twotc;
	std::vector<long double>  p; // chebysheff polynomial values for position
	std::vector<long double>  v; // chebysheff polynomial values for velocity (1 st derivative)
};