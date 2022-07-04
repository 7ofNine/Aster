#include <string>
#include <fstream>
#include <sstream>
#include <set>

#include "boost\algorithm\string\trim_all.hpp"

#include "AstrometricObservation.h"
#include "Power10.h"
#include "sofa.h"
#include "sofam.h"


using namespace boost::algorithm;

namespace {

    // The format of MPC observation records is described here
    //
    //  https://minorplanetcenter.net/iau/info/TechInfo.html
    //
    //

    static constexpr int MPC_RECORD_SIZE = 80;
    // fortran columns are base 1, C++ strings are base 0
    constexpr int FORTRAN_BASE = 1;
    static constexpr int MPC_PUBLISHING_NOTE = 14 - FORTRAN_BASE;
    static constexpr int MPC_PUBLISHING_NOTE_LENGTH = 1;
    //observational technology/Type
    static constexpr int MPC_OBSERVATION_TECHNOLOGY = 15 - FORTRAN_BASE;
    static constexpr int MPC_OBSERVATION_TECHNOLOGY_END = 15 - FORTRAN_BASE;
    static constexpr int MPC_OBSERVATION_TECHNOLOGY_LENGTH = MPC_OBSERVATION_TECHNOLOGY_END - MPC_OBSERVATION_TECHNOLOGY + 1;
    //record identification. Can be a combination of a number (for a numbered asteroid) or a provisional/temporary designation
    static constexpr int MPC_IDENTIFIER = 1 - FORTRAN_BASE;
    static constexpr int MPC_IDENTIFIER_END = 12 - FORTRAN_BASE;
    static constexpr int MPC_IDENTIFIER_LENGTH = MPC_IDENTIFIER_END - MPC_IDENTIFIER + 1;
    //the MPC identifier for the observed object starts in the first column of the record i.e., we can use absolut and relative for 
    //the identifier string.
    //These are the identifiers for Asteroids
    static constexpr int MPC_NUMBER = 1 - FORTRAN_BASE;
    static constexpr int MPC_NUMBER_END = 5 - FORTRAN_BASE;
    static constexpr int MPC_NUMBER_LENGTH = MPC_NUMBER_END - MPC_NUMBER + 1;
    static constexpr int MPC_PROVISIONAL = 6 - FORTRAN_BASE;
    static constexpr int MPC_PROVISIONAL_END = 12 - FORTRAN_BASE;
    static constexpr int MPC_PROVISIONAL_LENGTH = MPC_PROVISIONAL_END - MPC_PROVISIONAL + 1;
    static constexpr int MPC_DISCOVERY = 13 - FORTRAN_BASE;
    static constexpr int MPC_DISCOVERY_END = 13 - FORTRAN_BASE;
    //date of observation: The UTC time of the mid point of the observation
    //the format is YYYY MM DD.dddddd 
    static constexpr int MPC_DATE = 16 - FORTRAN_BASE;
    static constexpr int MPC_DATE_END = 32 - FORTRAN_BASE;
    static constexpr int MPC_DATE_LENGTH = MPC_DATE_END - MPC_DATE + 1;
    // Observed right ascension in J2000.0
    // format: HH MM SS.ddd 
    static constexpr int MPC_RA = 33 - FORTRAN_BASE;
    static constexpr int MPC_RA_END = 44 - FORTRAN_BASE;
    static constexpr int MPC_RA_LENGTH = MPC_RA_END - MPC_RA + 1;
    // Observed declination in J2000.0 
    // format: sDD MM SS.dd
    static constexpr int MPC_DEC = 45 - FORTRAN_BASE;
    static constexpr int MPC_DEC_END = 56 - FORTRAN_BASE;
    static constexpr int MPC_DEC_LENGTH = MPC_DEC_END - MPC_DEC + 1;
    // Observatory code
    static constexpr int MPC_OBSERVATORY = 78 - FORTRAN_BASE;
    static constexpr int MPC_OBSERVATORY_END = 80 - FORTRAN_BASE;
    static constexpr int MPC_OBSERVATORY_LENGTH = MPC_OBSERVATION_TECHNOLOGY_END - MPC_OBSERVATORY + 1;

    // these ar relativ within the provisional designation string
    static const char MPC_SURVEY_MARK = 'S';
    static const int MPC_SURVEY = 3 - FORTRAN_BASE;
    static const int MPC_SURVEY_NUMBER = 4 - FORTRAN_BASE;
    static const int MPC_SURVEY_NUMBER_END = 7 - FORTRAN_BASE;
    static const int MPC_SURVEY_NUMBER_LENGTH = MPC_SURVEY_NUMBER_END - MPC_SURVEY_NUMBER + 1;
    static const int MPC_SURVEY_SRC = 1 - FORTRAN_BASE;
    static const int MPC_SURVEY_ID = 2 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_YEAR = 1 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_YEAR_END = 3 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_YEAR_LENGTH = MPC_PROVISIONAL_YEAR_END - MPC_PROVISIONAL_YEAR + 1;
    static const int MPC_PROVISIONAL_HALF_MONTH = 4 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_ORDER = 7 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_CYCLE = 5 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_CYCLE_END = 6 - FORTRAN_BASE;
    static const int MPC_PROVISIONAL_CYCLE_LENGTH = MPC_PROVISIONAL_CYCLE_END - MPC_PROVISIONAL_CYCLE + 1;

