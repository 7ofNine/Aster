
#include <sstream>
#include <stdexcept>
#include "Angle.h"
#include "Constants.h"
#include "Power10.h"


using namespace std;
using namespace fundamental;

//
// Angle  base class
//
Angle::Angle() :radians(false) {}
Angle::~Angle() {}

long double Angle::deg() { return valueDegrees; }

long double Angle::rad()
{ 
	if (radians)
	{
		return valueRadians;
	}else
	{
		valueRadians = valueDegrees * DEGREES_2_RADIANS;
		radians = true;
		return valueRadians;
	}
}

long double Angle::prec() { return precission; }




// return right ascension in decimal degrees
// if values outside of allowed ranges throw exception
long double AngleRA::operator()(string const & hours, const string & minutes, const string & seconds) 
{
	int numHours(0);
	int numMinutes(0);
	long double numSeconds(0.0);

	if (!hours.empty())
	{
		stringstream temp(hours);

		temp >> numHours;
		if (numHours < 0 || numHours > 23)
		{
			throw invalid_argument("hours invalid: " + hours);
		}
		precission = long double(SECONDS_PER_HOUR);
		if (!minutes.empty())
		{
			temp.clear();
			temp.str(minutes);

			temp >> numMinutes;
			if (numMinutes < 0 || numMinutes > 59)
			{
				throw invalid_argument("minutes invalid: " + minutes);
			}
			
			precission = SECONDS_PER_MINUTE;

			if (!seconds.empty())
			{
				temp.clear();
				temp.str(seconds);
				temp >> numSeconds;

				precission = determinePrecission(seconds);
			}
		}
	}

	valueDegrees = ((numSeconds * MINUTES_PER_SECOND + numMinutes) * HOURS_PER_MINUTE + numHours) * HOURS_2_DEGREES;
	return  valueDegrees;
}

string AngleRA::operator()(long double const degree)
{
    if(degree < 0.0 || degree >= 360.0)
    {
        throw invalid_argument("degrees invalid");
    }
    
    long double temp = degree/DEGREES_PER_HOUR;
    const int hours = int(temp); // get integer part
    // remainder and change to minutes
    temp = (temp - hours) * MINUTES_PER_HOUR;
    const int minutes = int(temp);
    //remainder and change to seconds
    const long double seconds = (temp - minutes) * SECONDS_PER_MINUTE;

    stringstream ra;
    // TODO formatted output !!
    ra << hours << " " << minutes << " " << seconds;
    return "";
}


// declination is constrained to be within -90 .... + 90
// this is equivalent to AngDeg but contsrained to -90 ... + 90 deggrees
long double AngleDec::operator()(string const & declination, const string & minutes, const string & seconds)
{
	if (!declination.empty())
	{
		int numDeclination(0);
		int numMinutes(0);
		long double numSeconds(0.0);

		
		stringstream temp(declination);
		temp >> numDeclination;
		precission = long double(ARCSECONDS_PER_DEGREE);
		if (numDeclination < -90 || numDeclination > 90)
		{
			// throw exception    
		}

		const bool southern = numDeclination < 0;
		if (!minutes.empty())
		{
			temp.clear();
			temp.str(minutes);

			temp >> numMinutes;
			if (numMinutes < 0 || numMinutes > 59)
			{
				// throw exception out of range
			}

			if ((numDeclination == -90 || numDeclination == 90) && numMinutes != 0)
			{
				// throw exception
			}
			precission = long double(ARCSECONDS_PER_MINUTE);

			if (!seconds.empty())
			{
				temp.clear();
				temp.str(seconds);
				temp >> numSeconds;
				if (numSeconds < 0.0 || numSeconds >= 60.0)
				{
					// throw exception
				}

				if ((numDeclination == -90 || numDeclination == 90) && numSeconds != 0.0)
				{
					// throw exception
				}
				precission = determinePrecission(seconds);
			}
		}

		if (southern)
		{
			valueDegrees = -(numSeconds * ARCMINUTES_PER_SECOND + numMinutes) * DEGREES_PER_MINUTE + numDeclination;
		}
		else
		{
			valueDegrees = (numSeconds * ARCMINUTES_PER_SECOND + numMinutes) * DEGREES_PER_MINUTE + numDeclination;
		}

		return valueDegrees;
	} else
	{
		//TODO: throw exception?
	}
}

string AngleDeg::operator()(long double const degree)
{
        return "";
}


// degrees are not constrained. We might want to normalize then into a specific range e.g. 0...360
long double AngleDeg::operator()(string const & declination, const string & minutes, const string & seconds)
{
    stringstream temp(declination);
    int numDeclination;
    temp >> numDeclination;

    const bool southern = numDeclination < 0;
    
    temp.clear();
    temp.str(minutes);
    int numMinutes;
    temp >> numMinutes;
    if(numMinutes < 0 || numMinutes > 59)
    {
        // throw exception out of range
    }

    temp.clear();
    temp.str(seconds);
    long double numSeconds;
    temp >> numSeconds;
    if(numSeconds < 0.0 || numSeconds >= 60.0)
    {
        // throw exception
    }
	
    if(southern)
    {
        return -(numSeconds * ARCMINUTES_PER_SECOND + numMinutes) * DEGREES_PER_MINUTE + numDeclination;
    } else
    {
        return  (numSeconds * ARCMINUTES_PER_SECOND + numMinutes) * DEGREES_PER_MINUTE + numDeclination;
    }
}


long double Angle::determinePrecission(const std::string & seconds)
{
	static Power10 power10;

	size_t decimalIndex = seconds.find(".");
	long double accuracy;
	if (decimalIndex == string::npos)
	{
		accuracy = 1.0;
	} else
	{
		accuracy = power10(-(int(seconds.length()) - int(decimalIndex) - 1));
	}

	return accuracy;
}

 