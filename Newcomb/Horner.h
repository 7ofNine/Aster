#pragma once
//
// Implement the Horner scheme calculation of a f real polynomial 
// Functiorial
#include <vector>

class Horner
{
public:
    // The polynomial coefficients are given with the lowest order first i.e. the index into the vector corresponds to the order of the variable of the polynomial
    // meaning the index is the order. with order n there are n+1 coefeficients.
    // 
    Horner(std::vector<double> const & coeff);
    Horner(std::initializer_list<double> list);
    ~Horner();

    double operator()(double const);

private :
    std::vector<double> coeff;
};

