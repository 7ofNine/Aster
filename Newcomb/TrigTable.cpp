#include "TrigTable.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace std;
// create the table of cos and sin values cos(i*x), sin(*i*x) with min <= i <= max; 
// TODO: the static test here might make it worth to use templates!!
TrigTable::TrigTable(int const min, int const max, double const x): min(min), max(max)
{
    if (min > max)
    {
        throw out_of_range("Invalid range for TrigTable");
    }

    double const c0 = 1.0;  //cos(0.0) = cos(0*x)
    double const s0 = 0.0;  //sin(0.0) = sin(0*x)
    double const c1 = std::cos(x);
    double const s1 = std::sin(x);

    int const size = abs(max - min) + 1;            // 
                                                    // The size needed is 2*limit +1
    cvalues = std::vector<double>(size);            // no reallocations and initialize to 0.0
    svalues = std::vector<double>(size);

    // we have to bias the vector access in order to be able to use the original multiples
    // i.e.
    //original multiples:  min, min - 1, min -2, ...,    -1,        0,      +1,   ...   max - 1,         max
    //vector index      :  0  , 1      , 2,           |min| - 1,  |min|, |min| +1,     |max| + |min| -1, |max| + |min|  
    // example
    // min = -2;
    // max = +3;
    // mutltiples                 -2, -1, 0, 1, 2, 3
    // index                       0,  1, 2, 3, 4, 5,  (size = 6)
    //set multiple 0 first (fixed values)
    if (min <= 0 && 0 <= max) // if 0 is in range set it
    {
        cvalues[-min] = c0;
        svalues[-min] = s0;
    }

    // set the first real value
    if (min <= 1 && 1 <= max) // within the multiples range
    {
        cvalues[1 - min] = c1; // set  multiple 1
        svalues[1 - min] = s1;
        // fill from 2 to max
        for (int i = 1; i < max; ++i)
        {
            addTheorem(cvalues[i - min], svalues[i - min], c1, s1, cvalues[i + 1 - min], svalues[i + 1 - min]);
        }

        if (min < 0) // multiple <= -1
        {
            // fill from -1 to min
            cvalues[-1 - min] =  c1; // cos(-x) =  cos(x)
            svalues[-1 - min] = -s1; // sin(-x) = -sin(x)
            for (int i = 1; i < -min; ++i)
            {
                addTheorem(cvalues[-i - min], svalues[-i - min], c1, -s1, cvalues[-(i + 1) - min], svalues[-(i + 1) - min]);
            }
        }

    } else
    { // multiple 1 is outside of multiples range  

        // todo : missing cases 1 < min or 1 > max
        if (1 < min)
        {
            // first build up to the min value
            double ctemp = c1;
            double stemp = s1;
            for (int i = 1; i < min; ++i)
            {
                addTheorem(ctemp, stemp, c1, s1, ctemp, stemp);
            }
            // ctemp, temp contain value for min
            // fill the values
            cvalues[0] = ctemp;
            svalues[0] = stemp;
            for (int i = min; i < max; ++i)
            {
                addTheorem(cvalues[i - min], svalues[i - min], c1, s1, cvalues[i + 1 - min], svalues[i + 1 - min]);
            }
        }

        if (1 > max) // the case 0 has already been dealt with
        {

            if (max == 0 && min < 0) // i.e. -1 is in the interval of multiples
            {
                cvalues[max - 1 - min] = c1;
                svalues[max - 1 - min] = -s1;

                for (int i = -max + 1; i < -min; ++i)
                {
                    addTheorem(cvalues[-i - min], svalues[-i - min], c1, -s1, cvalues[-(i + 1) - min], svalues[-(i + 1) - min]);
                }
            }
            else if(max < 0 && min < 0) // max <=-1
            {
                // first build up to the max value (we are decreesing the index)
                // the case max = 0 is treated above
                double ctemp = c1; // this correpsonds to -1
                double stemp = -s1;
                for (int i = 1; i < -max; ++i)
                {
                    addTheorem(ctemp, stemp, c1, -s1, ctemp, stemp);
                }
                cvalues[max - min] = ctemp;
                svalues[max - min] = stemp;

                for (int i = -max; i < -min; ++i)
                {
                    addTheorem(cvalues[-i - min], svalues[-i - min], c1, -s1, cvalues[-(i + 1) - min], svalues[-(i + 1) - min]);
                }
            }
        }
    }
}

TrigTable::~TrigTable()
{
}

double TrigTable::sin(int const i) const
{
    return svalues.at(i - min);
}

double TrigTable::cos(int const i) const
{
    return cvalues.at(i - min);
}


void TrigTable::addTheorem(double const c1, double const s1, double const c2, double const s2, double & cv, double & sv)
{
    cv = c1*c2 - s1*s2; // cos addition theorem
    sv = s1*c2 + c1*s2; // sin addition theorem
}
