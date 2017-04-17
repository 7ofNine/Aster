//
// class to provide access to a binary jpl ephemeris file
// the ephmeris file had to be written with the corresponding asc2eph file
// it is not compatible with the original JPL Fortran format!

#include <fstream>
#include <cmath>
#include <exception>

#include "jpleph.h"
#include "jplephread.h"
#include "EphemerisRecord.h"

#include "Chebysheff.h"

using namespace std;

Jpleph::Jpleph(string const &  jplFileName):good(false), record(jpleph, good)
{
    jpleph.open(jplFileName, ifstream::binary);
    good = jpleph.good();
    // read the header
    vector<string> ttl;
    if(good)
    {
        good = read(jpleph, ttl); // read TTL
    }

    vector<string> constantNames; //CNAM
    if(good)
    {
        good = read(jpleph, constantNames);
    }

    vector<long double> constantValues; //CVAL
    if(good)
    {
        good = read(jpleph, constantValues);
    }

    good = constantNames.size() == constantValues.size();

    if(good)
    {
        good = read(jpleph, dateStart);
    }

    if(good)
    {
        good = read(jpleph, dateEnd);
    }

    if(good)
    {
        good = read(jpleph, dateInterval);
    }

    long double au;
    long double emrat;
    long int denum;
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
     record(); // initialize the record keeper. this reads record 0 (Fortran 1)


     //TODO: for test only
     std::vector<long double> *currentRecord = record[5];
     std::vector<long double> *sameRecord    = record[5];
     std::vector<long double> *record2    = record[6];
     std::vector<long double> *record3    = record[7];
     std::vector<long double> *record4    = record[8];
     std::vector<long double> *record5    = record[10];
     std::vector<long double> *record6    = record[12];
     std::vector<long double> *record7    = record[13];
     std::vector<long double> *record8    = record[15];
     std::vector<long double> *record9    = record[18];
     std::vector<long double> *record10    = record[0];
     std::vector<long double> *record11    = record[42]; // this should evict

	 Chebysheff chebysheff;
	 chebysheff(0.1, 5);
	 chebysheff(0.1, 7);
	 chebysheff(0.5, 5);


}


 bool Jpleph::dpleph(Time const & et, Target const target, Target const center/*, vector<long double> positionSpeed*/)
 {
    if(good)
    {
        return true;
    }
    return false;
 }


 // the actual intrpolation method
 void Jpleph::state(Time const & time)
 {
    if(time.t1 == 0.0)
    {
        return; //?? what about t = 0.0?
    }

    Time interpolationTime = determineTime(time, interpolationTime);

    // check if epoch is in range
    if(interpolationTime.t1 + interpolationTime.t2 < dateStart || interpolationTime.t1 + interpolationTime.t2 > dateEnd)
    {
        //throw out of range
       // throw out_of_range("Jpleph::state: Inerpolation time out of epehemris range " + dateStart + "/" + dateEnd);
    }
    
    // calculate ephemris record to load
    int loadRecord = int(floor((interpolationTime.t1 - dateStart)/dateInterval));
    if(interpolationTime.t1 == dateEnd)
    {
        loadRecord--;
    }
    // get the ephemeris record
    EphemerisRecord::RecordType * currentRecord = record[loadRecord];

    //scale time relative into the record (0<= tScaled <= 1.0
    long double tScaled = (interpolationTime.t1 - ((dateInterval * loadRecord) + dateStart) + interpolationTime.t2)/dateInterval;
 }

 Jpleph::Time & Jpleph::determineTime(Time const & inTime, Time & interpolationTime)
 {
    long double s = inTime.t1 - 0.5; // reduce from noon
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


void Jpleph::split(long double const time, Time & preciseTime)
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