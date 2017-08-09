#pragma once
// a struct that describes the position of an object. It is actually just a tripple of doubles. The precise interpretation
// depends on its use! Beware e.g. of ecliptic vs. equatorial.
struct Position
{
    double x;  // right ascension, longitude, x
    double y;  // declination, latitude, y
    double z;  // distance, z
};
