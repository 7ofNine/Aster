#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "EphemerisRecord.h"
//#include "RecordDescriptor.h"

class Jpleph 
{

public:
    explicit Jpleph(std::string const & jplFileName);

    struct Time
    {
        long double t1; // long double integer part
        long double t2; // long double fractional part  
    };

    enum class Target
    {
        OBJECT_NONE          =  0,
        OBJECT_MERCURY       =  1,
        OBJECT_VENUS         =  2,
        OBJECT_EARTH         =  3, 
        OBJECT_MARS          =  4,
        OBJECT_JUPITER       =  5,
        OBJECT_SATURN        =  6,
        OBJECT_URANUS        =  7,
        OBJECT_NEPTUN        =  8,
        OBJECT_PLUTO         =  9,
        OBJECT_MOON          = 10,
        OBJECT_SUN           = 11,
        OBJECT_SS_BARYCENTER = 12,
        OBJECT_EM_BARYCENTER = 13,
        OBJECT_NUTATIONS     = 14,  
        OBJECT_LIBRATIONS    = 15,
    };

    enum InterpolationType
    {
        INTERPOLATION_NOME              = 0,
        INTERPOLATION_POSITION          = 1,
        INTERPOLATION_POSITION_VELOCITY = 2,
    };


//     this method reads the jpl planetary ephemeris                 
//     and gives the position and velocity of the point 'target'          
//     with respect to 'center'.                                          
//                                                                       
//     calling sequence parameters:                                      
//                                                                       
//       et = julian ephemeris date at which interpolation          
//            is wanted.                                                 
//                                                                       
//       ** note the entry dpleph for a doubly-dimensioned time **       
//                                                                       
//     target = the 'target' point.                         
//                                                                       
//     ncent = the  center point.                           
//                                                                       
//            the numbering convention for 'target' and 'center' is:       
//                                                                       
//                1 = mercury           8 = neptune                      
//                2 = venus             9 = pluto                        
//                3 = earth            10 = moon                         
//                4 = mars             11 = sun                          
//                5 = jupiter          12 = solar-system barycenter      
//                6 = saturn           13 = earth-moon barycenter        
//                7 = uranus           14 = nutations (longitude and obli
//                            15 = librations, if on eph file            
//                                                                       
//             (if nutations are wanted, set ntarg = 14. for librations, 
//              set ntarg = 15. set ncent=0.)                            
//              
//      Remark: Note that the values for target and center are different than
//              the ones used for indexing into the raw binary ephemeris file.
//              For those ones see below   
//    
//      TODO: change this. What will it be                                                                         
//      rrd = output 6-word d.p. array containing position and velocity  
//            of point 'ntarg' relative to 'ncent'. the units are au and 
//            au/day. for librations the units are radians and radians   
//            per day. in the case of nutations the first four words of  
//            rrd will be set to nutations and rates, having units of    
//            radians and radians/day.                                   
//                                                                       
//            the option is available to have the units in km and km/sec.
//            for this, set km=.true. in the stcomx common block.        
//                                                                       

    bool dpleph(Time const & et, Target const target, Target const center/*, vector<long double> positionSpeed*/);  
private:

    void split(long double const time, Time & preciseTime);
    void state(Time const & time);
    Time & determineTime(Time const & inTime, Time & interpolationTime);

    
   std::ifstream jpleph;
   bool good; 
   std::streampos recordStart;
   std::streamoff recordLength;
   std::streampos currentPositon;

   EphemerisRecord readRecord(int const numRecord);


//   vector<int> index; // fortran index into the data record
//   vector<int> order; // number of chebycheff coefficients/ order
//   vector<int> entries; // number of entries for the data record
//    RecordDescriptor  recordDescriptor;

    EphemerisRecord record;

    // actual ephemeris data   
    long double dateStart;
    long double dateEnd;
    long double dateInterval;

};