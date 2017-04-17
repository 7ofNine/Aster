//
// A single record of the ephmeris file in palatable format for further processing
//
#pragma once

#include <fstream>
#include <vector>
#include <list>
#include <unordered_map>

#include "Chebysheff.h"
# 


//  the actual data
//  the entry descriptors
//  depending on the index the actual data record contains different number of components
//  from JPL document
//
//     INTERNAL FORMAT OF THE EPHEMERIS FILES
//  --------------------------------------
//  
//  On the first record of an export binary file or in the "GROUP 1050 of the ascii
//  "header", there are 3 sets of 15 integers each.  (Older versions have only the first 13
//  integers in each set)
//  
//  The 15 triplets give information about the location, order and time-coverage of
//  the chebychev polynomials corresponding to the following 15 items:
//  
//    1  3    Mercury
//    2  3    Venus
//    3  3    Earth-Moon barycenter
//    4  3    Mars 
//    5  3    Jupiter 
//    6  3    Saturn
//    7  3    Uranus
//    8  3    Neptune
//    9  3    Pluto
//    10 3    Moon (geocentric)
//    11 3    Sun
//    12 2    Earth Nutations in longitude and obliquity (IAU 1980 model)
//    13 3    Lunar mantle libration
//    14 3    Lunar mantle angular velocity
//    15 1    TT-TDB (at geocenter)
//  
//  Word (1,i) is the starting location in each data record of the chebychev 
//  coefficients belonging to the ith item.  Word (2,i) is the number of chebychev 
//  coefficients per component of the ith item, and Word (3,i) is the number of 
//  complete sets of coefficients in each data record for the ith item.
//  
//  Items not stored on the ascii files have 0 coefficients [Word (3,i)].
//  
//  Data Records ("GROUP 1070")
//  
//  These records contain the actual ephemeris data in the form of chebychev 
//  polynomials.
//  
//  The first two double precision words in each data record contain
//  
//           Julian date of earliest data in record.
//           Julian date of latest data in record.
//  
//  The remaining data are chebychev position coefficients for each component of 
//  each body on the tape.  The chebychev coefficients for the planets represent 
//  the solar system barycentric positions of the centers of the planetary systems.
//  
//  There are three Cartesian components (x, y, z), for each of the items #1-11; 
//  there are two components for the 12th item, nutations : d(psi) and d(epsilon);
//  there are three components for the 13th item, librations : phi, theta, psi;
//  there are three components for the 14th item, mantle omega_x,omega_y,omega_z;
//  there is one component for the 15th item, TT-TDB. (only in the DE432t version e.g.)
//  
//  Planetary positions are stored in units of kilometers (TDB-compatible).
//  The nutations and librations are stored in units of radians.
//  The mantle angular velocities are stored in radians/day.
//  TT-TDB is stored in seconds.


class EphemerisRecord 
{
public:
	enum class Entry // the raw types in the binary ephemeries file. The numerical value gives the order within the binary record
	{
		NONE      = -1,
		MERCURY   = 0,
		VENUS     = 1,
		EMB       = 2,
		MARS      = 3,
		JUPITER   = 4,
		STAURN    = 5,
		URANUS    = 6,
		NEPTUN    = 7,
		PLUTO     = 8,
		MOON      = 9,
		SUN       = 10,
		NUTATION  = 11,
		LIBRATION = 12,
		VELOCITY  = 13,
		TT_TDB    = 14,
	};

public:
    EphemerisRecord(std::ifstream & jpleph, bool & good);    

    // initialize i.e. read record 0
    void operator()();
    
    typedef std::vector<long double> RecordType;

    RecordType * operator[](int const numRecord);


private:

	// descibes the format of the original raw record in the binary file. TODO: better optimized file format.
   struct RecordDescriptorEntry
   {
        RecordDescriptorEntry(int const index, int const order, int const entries);
        int recordIndex;    // index into the record for the entry  (i.e. celestial body)
        int numCoefficient; // number of coefficients for this entry
        int numEntries;     // number of sub intervalls in the record
   };

    void getDescriptor(); // read the record structure descriptor. Note: This requires a proper positioning of the input stream!!
    RecordType * readRecord(int const numRecord);

    // caching functions
    RecordType * getRecord(const int k);
    void insert(int const k, RecordType  *  v);
    void evict();

    bool & good;
    std::vector<RecordDescriptorEntry> recordDescriptor;

   std::ifstream & jpleph;

   std::streampos recordStart;
   std::streamoff recordLength;
   std::streampos currentPosition;



  // the LRU cache for ephemeris records. The actual records are cached here and are retrieved as vectors
  typedef std::list<int> KeyTrackerType; 
  typedef std::unordered_map<int, std::pair< RecordType *, KeyTrackerType::iterator > > KeyToValueType; 

  // Maximum number of key-value pairs to be retained 
  const size_t capacity; 

    // Key access history 
  KeyTrackerType keyTracker; 
 
  // Key-to-value lookup 
  KeyToValueType keyToValue;  // the actual cache

  Chebysheff chebysheff;

};