#pragma once

#include <vector>
// Helper class to keep trigonometric arguments
// calculates all needed sin/cos of multiples of a given angle

class TrigTable
{
public:
    // min: minimal multiple of x, max: maximal multiple of x, x: rad of angle
    //TODO: use array+ template instead??
    TrigTable() = delete;
    TrigTable(int const min, int const max, double const x);
    
    ~TrigTable();

    double sin(int const i) const;
    double cos(int const i) const;
    static void addTheorem(double const c1, double const s1, double const c2, double const s2, double & cv, double & sv); // the addition theorem for trigonometric functions

private:
    int const min;
    int const max;
    std::vector<double> cvalues; // the cos values
    std::vector<double> svalues; // the sin values
};

