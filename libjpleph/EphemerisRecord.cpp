
#include <cassert>
//#include "Eigen"
#include "jplephread.h"
#include "EphemerisRecord.h"




using namespace std;

EphemerisRecord::EphemerisRecord(ifstream & jpleph, bool & good):good(good), jpleph(jpleph), recordLength(0), currentPosition(0), capacity(10), numElements(0) {} 


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


void EphemerisRecord::getDescriptor()
{
    vector<int> index; // fortran index into the data record
    vector<int> order; // number of chebycheff coefficients/ order
    vector<int> subRecords; // number of entries for the data record

    if (good)
    {
        good = ::read(jpleph, index);
    }

    if (good)
    {
        good = ::read(jpleph, order);
    }

    if (good)
    {
        good = ::read(jpleph, subRecords);
    }

    // they should all have the same size. TODO? put it differently into the binary file <vector<vector<int>>. Would make it possible to read it as one object
    good = (index.size() == order.size()) && (order.size() == subRecords.size());
    recordDescriptor.reserve(index.size());
    for (size_t i = 0; i < index.size(); ++i)
    {
        recordDescriptor.push_back(RecordDescriptorEntry(index[i] - 1, order[i], subRecords[i]));  // change Fortran index into C++ index (base 1 -> base 0)
    }

    for (size_t i = 0; i < recordDescriptor.size(); ++i)
    {
        int dimension = 3;
        if (i == 11)
        {
            dimension = 2;
        }
        else if (i == 14)
        {
            dimension = 1;
        }
        numElements += recordDescriptor[i].numEntries * recordDescriptor[i].numCoefficient*dimension; // total number of double coefficients  
    }
    numElements += 2; // startTime and endTime
}


EphemerisRecord::RecordType  & EphemerisRecord::operator[](int const numRecord)
{
    return *getRecord(numRecord);
}


EphemerisRecord::RecordType * EphemerisRecord::readRecord(int const numRecord)
{
    if (numRecord >= 0)
    {
        //calculate stream position
        streampos newPosition = recordStart + numRecord * recordLength; // for record 0 (i.e. first read this is = recordStart
        if (newPosition != currentPosition)
        {
            jpleph.seekg(newPosition);
        }
        RecordType * newRecord = new RecordType(recordDescriptor);
        good = read(jpleph, *newRecord);
        if (good)
        {
            currentPosition = jpleph.tellg();

            // now make record useable for interpolation:
            // TODO: better structure in binary file
            // can we do something better with enum in C++11
            //			auto record = new vector < vector < vector< Vector3d * > *> *>;
            //			(*record).reserve(15);

            //			for (int i = 0; i < int(Entry::TT_TDB) + 1; ++i) // for all different objects
            //			{
            // vector<>  : number of objects
            // of vector<> : number of sub intervals
            //    of vector<> : order
            //                int const numSubintervals = recordDescriptor[i].numEntries;
            //                auto      subIntervals    = new vector < vector< Vector3d * > *>;
            //				(*subIntervals).reserve(numSubintervals);
            //					for (int j = 0; j < numSubintervals; ++j)  // number of subintervals
            //					{
            //                        int const polyOrder    = recordDescriptor[i].numCoefficient; // order of polynomials (+1)
            //                        auto      coefficients = new vector< Vector3d * >;
            //						(*coefficients).reserve(polyOrder);
            //						for (int k = 0; k < polyOrder; ++k) // order of the polynomial
            //						{
            //                            // components: most have 3 components, exceptions are nutation (11), and time (14).
            //                            const int recordIndex = recordDescriptor[i].recordIndex;
            //                            Vector3d * coeff = NULL;
            //							if (i != int(Entry::NUTATION) && i != int(Entry::TT_TDB))
            //							{
            //                                coeff = new Vector3d((*newRecord)[recordIndex + (3 * j)*polyOrder + k], (*newRecord)[recordIndex + (3 * j + 1)*polyOrder + k], (*newRecord)[recordIndex + (3 * j + 2)*polyOrder + k]);
            //							} else if (i == int(Entry::NUTATION)) // nutation 2 components in longitude and obliquity
            //							{
            //                                coeff = new Vector3d((*newRecord)[recordIndex + (3 * j)*polyOrder + k], (*newRecord)[recordIndex + (3 * j + 1)*polyOrder + k], 0.0);
            //							}
            //							else // object 14 1 component TT to TDB 
            //							{
            //                                coeff = new Vector3d((*newRecord)[recordIndex + (3 * j)*polyOrder + k], 0.0,  0.0);
            //							}

            //                            if (coeff != NULL)
            //                            {
            //                                (*coefficients).push_back(coeff);
            //                            }

            //						} // poynomial

            //						(*subIntervals).push_back(coefficients);
            //					} // subinterval


            //				(*record).push_back(subIntervals);
            //			} // object

            return newRecord; //TODO: this is the raw record for testing only
        }
    }
    good = false;
    return NULL;
    //throw
}




EphemerisRecord::RecordType::RecordType(std::vector<EphemerisRecord::RecordDescriptorEntry> const & descriptor): descriptor(descriptor)
{
}


EphemerisRecord::RecordDescriptorEntry const & EphemerisRecord::RecordType::getDescriptor(int const body) const
{
    return descriptor.at(body);
}




EphemerisRecord::RecordDescriptorEntry::RecordDescriptorEntry(int const index, int const order, int const entries)
{
    recordIndex     = index;
    numCoefficient  = order;
    numEntries      = entries;
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


EphemerisRecord::RecordDescriptorEntry EphemerisRecord::getDescriptorEntry(const int body)
{
    //todo : range checks
    return recordDescriptor.at(body);
}

bool EphemerisRecord::read(std::ifstream & jpleph, EphemerisRecord::RecordType & values)
{
    return ::read(jpleph, static_cast<std::vector<double> &>(values));
}