    static constexpr char MPC_1800_MARK = 'I'; // 1800
    static constexpr char MPC_1900_MARK = 'J'; // 1900
    static constexpr char MPC_2000_MARK = 'K'; // 2000 


    static std::set<std::string> validTechnology{ " ", "A", "P", "e", "C", "B", "T", "M", "V", "v", "R","r",
                                                  "S", "s", "c", "E", "O", "H", "N", "n", "D", "Z", "W", "w", 
                                                  "Q", "q", "T", "t", "X", "x"};
    static std::set<std::string> validNote{}; //TODO: setup codes
}


AstrometricObservation::~AstrometricObservation(void)
{}

//decode a file record into an observation record. This allows us to return error codes and other hints
int AstrometricObservation::decode(std::string const& record) {
    
    publishingNote = getPublishingNote(record.substr(MPC_PUBLISHING_NOTE, MPC_PUBLISHING_NOTE_LENGTH));
    technology = getObservationTechnology(record.substr(MPC_OBSERVATION_TECHNOLOGY, MPC_OBSERVATION_TECHNOLOGY_LENGTH)); // TODO: this may determine further structure
    designation = getIAUDesignation(record.substr(MPC_IDENTIFIER, MPC_IDENTIFIER_LENGTH));
    obsCode = getObserverCode(record.substr(MPC_OBSERVATORY, MPC_OBSERVATORY_LENGTH));
    time = getDateOfObservation(record.substr(MPC_DATE, MPC_DATE_LENGTH));

    //TODO: incomplete
    return 0;
    // get declination
    // get right ascension
  
}

std::string AstrometricObservation::getIAUDesignation(const std::string& identifier)  // TODO: should we preserve the temporary designation?? 
{

    // from: http://www.minorplanetcenter.net/iau/info/PackedDes.html
    // Permanent Designations
    // The permanent designation stored on the orbit and observations is stored in a 5-character packed format that saves space and makes sorting easier.
    // Minor Planets: If the minor-planet number is less than 100000, then the number is stored as a zero-padded right-justified string.
    // E.g., (3202) is stored as "03202", (50000) as "50000".
    // 
    // When the number is above 99999, the number MOD 10000 is stored in columns 2-5 of the string and the number DIV 10000 is represented by the letters A-Z
    // (if between 10 and 36, inclusive) or a-z (if between 37 and 62, inclusive). E.g., (100345) is represented as "A0345", (360017) as "a0017", and (203289) as "K3289". 
    // different formats for comets and natural satelites

    std::string designation;
    if (identifier.length() != MPC_IDENTIFIER_LENGTH)
    {
        throw std::out_of_range("Identifier length wrong");
    }
    else
    {
        // check for numbered asteroid
        std::string number = identifier.substr(MPC_NUMBER, MPC_NUMBER_LENGTH);
        trim_all(number);
        if (number.length() == MPC_NUMBER_LENGTH)
        {
            // it is a numbered asteroid
            if (isdigit(number[0]))   // check on encoded for >= 100000
            {
                // remove leading zeros : TODO: really necessary?
                designation = number.erase(0, number.find_first_not_of('0'));

            }
            else if (isalpha(number[0]))  // this works for ASCII. be aware of local
            {
                std::stringstream decoded;
                decoded << (number[0] - 'A' + 1) * 10000 << number.substr(1, MPC_NUMBER_LENGTH - 1); // works as long as we stay in capital characters I.e < 2600000 asteroids
                designation = decoded.str();
            }
            else
            {

                throw std::out_of_range("Invalid character in id");
            }
        }
        else
        {
            // from: http://www.minorplanetcenter.net/iau/info/PackedDes.html
            // Provisional Designations
            // The provisional designation stored on the orbit and observations is stored in a 7-character packed format that saves space and makes sorting easier. 
            // The first two digits of the year are packed into a single character in column 1 (I = 18, J = 19, K = 20). 
            // Columns 2-3 contain the last two digits of the year. Column 4 contains the half-month letter and column 7 contains the second letter. 
            // The cycle count (the number of times that the second letter has cycled through the alphabet) is coded in columns 5-6,
            // using a letter in column 5 when the cycle count is larger than 99. The uppercase letters are used, followed by the lowercase letters.
            // Examples:
            // J95X00A = 1995 XA
            // J95X01L = 1995 XL1
            // J95F13B = 1995 FB13
            // J98SA8Q = 1998 SQ108
            // J98SC7V = 1998 SV127
            // J98SG2S = 1998 SS162
            // K99AJ3Z = 2099 AZ193
            // K08Aa0A = 2008 AA360
            // K07Tf8A = 2007 TA418
            //
            // Survey designations of the form 2040 P-L, 3138 T-1, 1010 T-2 and 4101 T-3 are packed differently.
            // Columns 1-3 contain the code indicating the survey and columns 4-7 contain the number within the survey.
            // Examples:
            // 2040 P-L  = PLS2040
            // 3138 T-1  = T1S3138
            // 1010 T-2  = T2S1010
            // 4101 T-3  = T3S4101

            //it has a provisional id. Decompress it
            std::string provisionalYear;
            std::string provisional = identifier.substr(MPC_PROVISIONAL, MPC_PROVISIONAL_LENGTH);
            if (provisional[MPC_SURVEY] == MPC_SURVEY_MARK)
            {
                // a survey designation
                designation = provisional.substr(MPC_SURVEY_NUMBER, MPC_SURVEY_NUMBER_LENGTH) + provisional[MPC_SURVEY_SRC] + "-" + provisional[MPC_SURVEY_ID];
            }
            else if (getProvisionalYear(provisional.substr(MPC_PROVISIONAL_YEAR, MPC_PROVISIONAL_YEAR_LENGTH), provisionalYear))
            {
                std::string cycle = getCycle(provisional.substr(MPC_PROVISIONAL_CYCLE, MPC_PROVISIONAL_CYCLE_LENGTH));
                designation = provisionalYear + provisional[MPC_PROVISIONAL_HALF_MONTH] + provisional[MPC_PROVISIONAL_ORDER] + cycle;

            }
            else
            {
                throw std::out_of_range("Unknown provisional id format");//TODO: unknown format for designation
            }
        }
    }
    return designation;
}

