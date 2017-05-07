#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "EphemerisRecord.h"

class Jpleph 
{

public:
    explicit Jpleph(std::string const & jplFileName, bool aukm = true, bool daysecond = true, bool iauau = false);

    struct Time
    {
        Time();
        double t1; // long double integer part
        double t2; // long double fractional part  
    };

    struct Posvel
    {
        Posvel();
        std::vector<double> pos; // position vector
        std::vector<double> vel; // velocity vector
    };

    enum class Target
    {
        NONE          =  0,
        MERCURY       =  1,
        VENUS         =  2,
        EARTH         =  3, 
        MARS          =  4,
        JUPITER       =  5,
        SATURN        =  6,
        URANUS        =  7,
        NEPTUN        =  8,
        PLUTO         =  9,
        MOON          = 10,
        SUN           = 11,
        SS_BARYCENTER = 12,
        EM_BARYCENTER = 13,
        NUTATIONS     = 14,  
        LIBRATIONS    = 15,
        LIBRATIONVELO = 16,
        TT_TTB        = 17,
    };

    enum class InterpolationType
    {
        NONE              = 0,
        POSITION          = 1,
        POSITION_VELOCITY = 2,
    };

    typedef std::array<InterpolationType, int(Target::LIBRATIONS)> BodyList;
    typedef std::array<Posvel, int(Target::LIBRATIONS)>            PosvelList;

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
//     The numbering convention for 'target' and 'center' is:     
//     the corresponding enum numericla value is this value -1
//
//                                                                       
//                1 = Mercury                                
//                2 = Venus                                    
//                3 = Earth            
//                4 = Mars system barycenter
//                5 = Jupiter system barycenter
//                6 = Saturn system brycenter
//                7 = Uranus system barycenter
//                8 = Neptune system barycenter
//                9 = Pluto system barycenter
//                10 = Moon(of Earth)
//                11 = Sun
//                12 = Solar - System Barycenter
//                13 = Earth - Moon barycenter
//                14 = Nutations(Longitude and Obliquity)
//                15 = Lunar Euler angles : phi, theta, psi
//                16 = Lunar angular velocity : omegax, omegay, omegaz
//                17 = TT - TDB
//
//      Note that not all ephemerides include all of the above quantities.
//      When a quantity is requested that is not on the file,
//      a warning is printed and the components of PV are set to - 99.d99,
//      which is not a valid value for any quantity.
//
//      For nutations, librations, and TT - TDB, 'center' is ignored
//
//      posvel     returned values

// TODO: extend description see testeph1.f
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

    void dpleph(Time const & et, Target const target, Target const center , Posvel & posvel);  

    // read the names and values of the ephemeries constants
    void constants(std::vector<std::string> & names, std::vector<double> & values,
                   double & dateStart, double & dateEnd, double & dateInterval) const;

private:

    void split(double const time, Time & preciseTime);
    Time & determineTime(Time const & inTime, Time & interpolationTime);
    void calculateFactors(bool aukm, bool daysecond, bool iauau);
    bool inDateRange(Time const & interpolationTime);
    bool isPresent(EphemerisRecord::Entry const body);

    
   std::ifstream jpleph;
   bool good; 
   std::streampos recordStart;
   std::streamoff recordLength;
   std::streampos currentPositon;



    EphemerisRecord record;

    std::vector<std::string> constantNames;
    std::vector<double>      constantValues;


    // actual ephemeris data   
    long double dateStart;
    long double dateEnd;
    long double dateInterval;

    // constants needed for calculations
    long double au;     // au in km
    long double emrat;  // earthmoon mass ratio
    long int denum;     // JPL DE number (for logging purposes)
    double factorEarth; // mass factor for earth
    double factorMoon;  // mass factor for moon
    double xscale;      // scale factor for position
    double vscale;      // scale factor for velocity
};