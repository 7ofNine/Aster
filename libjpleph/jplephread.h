// 
// templates for jpleph read methods
//
//
#pragma once

#include <fstream>
#include <vector>


// read of primitive types 
template<typename T> 
bool read(std::ifstream & jpleph, T & value)
{
    jpleph.read((char*) & value, sizeof(T));
    if(!jpleph.good())
    { 
        return false;
    }

    return true;
}


// specialized read for std::string
inline bool read(std::ifstream & jpleph, std::string & value)
{
    // we assume there are no strings longer than 255 char
    char buffer[255];
    jpleph.getline(buffer, 255, '\0');
    if(!jpleph.good())
    {
        return false;
    }
    
    value = std::string(buffer, (unsigned int)jpleph.gcount());
    return true;
}


template<typename T>
bool read(std::ifstream & jpleph, std::vector<T> & values)
{
    // first we read the size
    std::vector<T>::size_type size;
    if(!read(jpleph, size))
    {
        return false;
    }

    // just in case clear out the vector
    values.clear();
    // reserve the size and avoid realocations
    values.reserve(size);
    // now read the values
    for(std::vector<T>::size_type i = 0; i < size; ++i)
    {
        T value;
        jpleph.read((char*) & value, sizeof(T));
        if(!jpleph.good())
        {
            return false;
        }
        values.push_back(value);
    }


    return true;
}

// specialized template for reading a vector<std::string>

inline bool read(std::ifstream & jpleph, std::vector<std::string> & values)
{
    // first we read the size
    std::vector<std::string>::size_type size;
    if(!read(jpleph, size))
    {
        return false;
    }

    // just in case clear out the vector
    values.clear();
    // reserve the size and avoid realocations
    values.reserve(size);
    // now read the values
    for(std::vector<std::string>::size_type i = 0; i < size; ++i)
    {
        std::string value;
        read(jpleph, value);
        if(!jpleph.good())
        {
            return false;
        }
        values.push_back(value);
    }

    return true;
}
