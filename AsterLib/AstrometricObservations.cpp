

#include <iostream>
#include <sstream>
#include <cctype>

#include "boost\algorithm\string\trim_all.hpp"
#include "AstrometricObservations.h"
#include "Power10.h"
#include "Angle.h"



using namespace std;
using namespace boost::algorithm;

namespace {

static const int MPC_RECORD_SIZE = 80;
// fortran columns are base 1, C++ strings are base 0
static const int FORTRAN_BASE                 = 1;
//observational technology
static const int MPC_OBSERVATION_TECHNOLOGY   = 15 - FORTRAN_BASE;
static const int MPC_OBSERVATION_TECHNOLOGY_END     = 15 - FORTRAN_BASE;
static const int MPC_OBSERVATION_TECHNOLOGY_LENGTH  = MPC_OBSERVATION_TECHNOLOGY_END - MPC_OBSERVATION_TECHNOLOGY + 1;
//record identification. Can be a combination of a number (for a numbered asteroid) or a provisional/temporary designation
static const int MPC_IDENTIFIER = 1 - FORTRAN_BASE;
static const int MPC_IDENTIFIER_END = 12 -FORTRAN_BASE;
static const int MPC_IDENTIFIER_LENGTH = MPC_IDENTIFIER_END - MPC_IDENTIFIER + 1;
//the MPC identifier for the observed object starts in the first column of the record i.e., we can use absolut and relative for 
//the identifier string.
//These are the identifiers for Asteroids
static const int MPC_NUMBER = 1 - FORTRAN_BASE;
static const int MPC_NUMBER_END = 5 - FORTRAN_BASE;
static const int MPC_NUMBER_LENGTH = MPC_NUMBER_END - MPC_NUMBER + 1;
static const int MPC_PROVISIONAL = 6 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_END = 12 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_LENGTH = MPC_PROVISIONAL_END - MPC_PROVISIONAL + 1;
static const int MPC_DISCOVERY     = 13 - FORTRAN_BASE;
static const int MPC_DISCOVERY_END = 13 - FORTRAN_BASE;
//date of observation: The UTC time of the mid point of the observation
//the format is YYYY MM DD.dddddd 
static const int MPC_DATE = 16 - FORTRAN_BASE;
static const int MPC_DATE_END = 32 - FORTRAN_BASE; 
static const int MPC_DATE_LENGTH = MPC_DATE_END - MPC_DATE + 1;
// Observed right ascension in J2000.0
// format: HH MM SS.ddd 
static const int MPC_RA     = 33 - FORTRAN_BASE;
static const int MPC_RA_END = 44 - FORTRAN_BASE;
static const int MPC_RA_LENGTH = MPC_RA_END - MPC_RA + 1;
// Observed declination in J2000.0 
// format: sDD MM SS.dd
static const int MPC_DEC    = 45 - FORTRAN_BASE;
static const int MPC_DEC_END = 56 - FORTRAN_BASE;
static const int MPC_DEC_LENGTH = MPC_DEC_END - MPC_DEC + 1;
// Observatory code
static const int MPC_OBSERVATORY = 78 - FORTRAN_BASE;
static const int MPC_OBSERVATORY_END = 80 - FORTRAN_BASE;
static const int MPC_OBSERVATORY_LENGTH = MPC_OBSERVATION_TECHNOLOGY_END - MPC_OBSERVATORY + 1;

// these ar relativ within the provisional designation string
static const char MPC_SURVEY_MARK = 'S';
static const int MPC_SURVEY = 3 - FORTRAN_BASE;
static const int MPC_SURVEY_NUMBER = 4 - FORTRAN_BASE;
static const int MPC_SURVEY_NUMBER_END = 7 - FORTRAN_BASE;
static const int MPC_SURVEY_NUMBER_LENGTH = MPC_SURVEY_NUMBER_END - MPC_SURVEY_NUMBER + 1;
static const int MPC_SURVEY_SRC = 1 - FORTRAN_BASE; 
static const int MPC_SURVEY_ID  = 2 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_YEAR = 1 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_YEAR_END = 3 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_YEAR_LENGTH = MPC_PROVISIONAL_YEAR_END - MPC_PROVISIONAL_YEAR + 1;
static const int MPC_PROVISIONAL_HALF_MONTH = 4 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_ORDER = 7 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_CYCLE = 5 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_CYCLE_END = 6 - FORTRAN_BASE;
static const int MPC_PROVISIONAL_CYCLE_LENGTH = MPC_PROVISIONAL_CYCLE_END - MPC_PROVISIONAL_CYCLE + 1;

static const char MPC_1800_MARK = 'I'; // 1800
static const char MPC_1900_MARK = 'J'; // 1900
static const char MPC_2000_MARK = 'K'; // 2000 

}    

