//
// class to provide access to a binary jpl ephemeris file
// the ephmeris file had to be written with the corresponding asc2eph file
// it is not compatible with the original JPL Fortran format!

#include <fstream>
#include <cmath>
#include <exception>
#include <iostream>
#include <iomanip>

#include "jpleph.h"
#include "jplephread.h"
#include "EphemerisRecord.h"

#include "Chebysheff.h"

using namespace std;

namespace {
    static const double IAU = 149597870.700;      // IAU 2000 value for au in km
    static const double SECONDS_PER_DAY = 86400.0; // the number of seconds in a day 
}

Jpleph::Jpleph(string const &  jplFileName, bool aukm, bool daysecond, bool iauau):good(false), record(jpleph, good)
{
    jpleph.open(jplFileName, ifstream::binary);
    good = jpleph.good();
    // read the header
    vector<string> ttl;
    if(good)
    {
        good = read(jpleph, ttl); // read TTL
    }

    //CNAM
    vector<string> constantNames;
    if(good)
    {
        good = read(jpleph, constantNames);
    }

    //CVAL
    vector<double> constantValues;
    if(good)
    {
        good = read(jpleph, constantValues);
    }

    good = constantNames.size() == constantValues.size();

    jplConstants.reserve(constantValues.size());
    for (size_t i = 0; i < constantNames.size(); ++i)
    {
        jplConstants.push_back(Constant(constantNames.at(i), constantValues.at(i)));
    }

    //SS(1)
    if(good)
    {
        good = read(jpleph, dateStart);
    }

    //SS(2)
    if(good)
    {
        good = read(jpleph, dateEnd);
    }

    //SS(3)
    if(good)
    {
        good = read(jpleph, dateInterval);
    }

    if(good)
    {
        good = read(jpleph, au);
    }

    if(good)
    {
        good = read(jpleph, emrat);
    }

    if(good)
    {
        good = read(jpleph, denum);
    }


    // read the first record to set up the size and the ramdom access
     record(); // initialize the record keeper. this reads record 0 (Fortran 1) of actual ephemeries data

     //has to be after reading the data 
     calculateFactors(aukm, daysecond, iauau);
}


