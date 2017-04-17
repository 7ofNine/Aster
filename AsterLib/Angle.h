#pragma once

#include <string>

// represent the different formes of angles that are used
// maninly intended for easy transformation
//
// Angles can be given in the following units
//
// a) degress
// b) hours
// c) radians
// d) grades  (should we support this? only for old observations)
// 
// the values can be given
// i)  split into prime, minute, secondes....
// ii) as decimal values
//
// the can have the following representations:
//
// 1) numeric decimal
// 2) string

class Angle
{
public:
	Angle();
	virtual ~Angle();

	long double deg();
	long double rad();
	long double prec();


protected:
	long double determinePrecission(const std::string & seconds);
	long double valueDegrees;
	long double valueRadians;
	long double precission;
	bool radians;

};


class AngleRA : public Angle {

public:

    long double operator()(std::string const & hours, const std::string & minutes, const std::string & seconds);
    std::string operator()(long double const degrees);
};


class AngleDeg : public Angle
{

public:
//    AngleDec(std::string const & degrees, const std::string & minutes, const std::string seconds);
    long double operator()(std::string const & degrees, const std::string & minutes, const std::string & seconds);
    std::string operator()(long double const degrees);

};

class AngleDec : public Angle
{

public :
    long double operator()(std::string const & degrees, const std::string & minutes, const std::string & seconds);
    std::string operator()(long double const degrees);
};