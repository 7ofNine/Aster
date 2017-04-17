#pragma once

#include "boost/math/constants/constants.hpp"
// fundamental constants used that should be centrally defines
// these are mathematical as well as some physical constants.
// changing them here should change them consistently over the system
// don't use another place for defining them excpet if there is a very good reson for it

namespace fundamental
{

    const long double PI   = boost::math::long_double_constants::pi;
    const long double PI2  = boost::math::long_double_constants::two_pi; 


    //conversion values
    const int CIRCLE_DEGREES = 360;
    const int CIRCLE_HOURS   =  24;
    const int CIRCLE_GRADES  = 400;
    const int HOURS_PER_DAY  = 24;
    const int MINUTES_PER_HOUR = 60;
    const int SECONDS_PER_MINUTE = 60;
	const int SECONDS_PER_HOUR = MINUTES_PER_HOUR * SECONDS_PER_MINUTE;
    const long double MINUTES_PER_SECOND = 1.0/SECONDS_PER_MINUTE;
    const long double HOURS_PER_MINUTE   = 1.0/MINUTES_PER_HOUR;
    const int ARCMINUTES_PER_DEGREE = 60; //minutes of arc
    const int ARCSECONDS_PER_MINUTE = 60; //seconds of arc
	const int ARCSECONDS_PER_DEGREE = ARCSECONDS_PER_MINUTE*ARCMINUTES_PER_DEGREE; // 3600 arc seconds per degree;
    const long double ARCMINUTES_PER_SECOND = 1.0/ARCSECONDS_PER_MINUTE;
	
    const long double DEGREES_PER_MINUTE = 1.0/ARCMINUTES_PER_DEGREE;
    const int DEGREES_PER_HOUR = CIRCLE_DEGREES/CIRCLE_HOURS; /* 15 */

    const int SECONDS_PER_DAY = HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE; /*86400*/
    const int ARCSECONDS_PER_CIRCLE = CIRCLE_DEGREES * ARCMINUTES_PER_DEGREE * ARCSECONDS_PER_MINUTE;

    const long double HOURS_2_DEGREES = CIRCLE_DEGREES/CIRCLE_HOURS;

	const long double DEGREES_2_RADIANS = PI2 / CIRCLE_DEGREES;

}