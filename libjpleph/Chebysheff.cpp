#include "Chebysheff.h"

Chebysheff::Chebysheff(EphemerisRecord::RecordType const & record, double const & secspan): twotc(0.0), record(record), secspan(secspan) { }

void Chebysheff::operator()(double const & time, int const body, bool const vel, std::vector<double> & position, std::vector<double> & velocity)
{
    // read the descriptor for the body
    EphemerisRecord::RecordDescriptorEntry const & entryDescriptor = record.getDescriptor(body);

    int const nsub = entryDescriptor.numEntries; // number of subintervalls in the record for 'body'

    int sub = 0;
    double const tc = normalizedTime(time, nsub, sub); // determine sub intervall and interpolation point within the intervall -1 <= tc <= 1.0

    // create chebysheff polynomial values
    createPolyValues(tc, entryDescriptor.numCoefficient, vel, p, v);

    // interpolation
    int const base = entryDescriptor.recordIndex + sub*(entryDescriptor.numCoefficient) * (entryDescriptor.dimension); 
    for (int i = 0; i < entryDescriptor.dimension; ++i)  
    {
        double tempp = 0.0;

        for (int j = entryDescriptor.numCoefficient - 1; j >= 0; --j)
        {
            tempp = tempp + p[j] * record.at(j + i *entryDescriptor.numCoefficient + base);
        }

        position[i] = tempp;
    }


    if (vel)
    {
        double const vfac = (2.0 * nsub) / secspan;
        for (int i = 0; i < entryDescriptor.dimension; ++i) 
        {
            double tempv = 0.0;
            for (int j = entryDescriptor.numCoefficient - 1; j >= 1; --j)
            {
                tempv = tempv + v[j] * record.at(j + i *entryDescriptor.numCoefficient + base);
            }

            velocity[i] = tempv * vfac;
        }
    }
}

double Chebysheff::normalizedTime(double const & time, int const nsub, int & sub)
{
    double tScaled = nsub * time;   // stretch to intervall length n
    sub = (int)floor(tScaled);      // index of sub intervall (we are using only small positive integers!)
    bool endOfIntervall = false;

    if (sub == nsub)
    {
        sub -= 1;   // we hit the end of the last sub intervall. This has to be mapped to 1.0
        endOfIntervall = true;

    }

    double tFraction = tScaled - (long)tScaled; // should be okay we are only using small integers for nsub

    // normalize input time t to standard chebysheff range -1.0 <= tc <= 1.0

    return (endOfIntervall? (2.0 * (tFraction + 1.0) - 1.0) : (2.0 * tFraction - 1.0));
}


// create the values of the different chebysheff polynomials for interpolation
void Chebysheff::createPolyValues(double const & tc, int const order, bool const vel, std::vector<double> & pc, std::vector<double> & vc)
{
    double const  twotc = tc + tc;

    // create chebysheff polynomial values
    if (pc.size() >= 2 && pc[1] != tc || pc.size() < 2) // new value for tc i.e. we have to calculate new Chebysheff values
    {
        pc.clear();
        pc.reserve(order);
        pc = { 1.0 };     // p[0]
        pc.push_back(tc); // p[1]

        if (vel)
        {
            vc.clear();
            vc.reserve(order);
            vc = { 0.0, 1.0, twotc + twotc }; // vc[0], vc[1], vc[2]
           // vc.push_back(twotc + twotc);
        }
    }

    // double const twotc = tc + tc;
    // fill up if not already calculated
    if (int(pc.size()) - 1 < order)
    {
        for (int i = pc.size(); i < order; ++i)
        {
            pc.push_back((twotc * pc[i - 1]) - pc[i - 2]);
        }
    }


    if (vel) // if speeds requested 
    {
        // caculate the derivative Chebysheff values
        if (int(vc.size()) - 1 < order)
        {
            for (int i = vc.size(); i < order; ++i)
            {
                vc.push_back(twotc * vc[i - 1] + pc[i - 1] + pc[i - 1] - vc[i - 2]);
            }
        }
    }
}