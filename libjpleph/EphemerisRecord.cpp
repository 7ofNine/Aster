
#include <cassert>
#include "EphemerisRecord.h"
#include "jplephread.h"

//#include "RecordDescriptor.h"
using namespace std;

EphemerisRecord::EphemerisRecord(ifstream & jpleph, bool & good):good(good), jpleph(jpleph), recordLength(0), currentPosition(0), capacity(10) {} 

void EphemerisRecord::operator()() // to be called when input stream is properly positioned
{

    //first read the record descriptor
    getDescriptor();
    if(good)
    {
        recordStart = jpleph.tellg();
    }

   readRecord(0); // read record 0 to set up the data. Could that be avoided?
   if(good)
   {
      currentPosition = jpleph.tellg();
      recordLength    = currentPosition - recordStart;
   }
    
}

EphemerisRecord::RecordType * EphemerisRecord::operator[](int const numRecord)
{
    return getRecord(numRecord);
   // return this->()(numRecord);
}

EphemerisRecord::RecordType * EphemerisRecord::readRecord(int const numRecord)
{
    if(numRecord >= 0)
    {
        //calculate stream position
        streampos newPosition = recordStart + numRecord * recordLength;
        if(newPosition != currentPosition)
        {
            jpleph.seekg(newPosition);
        }
        RecordType * newRecord = new RecordType();
        good = read(jpleph, *newRecord);
        if(good)
        {
            currentPosition = jpleph.tellg();

			// now make record useable for interpolation:
			// TODO: better structure in binary file
			// use Eigen objects?
			// can we do something better with enum in C++11
			auto record = new vector < vector < vector< vector<long double> * > *> *>;
			(*record).reserve(15);

			for (int i = 0; i < int(Entry::TT_TDB) + 1; ++i) // for all different objects
			{
				// vector<>  : number of objects
				// of vector<> : number of sub intervals
				//    of vector<> : order
				auto subIntervals = new vector < vector< vector<long double> * > *>;
				(*subIntervals).reserve(recordDescriptor[i].numEntries);


					for (int j = 0; j < recordDescriptor[i].numEntries; ++j)  // number of subintervals
					{
						auto coefficients = new vector< vector<long double> * >;
						(*coefficients).reserve(recordDescriptor[i].numCoefficient);
						for (int k = 0; k < recordDescriptor[i].numCoefficient; ++k) // order of the polynomial
						{
							auto  coeff = new vector<long double>;
							(*coeff).reserve(3);
							if (i != 11 && i != 14)
							{
								for (int l = 0; l < 3; ++l)   // single components x,y,z
								{
									(*coeff).push_back((*newRecord)[recordDescriptor[i].recordIndex + (3 * j + l)*recordDescriptor[i].numCoefficient + k]); // access element in raw record TODO: reorganize file format
								}

								(*coefficients).push_back(coeff);
							} else if (i == 11) // nutation in longitude and obliquity
							{
								for (int l = 0; l < 2; ++l)   // single components d(psi) and d(epsilon)
								{
									(*coeff).push_back((*newRecord)[recordDescriptor[i].recordIndex + (2 * j + l)*recordDescriptor[i].numCoefficient + k]); // access element in raw record TODO: reorganize file format
								}

							}
							else // object 15 1 component TT to TDB 
							{
								(*coeff).push_back((*newRecord)[recordDescriptor[i].recordIndex + j*recordDescriptor[i].numCoefficient + k]); // access element in raw record TODO: reorganize file format
							}

						}

						(*subIntervals).push_back(coefficients);
					}


				(*record).push_back(subIntervals);
			}

            return newRecord;
        }
    }
    good = false;
    return NULL;
        //throw
}

EphemerisRecord::RecordDescriptorEntry::RecordDescriptorEntry(int const index, int const order, int const entries)
{
    recordIndex     = index;
    numCoefficient  = order;
    numEntries      = entries;
}


void EphemerisRecord::getDescriptor()
{
    vector<int> index; // fortran index into the data record
    vector<int> order; // number of chebycheff coefficients/ order
    vector<int> entries; // number of entries for the data record

    if(good)
    {
        good = read(jpleph, index);
    }

    if(good)
    {
        good = read(jpleph, order);
    }

    if(good)
    {
        good = read(jpleph, entries);
    }

    // they should all have the same size. TODO? put it differently into the binary file <vector<vector<int>>. Would make it possible to read it as one object
    good = (index.size() == order.size()) && (order.size() == entries.size());
    recordDescriptor.reserve(index.size());
    for(size_t i = 0; i < index.size(); ++i)
    {
        recordDescriptor.push_back(RecordDescriptorEntry(index[i] - 1, order[i], entries[i]));  // change Fortran index into C++ index (base 1 -> base 0)
    }
    
}


  // cache functions  
  // Obtain value of the cached function for k 
  EphemerisRecord::RecordType  * EphemerisRecord::getRecord(const int k)
  { 
    // Attempt to find existing record 
    const KeyToValueType::iterator it = keyToValue.find(k); 
 
    if (it == keyToValue.end())
    { 
        // We don't have it: 
        // Evaluate function and create new record 
        RecordType  * v = readRecord(k); 
        insert(k, v); 
 
        // Return the freshly computed value 
        return v; 
 
    } else
    { 
        // We do have it: 
        // Update access record by moving 
        // accessed key to back of list 
        keyTracker.splice(keyTracker.end(), keyTracker, (*it).second.second); 
 
        // Return the retrieved value 
        return (*it).second.first; 
    } 
  }

    // Record a fresh key-value pair in the cache 
  void EphemerisRecord::insert(int const k, RecordType  *  v)
  { 
 
    // Method is only called on cache misses 
    assert(keyToValue.find(k) == keyToValue.end()); 
 
    // Make space if necessary 
    if (keyToValue.size() == capacity)
    { 
      evict(); 
    }
 
    // Record k as most-recently-used key 
    KeyTrackerType::iterator it = keyTracker.insert(keyTracker.end(), k); 
 
    // Create the key-value entry, 
    // linked to the usage record. 
    keyToValue.insert(std::make_pair(k, std::make_pair(v, it) )); 

    // No need to check return, 
    // given previous assert. 
  } 

    // Purge the least-recently-used element in the cache 
  void EphemerisRecord::evict()
  { 
    // Assert method is never called when cache is empty 
    assert(!keyTracker.empty()); 
 
    // Identify least recently used key 
    const KeyToValueType::iterator it = keyToValue.find(keyTracker.front()); 

    assert(it != keyToValue.end()); 
 
    // Erase both elements to completely purge record 
    delete (*it).second.first;
    keyToValue.erase(it); 
    keyTracker.pop_front(); 
  } 



