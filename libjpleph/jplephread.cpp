// the read template definition file
//
#include "jplephread.h"

template<>
bool read(std::ifstream & jpleph, std::string & value)
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
