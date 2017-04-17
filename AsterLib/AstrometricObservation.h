//
//
// a single astrometric observation
//
// TODO: remove the name and handle it globally
//
#pragma once
#include <string>

// class representing an astrometrical observation as they are given by the MPC
// The MPC format is described here in detail http://www.minorplanetcenter.net/iau/info/ObsFormat.html
// these formats differ depending on the type of observation
//
class AstrometricObservation
{
 public:
    enum ObservationType  // the type of observation used
    {
        TYPE_NONE    = 0,
        TYPE_OPTICAL = 1,
        TYPE_RADAR_RANGE   = 2,
        TYPE_RADAR_RANGE_RATE = 3,
        TYPE_SATELLITE_SPACE  = 4,
    };

    enum ObservationTechnology  // the observation technology used MPC column 15
    {
        TECHNOLOGY_NONE = 0,
        TECHNOLOGY_PHOTOGRAPHIC = 1,
        TECHNOLOGY_ENCODER = 2,
        TECHNOLOGY_CCD = 3,
        TECHNOLOGY_TRANSIT = 4,
        TECHNOLOGY_MICROMETER = 5,
        TECHNOLOGY_CORRECTED = 6,   // corrected for center of mass (radar only)
        TECHNOLOGY_SURFACE = 7,     // surface bounce (radar only) 
    };

    enum ParallaxUnit    // unit used for parallax 
    {
        PARALLAX_NONE = 0,
        PARALLAX_KM   = 1,   // kilometer
        PARALLAX_AU   = 2,   // astronomical unit 
    };
    
/*    enum Catalog    // catalog usef for comparison stars  MPC column 14
    { 
        NONE = 0,   //TODO: is that really still used in the MPC ??
    };   
*/

public:
    AstrometricObservation(void);
    ~AstrometricObservation(void);

private:

    std::string designation;
    ObservationType type;




};