bool AstrometricObservation::getProvisionalYear(const std::string& provisional, std::string& year)
{
    // the format is Ixx
    char centuryMark = provisional[0];
    if (centuryMark == MPC_2000_MARK || centuryMark == MPC_1900_MARK || centuryMark == MPC_1800_MARK)
    {
        int century = (centuryMark - MPC_1800_MARK) + 18;
        std::stringstream theYear;
        theYear << century << provisional.substr(1, 2);
        year = theYear.str();
        return true;
    }
    else
    {
        // we don't known about this century marker
        return false;
    }
}

std::string AstrometricObservation::getCycle(const std::string& cycle)
{
    // format of cycle: 00, 01, 42, A3, z9
    if (cycle == "00")
    {
        return "";

    }
    else if (cycle[0] == '0')
    {
        return cycle.substr(1, 1);

    }
    else if (isdigit(cycle[0]))
    {
        return cycle;
    }
    else {
        // we have an alphanumeric cycle
        int msc;
        if (isupper(cycle[0]))
        {
            msc = cycle[0] - 'A' + 10;    // A=10x, B=11x, ... , Z=35x
        }
        else
        {
            msc = cycle[0] - 'a' + 36;    // a=36x, ...
        }
        std::stringstream thisCycle;
        thisCycle << msc << cycle[1];
        return thisCycle.str();
    }
}

// DATE OF OBSERVATIONS
// Columns 16-32 contain the date and UTC time of the mid-point of observation.
// If the observation refers to one end of a trailed image, then the time of observation will be either the start time of the exposure or the finish time of the exposure.
// The format is "YYYY MM DD.dddddd", with the decimal day of observation normally being given to a precision of 0.00001 days.
// Where such precision is justified, there is the option of recording times to 0.000001 days. 
//
// we also determine the accuracy
AstrometricObservation::ObservationTime AstrometricObservation::getDateOfObservation(const std::string& date)
{
    static Power10 power10;
    std::stringstream thisDate;
    thisDate.str(date);

    int year;
    int month;
    std::string dayString;
    std::string fractionString;
    long double accuracy;
    int day;
    //long double secondsUTC;
    long double dayFraction;
    long double hoursFraction;
    long double minutesFraction;
    long double seconds;
    int hours;
    int minutes;

    thisDate >> year >> month >> dayString;

    size_t decimalIndex = dayString.find(".");
    if (decimalIndex == std::string::npos)
    {
        accuracy = 1.0;
        thisDate.str(dayString);
        thisDate.clear();
        thisDate >> day;
    }
    else {
        accuracy = power10(-(int(dayString.length()) - int(decimalIndex) - 1));
        fractionString = dayString.substr(decimalIndex); // keep the decimal point for scaling
        thisDate.str(fractionString);
        thisDate.clear();   // reset eof bit
        thisDate >> dayFraction;

        dayString = dayString.substr(0, decimalIndex);
        thisDate.str(dayString);
        thisDate.clear();   // reset eof bit
        thisDate >> day;
        hoursFraction = dayFraction * 24;
        hours = static_cast<int>(truncl(hoursFraction));
        minutesFraction = (hoursFraction - hours) * 60;
        minutes = static_cast<int>(truncl(minutesFraction));
        seconds = (minutesFraction - minutes) * 60;

    }
    ObservationTime time(year, month, day, hours, minutes, seconds, accuracy);
    return time;
// this is incomplete we only extract the strings. wee need to actually create the MJD
}