AstrometricObservations::AstrometricObservations(string observationsFile)
{
    // TODO: first attempt in a first step we only support MPC input format files

    int line = 0; // keep track of the input record
    ifstream input(observationsFile);
    if(!input)
    {
        //TODO: trace+log + throw exception?
        cerr << "input file " + observationsFile + " not found" << endl;
    } else 
    {
        string mpcRecord;
        while(input)
        {
            getline(input, mpcRecord); // read a single record
            if(input && !mpcRecord.empty())
            {
                // retrieve single components
                if(!mpcRecord2Observation(mpcRecord))
                {
                    //TODO: log + trace error
                    // skip the line and continue reading hoping for the best
                    
                }
            }
        }
    }
    
}

AstrometricObservations::~AstrometricObservations(){}

bool AstrometricObservations::mpcRecord2Observation(string const & mpcRecord)
{
    // the records have Fortran formatting
    if(mpcRecord.size() == MPC_RECORD_SIZE)  
    {
        // retrieve single components
        string designation = getIAUDesignation(mpcRecord.substr(MPC_IDENTIFIER, MPC_IDENTIFIER_LENGTH));
        getDateOfObservation(mpcRecord.substr(MPC_DATE, MPC_DATE_LENGTH));
        getObservedDec(mpcRecord.substr(MPC_DEC, MPC_DEC_LENGTH));
        getObservedRA(mpcRecord.substr(MPC_RA, MPC_RA_LENGTH));
		getObservatoryCode(mpcRecord.substr(MPC_OBSERVATORY, MPC_OBSERVATORY_LENGTH));
        //TODO: actual handling
        cout << mpcRecord << endl;
        cout << designation << endl;

       return true;
    } else 
    {
        return false;
    }    

}


string AstrometricObservations::getIAUDesignation(const string & identifier)
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

    string designation;
    if(identifier.length() != MPC_IDENTIFIER_LENGTH)
    {
        //TODO: log/throw etc
    } else
    {
        // check for numbered asteroid
        string number = identifier.substr(MPC_NUMBER, MPC_NUMBER_LENGTH);
        trim_all(number);
        if(number.length() == MPC_NUMBER_LENGTH)
        {
            // it is a numbered asteroid
            if(isdigit(number[0]))   // check on encoded for >= 100000
            {
                // remove leading zeros : TODO: really necessary?
                designation = number.erase(0, number.find_first_not_of('0'));

            } else if(isalpha(number[0]))  // this works for ASCII. be aware of local
            {
                stringstream decoded;
                decoded << (number[0] -'A' + 1)*10000 << number.substr(1, MPC_NUMBER_LENGTH - 1); // works as long as we stay in capital characters I.e < 2600000 asteroids
                designation = decoded.str();
            } else 
            {
                //Todo: unexpected character
            }
        } else
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
            string provisionalYear;
            string provisional = identifier.substr(MPC_PROVISIONAL, MPC_PROVISIONAL_LENGTH);
            if(provisional[MPC_SURVEY] == MPC_SURVEY_MARK)
            {
                // a survey designation
                designation = provisional.substr(MPC_SURVEY_NUMBER, MPC_SURVEY_NUMBER_LENGTH) + provisional[MPC_SURVEY_SRC] + "-" + provisional[MPC_SURVEY_ID];    
            } else if (getProvisionalYear(provisional.substr(MPC_PROVISIONAL_YEAR, MPC_PROVISIONAL_YEAR_LENGTH), provisionalYear))
            {
                string cycle = getCycle(provisional.substr(MPC_PROVISIONAL_CYCLE, MPC_PROVISIONAL_CYCLE_LENGTH));
                designation  = provisionalYear + provisional[MPC_PROVISIONAL_HALF_MONTH] + provisional[MPC_PROVISIONAL_ORDER] + cycle;
                
            } else
            {
                //TODO: unknown format for designation
            }
        }
        
    }
    return designation;
}


