#include "Newcomb.h"

Horner const Newcomb::g   { (358.0*60.0 + 28.0)*60.0 + 33.0, 129596579.10, -0.54, -0.012}; // mean anomaly of Earth in arc seconds. APEA 6, p. 9
Horner const Newcomb::e   { 0.01675104, -0.0004180, -0.000000126 };                        // excentricity of earth orbit. APEA 6, p. 9 
Horner const Newcomb::eps{ (23.0*60.0 + 27.0)*60.0 + 8.26,  -46.845, -0.0059, 0.00181 };   // obliquity of the ecliptic in arc seconds. APEA 6, p. 10

Newcomb::Newcomb()
{
}

Newcomb::~Newcomb()
{
}

Position Newcomb::earth(double const t)
{   
    return Position();

} 

Position Newcomb::neptun(double const t)
{
    return Position();
}
