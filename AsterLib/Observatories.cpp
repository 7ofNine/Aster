//
// map for known Observatory codes
//
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "Observatories.h"


void Observatories::load(std::ifstream& input, const ObservatoryType type)
{
    int line = 0; // keep track of the input record
    
    if (!input)
    {
        //TODO: trace+log + throw exception?
        //// should never come here, Test already done before call
        ;
    }
    else
    {
        std::string observerRecord;
        while (input)
        {
            ++line;
            getline(input, observerRecord); // read a single record
            if (input && !observerRecord.empty()&&!observerRecord.starts_with('#'))
            {
                std::istringstream stream(observerRecord);
                std::string code; // w: 3 code of observatory 
                long double longitude; // w: 9 terrestial longitude
                long double x;  // w: 8 rho0*cos(phi)
                long double y;  // w: 9 rho0*sin(phi)
                std::string name; // name of obervatory

                stream >> std::setw(3) >> code >> std::setw(9) >> longitude >> std::setw(8)
                    >> x >> std::setw(9) >> y >> name;

                Observatory* observatory = new Observatory(code, longitude, x, y, name, type); // TODO: Observatory type has to be filled in according to file used
                // retrieve single components
                    //TODO: log + trace error
                    // skip the line and continue reading hoping for the best
                bool ok = insert(std::make_pair(observatory->code(), observatory)).second;
                if (!ok) {
                    //report dupliate entry
                }
            }
        }
    }
}

//Observatory::Observatory(std::string const & code, long double const & longitude, long double const & x, long double const & y, std::string const & name, ObservatoryType const type) {