bool AstrometricObservations::getProvisionalYear(const string & provisional, string & year)
{
    // the format is Ixx
    char centuryMark =  provisional[0];
    if(centuryMark == MPC_2000_MARK || centuryMark == MPC_1900_MARK || centuryMark == MPC_1800_MARK)
    {
        int century = (centuryMark - MPC_1800_MARK) + 18;
        stringstream theYear;
        theYear << century << provisional.substr(1, 2);
        year = theYear.str();
        return true;
    } else
    {
        // we don't known about this century marker
        return false;
    }
}

string AstrometricObservations::getCycle(const string & cycle)
{
    // format of cycle: 00, 01, 42, A3, z9
    if(cycle == "00")
    {
        return "";

    } else if (cycle[0] == '0')
    {
        return cycle.substr(1, 1);

    } else if (isdigit(cycle[0]))
    {
        return cycle;
    } else {
        // we have an alphanumeric cycle
        int msc;
        if(isupper(cycle[0]))
        {
            msc = cycle[0] - 'A' + 10;    // A=10x, B=11x, ... , Z=35x
        } else
        {
            msc = cycle[0] - 'a' + 36;    // a=36x, ...
        }
        stringstream thisCycle;
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
void AstrometricObservations::getDateOfObservation(const string & date)
{
	static Power10 power10;
    stringstream thisDate;
    thisDate.str(date);

    int year;
    int month;
    string dayString;
    string fractionString;
    long double accuracy;
    int day;
    long double secondsUTC;

    thisDate >> year >> month >> dayString;

    size_t decimalIndex = dayString.find(".");
    if(decimalIndex == string::npos)
    {
        accuracy = 1.0;
        thisDate.str(dayString);
        thisDate.clear();
        thisDate >> day;
    } else {
        accuracy = power10(-(int(dayString.length()) - int(decimalIndex) - 1));
        fractionString = dayString.substr(decimalIndex); // keep the decimal point for scaling
        thisDate.str(fractionString);
        thisDate.clear();   // reset eof bit
        thisDate >> secondsUTC;
        
        secondsUTC *= 86400;

        dayString = dayString.substr(0, decimalIndex);
        thisDate.str(dayString);
        thisDate.clear();   // reset eof bit
        thisDate >> day;
   
    }
}


void AstrometricObservations::getObservedDec(const string & declinationString)
{
	stringstream thisDeclination;
	thisDeclination.clear();
	thisDeclination.str(declinationString);
	string degrees;
	string minutes;
	string seconds;
	thisDeclination >> degrees >> minutes >> seconds;
	AngleDec declination;
		
	long double declinationValue = declination(degrees, minutes, seconds);

}

void AstrometricObservations::getObservedRA(const std::string & raString)
{
	stringstream thisRA;
	thisRA.clear();
	thisRA.str(raString);
	string hours;
	string minutes;
	string seconds;
	thisRA >> hours >> minutes >> seconds;
	AngleRA rightAscension;

	long double declinationValue = rightAscension(hours, minutes, seconds);

}

void AstrometricObservations::getObservatoryCode(const std::string & observatory)
{
	int observatoryId = 1;
}