AstrometricObservation::ObservationTime::ObservationTime(int const  year, int const month, int const day, int const hours, int const minutes, long double const seconds, long double const accuracy, std::string system):accuracy(accuracy)
{
    double u1{ 0.0 };  // always set to MJD zero point 2400000.5
    double u2{ 0.0 };  // the delta
    double ta1{ 0.0 };
    double ta2{ 0.0 };
    double tt1{ 0.0 };
    double tt2{ 0.0 };
    int error{ 0 };

    if (year < 1972) { // UT1 was used /?/
        error = iauDtf2d((year < 1972)? "UT1":"UTC", year, month, day, hours, minutes, seconds, &u1, &u2);   // translate to Julian date 
    }
    else {
        error = iauDtf2d("UTC", year, month, day, hours, minutes, seconds, &u1, &u2);   // watch out for leap seconds, translate to Julian date 
    }
    
    if (error != 0) {// this is a C interface, something went wrong
        throw std::out_of_range("Failed UT1/UTC to MJD transformation.");
    }  
    timeUTC = u2 + u1 - DJM0;
    // translate to TAI
    error = iauUtctai(u1, u2, &ta1, &ta2);
    if (error != 0) {
        throw std::out_of_range("Failed MJD to TAI transformation");
    }
    // translate to TT
    error = iauTaitt(ta1, ta2, &tt1, &tt2);
    if (error != 0) {
        throw std::out_of_range("Failed TAI to TT transformation");
    }
    timeTT = tt2 + tt1 - DJM0;  // possibly smarter. Precission!

}

//
// Codes for observational technologies and other hints
// see https://minorplanetcenter.net/iau/info/OpticalObs.html
// Note 2
//
// A   Converted to J2000.0
// P   Photographic(default if column is blank)
// e   Encoder
// C   CCD
// B   CMOS
// T   Meridian or transit circle
// M   Micrometer
// V / v  "Roving Observer" observation
// R / r  Radar observation
// S / s  Satellite observation
// c   Corrected - without - republication CCD observation[MUST NOT be used on observation submissions]
// E   Occultation - derived observations
// O   Offset observations(used only for observations of natural satellites)
// H   Hipparcos geocentric observations
// N   Normal place
// n   Mini - normal place derived from averaging observations from video frames
//
// D   CCD observation converted from original XML - formatted submission[MUST NOT be used on observation submissions]
// Z   Photographic observation converted from original XML - formatted submission[MUST NOT be used on observation submissions]
// W / w  "Roving observer" observation converted from original XML - formatted submission[MUST NOT be used on observation submissions]
// Q / q  Radar observation converted from original XML - formatted submission[MUST NOT be used on observation submissions]
// T / t  Satellite observation converted from original XML - formatted submission[MUST NOT be used on observation submissions]
// X/x used only for already - filed observations. 'X' was given originally only to discovery observations that were approximate or semi - accurate
//     and that had accurate measures corresponding to the time of discovery : this has been extended to other replaced discovery observations.
//     Observations marked 'X' / 'x' are to be suppressed in residual blocks.They are retained so that there exists an original record of a discovery.These codes MUST NOT be used on observation submissions.

std::string AstrometricObservation::getObservationTechnology(std::string const& data)
{
    std::string technology_temp; // should actually report an error
    static std::string photographic{ "P" };

    technology_temp = protect(data);
    if (validTechnology.contains(technology_temp)) {
        if (technology_temp == " ") {
            return photographic;
        }
        return technology_temp;
    }
    return technology_temp; // TODO: should report error!!
}

// Protect from stray characters in single character string (seems to happen occasionally)
std::string AstrometricObservation::protect(std::string data){
    std::string empty{ " " };
    if (data == "'" || data == "\"" || data == "\\") {
        return empty;
    }
    return data;
}



std::string AstrometricObservation::getPublishingNote(std::string const& data) {
    std::string note; // should actually report an error
    
    note = protect(data);
    if (validNote.contains(note)) {
        return note;
    }
    return note; // TODO: should report error!!

}

AstrometricObservation::ObserverCode AstrometricObservation::getObserverCode(const std::string& observer)
{
    
    return ObserverCode(observer);
}

