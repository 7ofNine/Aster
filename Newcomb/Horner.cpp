#include "Horner.h"


using namespace std;


Horner::Horner(std::vector<double> const & coeff): coeff(coeff)
{ }

Horner::Horner(std::initializer_list<double> coeff): coeff(coeff)
{ }

Horner::~Horner()
{ }

double Horner::operator()(double const x) const
{
    // because of the order of the coefficients we do a reverse order. This corresponds to the Horner scheme.
    double value = 0.0;
    auto const end = coeff.rend();
    for (auto it = coeff.rbegin(); it != end; ++it)
    {
        value = value * x + *it;
    }
    return value;
}
