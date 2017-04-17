//
// 
// Class providing fixed-size (by number of records) 
// LRU-replacement cache of a function with signature 
// V f(K). 
// 
#pragma once
#include <cassert>
#include <list>
#include <unordered_map>


template < typename K, typename V, typename W > 
class LRUCache 
{ 
public: 
 
  typedef K key_type; 
  typedef V value_type; 
 
  // Key access history, most recent at back 
  typedef std::list<key_type> key_tracker_type; 
 
  // Key to value and key history iterator 
  typedef std::unordered_map<key_type, std::pair< value_type, typename key_tracker_type::iterator > > key_to_value_type; 

  // Constuctor specifies the cached function and 
  // the maximum number of records to be stored 
  LRUCache(value_type (W::* f)(const key_type&), size_t c, W & host): fn(f), capacity(c), host(host) 
  { 
    assert(capacity != 0); 
  } 


  // Obtain value of the cached function for k 
  value_type operator()(const key_type& k)
  { 
    // Attempt to find existing record 
    const typename key_to_value_type::iterator it = keyToValue.find(k); 
 
    if (it == keyToValue.end())
    { 
        // We don't have it: 
        // Evaluate function and create new record 
        const value_type v = host.*host.fn(k); 
        insert(k,v); 
 
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


private:

  // Record a fresh key-value pair in the cache 
  void insert(const key_type& k, const value_type& v)
  { 
 
    // Method is only called on cache misses 
    assert(keyToValue.find(k) == keyToValue.end()); 
 
    // Make space if necessary 
    if (keyToValue.size() == capacity)
    { 
      evict(); 
    }
 
    // Record k as most-recently-used key 
    typename key_tracker_type::iterator it = keyTracker.insert(keyTracker.end(), k); 
 
    // Create the key-value entry, 
    // linked to the usage record. 
    keyToValue.insert(std::make_pair(k, std::make_pair(v, it) )); 

    // No need to check return, 
    // given previous assert. 
  } 

  // Purge the least-recently-used element in the cache 
  void evict()
  { 
    // Assert method is never called when cache is empty 
    assert(!key_tracker.empty()); 
 
    // Identify least recently used key 
    const typename key_to_value_type::iterator it = keyToValue.find(keyTracker.front()); 

    assert(it != keyToValue.end()); 
 
    // Erase both elements to completely purge record 
    keyToValue.erase(it); 
    keyTracker.pop_front(); 
  } 


  // The function to be cached 
  value_type (W::* fn)(const key_type&); 
  // the class it resides in
  W & host;
 
  // Maximum number of key-value pairs to be retained 
  const size_t capacity; 
 
  // Key access history 
  key_tracker_type keyTracker; 
 
  // Key-to-value lookup 
  key_to_value_type keyToValue; 
};