// the actual retrieval method.
// At time t get the position (and) velocity of target with respect to center
void Jpleph::dpleph(Time const & etd, Target const target, Target const center, Posvel & posvel)
{
    //sanity check for proper combination of target and center
    if (target < Target::MERCURY || target > Target::TT_TTB)
    {
        cerr << "Jpleph::dpleph: Invalid target " << int(target) << endl;
        throw out_of_range("Invalid target");
    }

    // no center for non body values
    if (   (center <= Target::NONE && target < Target::NUTATIONS)
        || (center > Target::EM_BARYCENTER && target < Target::NUTATIONS)
        || (target <= Target::EM_BARYCENTER && center >= Target::NUTATIONS) )
    {
        cerr << "Jpleph::dpleph: Invalid center " << int(center) << " for target " << int(target) << endl;
        throw out_of_range("Invalid center");
    }


    // target anc center are the same. Location vector and relative speeds are both 0.0 
    if (target <= Target::EM_BARYCENTER && target == center)
    {   // target and center are equal
        posvel.pos = std::vector<double>({ 0.0, 0.0, 0.0 });
        posvel.vel = std::vector<double>({ 0.0, 0.0, 0.0 });
        return;
    }

    Time interpolationTime = determineTime(etd, interpolationTime); // Really needed? 

    if (!inDateRange(interpolationTime))
    {
        cerr << "Jpleph::dpleph: Requested date " 
             << fixed << showpoint << setw(14) << setprecision(5) << (etd.t1 + etd.t2)
             << " not in ephmeries range "
             << fixed << showpoint << setw(10) << setprecision(1) << dateStart 
             << " < t < " 
             << fixed << showpoint << setw(10) << setprecision(1) << dateEnd
             << endl;

        throw out_of_range("Date outside of ephemeries range");
    }

    // calculate ephemris record to load
    int loadRecord = int(floor((interpolationTime.t1 - dateStart) / dateInterval));
    if (interpolationTime.t1 == dateEnd)
    {
        loadRecord--;
    }

    // scale time relative into the record (0<= tScaled <= 1.0). This is the range for time t the chebysheff interpolation routine expects.
    long double tScaled = (interpolationTime.t1 - ((dateInterval * loadRecord) + dateStart) + interpolationTime.t2) / dateInterval;

    Chebysheff chebysheff(record[loadRecord], dateInterval * SECONDS_PER_DAY); // set up interpolation
    //do auxiliary cases first
    //Nutation
    if (Target::NUTATIONS == target)
    {
        if (isPresent(EphemerisRecord::Entry::NUTATION))
        {
            chebysheff(tScaled, int(EphemerisRecord::Entry::NUTATION), true, posvel.pos, posvel.vel); // dimension =2
            posvel.vel[0] = SECONDS_PER_DAY * posvel.vel[0];  // always convert to radians/day
            posvel.vel[1] = SECONDS_PER_DAY * posvel.vel[1];
            return;
        }
        else
        {
            cerr << "Jpleph::dpleph: Requested \"Nutation\" but not in ephemeries file";
            throw invalid_argument("Nutation not in ephemeries file");
        }
     }

    // Lunar mantle Euler angles
    if (Target::LIBRATIONS == target)
    {
        if (isPresent(EphemerisRecord::Entry::LIBRATION))
        {
            chebysheff(tScaled, int(EphemerisRecord::Entry::LIBRATION), true, posvel.pos, posvel.vel); // dimension = 3
            posvel.vel[0] = SECONDS_PER_DAY * posvel.vel[0];  // always convert to radians/day
            posvel.vel[1] = SECONDS_PER_DAY * posvel.vel[1];
            posvel.vel[2] = SECONDS_PER_DAY * posvel.vel[2];
            return;
        }
        else
        {
            cerr << "Jpleph::dpleph: Requested \"Libration\" but not in ephemeries file";
            throw invalid_argument("Libration not in ephemeries file");
        }
    }


    // Lunar mantle angular velocity
    if (Target::LIBRATIONVELO == target)
    {
        if (isPresent(EphemerisRecord::Entry::VELOCITY))
        {
            chebysheff(tScaled, int(EphemerisRecord::Entry::VELOCITY), true, posvel.pos, posvel.vel); // dimension = 3
            posvel.vel[0] = SECONDS_PER_DAY * posvel.vel[0];  // always convert to radians/day**2
            posvel.vel[1] = SECONDS_PER_DAY * posvel.vel[1];
            posvel.vel[2] = SECONDS_PER_DAY * posvel.vel[2];
            return;
        }
        else
        {
            cerr << "Jpleph::dpleph: Requested \"Libration Rates\" but not in ephemeries file";
            throw invalid_argument("Libration rates not in ephemeries file");
        }
    }


    // TT - TDB
    if (Target::TT_TTB == target)
    {
        if (isPresent(EphemerisRecord::Entry::TT_TDB))
        {
            chebysheff(tScaled, int(EphemerisRecord::Entry::TT_TDB), true, posvel.pos, posvel.vel); // dimension = 1
            posvel.vel[0] = SECONDS_PER_DAY * posvel.vel[0];  // always convert to seconds/second to second/day
            return;
        }
        else
        {
            cerr << "Jpleph::dpleph: Requested \"TT-TDB\" but not in ephemeries file";
            throw invalid_argument("TT-TDB not in ephemeries file");
        }
    }


    // lookup for bodies
    if (target == Target::MOON && center == Target::EARTH)
    {
        chebysheff(tScaled, int(EphemerisRecord::Entry::MOON), true, posvel.pos, posvel.vel); // dimension = 3
        for (int i = 0; i < 3; ++i)
        {
            posvel.pos[i] *= xscale;
            posvel.vel[i] *= vscale;
        }
        return;
    }

    if (target == Target::EARTH && center == Target::MOON)
    {
        chebysheff(tScaled, int(EphemerisRecord::Entry::MOON), true, posvel.pos, posvel.vel); // dimension = 3
        for (int i = 0; i < 3; ++i)
        {
            posvel.pos[i] *= -xscale;
            posvel.vel[i] *= -vscale;
        }
        return;
    }

    Posvel posvel1;
    Posvel posvel2;
    Posvel posvelMoon;
    Posvel posvelEMB;

    // cases involving moon xor earth
    if (target == Target::EARTH || target == Target::MOON || center == Target::EARTH || center == Target::MOON)
    {
        chebysheff(tScaled, int(EphemerisRecord::Entry::MOON), true, posvelMoon.pos, posvelMoon.vel); // dimension = 3
        chebysheff(tScaled, int(EphemerisRecord::Entry::EMB),  true, posvelEMB.pos,  posvelEMB.vel);   // dimension = 3

        if (target == Target::EARTH || target == Target::MOON)
        {
            if (center < Target::SS_BARYCENTER)
            {
                chebysheff(tScaled, int(center) - 1, true, posvel2.pos, posvel2.vel); // target as integer are the same in the given range as for Entry!!
            }
            else if (center == Target::EM_BARYCENTER)
            {
                chebysheff(tScaled, int(EphemerisRecord::Entry::EMB), true, posvel2.pos, posvel2.vel);
            }

            if (target == Target::EARTH)
            {
                for (int i = 0; i < 3; ++i)
                {
                    posvel.pos[i] = posvelEMB.pos[i] - factorEarth * posvelMoon.pos[i] - posvel2.pos[i];
                    posvel.vel[i] = posvelEMB.vel[i] - factorEarth * posvelMoon.vel[i] - posvel2.vel[i];
                }
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    posvel.pos[i] = posvelEMB.pos[i] - factorMoon * posvelMoon.pos[i] - posvel2.pos[i];
                    posvel.vel[i] = posvelEMB.vel[i] - factorMoon * posvelMoon.vel[i] - posvel2.vel[i];
                }

            }
        }
        else
        {
            if (target < Target::SS_BARYCENTER)
            {
                chebysheff(tScaled, int(target) - 1, true, posvel1.pos, posvel1.vel); // target as integer are the same in the given range as for Entry!!
            }
            else if (target == Target::EM_BARYCENTER)
            {
                chebysheff(tScaled, int(EphemerisRecord::Entry::EMB), true, posvel1.pos, posvel1.vel);
            }

            if (center == Target::EARTH)
            {
                for (int i = 0; i < 3; ++i)
                {
                    posvel.pos[i] = posvel1.pos[i] - (posvelEMB.pos[i] - factorEarth * posvelMoon.pos[i]);
                    posvel.vel[i] = posvel1.vel[i] - (posvelEMB.vel[i] - factorEarth * posvelMoon.vel[i]);
                }
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    posvel.pos[i] = posvel1.pos[i] - (posvelEMB.pos[i] - factorMoon * posvelMoon.pos[i]);
                    posvel.vel[i] = posvel1.vel[i] - (posvelEMB.vel[i] - factorMoon * posvelMoon.vel[i]);
                }
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            posvel.pos[i] *= xscale;
            posvel.vel[i] *= vscale;
        }

        return;
    }


    // cases not envolving moon or earth
    if (target < Target::SS_BARYCENTER)
    {
        chebysheff(tScaled, int(target) - 1, true, posvel1.pos, posvel1.vel); // target as integer are the same in the given range as for Entry!!
    }
    else if (target == Target::EM_BARYCENTER)
    {
        chebysheff(tScaled, int(EphemerisRecord::Entry::EMB), true, posvel1.pos, posvel1.vel);
    }

    if (center < Target::SS_BARYCENTER)
    {
        chebysheff(tScaled, int(center) - 1, true, posvel2.pos, posvel2.vel); // target as integer are the same in the given range as for Entry!!
    }
    else if (center == Target::EM_BARYCENTER)
    {
        chebysheff(tScaled, int(EphemerisRecord::Entry::EMB), true, posvel2.pos, posvel2.vel);
    }

    for (int i = 0; i < 3; ++i)
    {
        posvel.pos[i] = (posvel1.pos[i] - posvel2.pos[i]) * xscale;
        posvel.vel[i] = (posvel1.vel[i] - posvel2.vel[i]) * vscale;
    }
 }


 Jpleph::Time & Jpleph::determineTime(Time const & inTime, Time & interpolationTime)
 {
    double s = inTime.t1 - 0.5; // reduce from noon
    Time part1;
    split(s, part1);       // split integer and decimal part
    
    Time part2;
    split(inTime.t2, part2); // split integer and decimal part

    interpolationTime.t1 = part1.t1 + part2.t1 + 0.5;
    interpolationTime.t2 = part1.t2 + part2.t2; // this may result in a new integer component
    split(interpolationTime.t2, part2);   // split integer and decimal part 
    interpolationTime.t1 += part2.t1;
    interpolationTime.t2  = part2.t2;

    return interpolationTime;
 }


