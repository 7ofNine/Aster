#pragma once

#include <fstream>
#include <string>

// templates for write methods

template <typename T>
bool write(std::ofstream & outStream,  T const & value)
{
    outStream.write((char *) & value, sizeof(T));
    return outStream.good();
}

// specialize for strings
template<>
bool write(std::ofstream & outStream, std::string const & value)   //TODO: how can we force ot to take a reference for strings but not for primitives Traits??
{
        outStream.write((char *) value.c_str(), value.size() + 1);
        return outStream.good();    
} 

template <typename T>
bool write(std::ofstream & outStream, std::vector<T> const & values)
{
    if(!write<std::vector<T>::size_type>(outStream, values.size())) // writing out the size of the vector
    {
        return false;
    }

    for(std::vector<T>::size_type i = 0 ; i < values.size(); ++i)
    {
        if(!write<T>(outStream, values[i]))
        {
            return false;
        }    
    }
    return true;
}
