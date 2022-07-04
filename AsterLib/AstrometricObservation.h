//
//
// a single astrometric observation
//
// TODO: remove the name and handle it globally
//
#pragma once
#include <string>

#include "Angle.h"

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

    //enum ObservationTechnology  // the observation technology used MPC column 15
    //{
    //    TECHNOLOGY_NONE = 0,
    //    TECHNOLOGY_PHOTOGRAPHIC = 1,
    //    TECHNOLOGY_ENCODER = 2,
    //    TECHNOLOGY_CCD = 3,
    //    TECHNOLOGY_TRANSIT = 4,
    //    TECHNOLOGY_MICROMETER = 5,
    //    TECHNOLOGY_CORRECTED = 6,   // corrected for center of mass (radar only)
    //    TECHNOLOGY_SURFACE = 7,     // surface bounce (radar only) 
    //};

    enum ParallaxUnit    // unit used for parallax 
    {
        PARALLAX_NONE = 0,
        PARALLAX_KM   = 1,   // kilometer
        PARALLAX_AU   = 2,   // astronomical unit 
    };
    
    class ObserverCode {
        std::string obsCode;
        ObservationType type;
    public:  
        ObserverCode() = default;
        ObserverCode(const std::string code, ObservationType type = ObservationType::TYPE_NONE) :obsCode(code), type(type) {};
        ~ObserverCode() = default;
        std::string code() { return obsCode; };
    };

    class CatalogCode {
        const std::string code;
       
    public:
        CatalogCode() = default;
        CatalogCode(const std::string code) :code(code) {};
        std::string catalog() {};
    };

    class Magnitude {
        unsigned int selectd = 0; // magnitud selection type: 0: not selected, 1: used in least square fit
        bool valid = false;  // not all entries have a magnitude
        char color = '?';    // color band of magnitude
        long double value;   // numerical value of magnitude.
        long double rms; // rms value calculated
        bool residualDefined = false; // true if residual has been calculated
        long double residual; // residual of 
        long double chi;      // sqrt of chi^2 of residuals (probably for the coordinates
        long double accuracy; // accuracy given in raw data
    };


    class RightAscensionData {
        AngleRA rightAscension;
    };

    class DeclinationData {
        AngleDec declination;
    };

    class Coordinates {
        RightAscensionData  rightAscension; // TODO: change type to include precission RMS etc?
        DeclinationData    declination;     // TODO: change type to include precission RMS etc? 
    };



    class ObservationTime {                               // Observation date/time in MJD. For MPC data it is in UTC. Do we need it in other systems? use SOFA for translations

        long double timeUTC;
        long double timeTT;     // former Terestial Dynamical Time is now TT

        long double accuracy;
        
    public:
        ObservationTime(int year, int month, int day, int hours, int minutes, long double seconds, long double accuracy, std::string system = "UTC");

    };

public:
    AstrometricObservation(void) {};
    ~AstrometricObservation(void);

    int decode(std::string const& recoerd); // transform file record into an observation record

private:
    std::string getIAUDesignation(const std::string& identifier);
    bool getProvisionalYear(const std::string& provisional, std::string& year);
    std::string getCycle(const std::string& cycle);

    ObservationTime getDateOfObservation(const std::string& date);
    std::string getObservationTechnology(const std::string& data);
    std::string getPublishingNote(std::string const & data);
    std::string protect(std::string data);

    ObserverCode getObserverCode(const std::string& observatory);


private:

    std::string designation;
    std::string  technology;
    ParallaxUnit parUnit;
    std::string  publishingNote;
    CatalogCode  catCodeMPC; // MPC catalog code
    ObserverCode obsCode;    // observatory code
    ObservationTime time;    // MJD
    Magnitude    magnitude;
    //Acuuracy     accuracyCoor;   // probably intgerated into coordinates
    //Selection    selectionCoor;
    //Selection    selectionMag;







};

