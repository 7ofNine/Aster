#pragma once
//
// The container for a set of astronomic observations read from a given file.
// The file is in MPC format ::TODO suport other format (rwo format)
//
#include <string>
#include <vector>  //TODO: a hash is probably better for immedate sorting by time
//#include <fstream>

#include "AstrometricObservation.h"

class AstrometricObservations
{

public:
    AstrometricObservations(std::string observationsFile);
    ~AstrometricObservations(void);

private:
    void readMPCObservations(std::ifstream & instream);
    bool mpcRecord2Observation(std::string const & mpcRecord);
//    std::string getIAUDesignation(const std::string & identifier);
//    bool getProvisionalYear(const std::string & provisional, std::string & year);
//    std::string getCycle(const std::string & cycle);
//    void getDateOfObservation(const std::string & date);
//    long double power10(const int power);
	void getObservedDec(const std::string & declinationString);
	void getObservedRA(const std::string & raString);
	void getObservatoryCode(const std::string & observatory);

    // storage for the actual observations : TODO: change to a hash ot tree based on the observation time?
    std::vector<AstrometricObservation *> observations;     

};
