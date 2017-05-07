//
// class representing the values of the different chebysheff polynomials
// up to specific order
// we have a dynamic handling in order to minimize recalcluations
//
#ifndef CHEBYSHEFF_H
#define CHEBYSHEFF_H

#include <vector>

#include "EphemerisRecord.h"


class Chebysheff 
{
    public:
    explicit Chebysheff(EphemerisRecord::RecordType const & record, double const & secspan);
    void operator()(double const & interpolPoint, int const order);
    void operator()(double const & time, int const body, bool const vel, std::vector<double> & position, std::vector<double> & velocity);

    private:
        double normalizedTime(double const & time, int const nsub, int & sub); // normalize to proper intervall and determine subintervall
        void createPolyValues(double const & tc, int const order, bool const vel, std::vector<double> & pc, std::vector<double> & vc);
	double twotc;
	std::vector<double>  p; // chebysheff polynomial values for position
	std::vector<double>  v; // chebysheff polynomial values for velocity (1 st derivative)

    EphemerisRecord::RecordType const & record;

    double const secspan; // record interval in seconds
};

#endif