// split a given time into components in order to improve ephemeries precission
void Jpleph::split(double const time, Time & preciseTime)
{
    preciseTime.t2 = modf(time, &preciseTime.t1);  
    if(time >= 0 || preciseTime.t2 == 0.0)
    {
        return;
    }

    // correct for negative values
    preciseTime.t1 = preciseTime.t1 - 1.0; 
    preciseTime.t2 = preciseTime.t2 + 1.0; 
}

void Jpleph::calculateFactors(bool aukm, bool daysecond, bool iauau)
{
    factorEarth = 1.0 / (1.0 + emrat);
    factorMoon  = factorEarth - 1.0;

    if (factorEarth == 0.0)
    {
        throw invalid_argument("Mass factor for Earth could not be determined");
    }

    if (aukm) // use au instead of km
    {
        if (iauau) // use the IAU 2000 defined au value
        {
            xscale = 1.0 / IAU;
        }
        else
        {
            xscale = 1.0 / au;
        }
    }

    if (daysecond) // in seconds or days
    {
        vscale = xscale * SECONDS_PER_DAY; // per day
    } else
    { 
        vscale = xscale;           // per second
    }

    if (denum == 0)
    {
        throw invalid_argument("No DENUM found");
    }
}

// test if given time is within the date range of ephemeries file
bool Jpleph::inDateRange(Time const & interpolationTime)
{
    return (interpolationTime.t1 + interpolationTime.t2 >= dateStart && interpolationTime.t1 + interpolationTime.t2 <= dateEnd);
}

// test if data for 'body' is present the ephemeries file
bool Jpleph::isPresent(EphemerisRecord::Entry const body)
{
    EphemerisRecord::RecordDescriptorEntry const & descriptor = record.getDescriptorEntry(int(body));

    return (descriptor.recordIndex > 0 && (descriptor.numEntries) * (descriptor.numCoefficient) != 0);
}


void Jpleph::constants(Constants & cons, double & dateStart, double & dateEnd, double & dateInterval) const
{
    cons         = jplConstants;
    dateStart    = this->dateStart;
    dateEnd      = this->dateEnd;
    dateInterval = this->dateInterval;
}

Jpleph::Time::Time() : t1(0.0), t2(0.0) {}

Jpleph::Posvel::Posvel() : pos({ 0.0, 0.0, 0.0 }), vel({ 0.0, 0.0, 0.0 }) {}

Jpleph::Constant::Constant(string const name, double const value) : name(name), value(value) {}
Jpleph::Constant::Constant(Constant const & other) : name(other.name), value(other.value) {}
Jpleph::Constant Jpleph::Constant::operator=(Constant const & rhs)
{
    return Constant(rhs.name, rhs.value);
}

