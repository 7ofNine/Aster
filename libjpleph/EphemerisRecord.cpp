
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
    vector<int> dimensions; // number of components/dimension of entries

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

    if (good)
    {
        good = ::read(jpleph, dimensions);
    }

    // they should all have the same size. TODO? put it differently into the binary file <vector<vector<int>>. Would make it possible to read it as one object
    good = (index.size() == order.size()) && (order.size() == subRecords.size() && (order.size() == dimensions.size()));
    recordDescriptor.reserve(index.size());
    for (size_t i = 0; i < index.size(); ++i)
    {
        recordDescriptor.push_back(RecordDescriptorEntry(index[i] - 1, order[i], subRecords[i], dimensions[i]));  // change Fortran index into C++ index (base 1 -> base 0)
    }

    for (size_t i = 0; i < recordDescriptor.size(); ++i)
    {
        numElements += recordDescriptor[i].numEntries * (recordDescriptor[i].numCoefficient)*(recordDescriptor[i].dimension); // total number of double coefficients  
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

            return newRecord; 
        }
    }
    good = false;
    throw invalid_argument("EphemerisRecord::RecordType::readRecord: invalid record number");
}




EphemerisRecord::RecordType::RecordType(std::vector<EphemerisRecord::RecordDescriptorEntry> const & descriptor): descriptor(descriptor)
{
}


EphemerisRecord::RecordDescriptorEntry const & EphemerisRecord::RecordType::getDescriptor(int const body) const
{
    return descriptor.at(body);
}




EphemerisRecord::RecordDescriptorEntry::RecordDescriptorEntry(int const index, int const order, int const entries, int const dim)
{
    recordIndex     = index;
    numCoefficient  = order;
    numEntries      = entries;
    dimension       = dim;
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

// cleanup chache and don't leak
EphemerisRecord::~EphemerisRecord()
{
    for (auto elem : keyToValue)
    {
        delete (elem.second.first);
    }
}