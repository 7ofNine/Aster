#pragma once
// 
// implementation of Necombs Theorie of the bodies of the Solar system up to Neptune and the obliquity of the ecliptic
// as given in the "Astronomical Papers Prepared for the use of the American Ehpemeries and Nautical Almanac"
//
//  Volume VI 1898: Tables of the Motion of the earth on its axis and arround the Sun p. 7 ff.
//                  Tables of Heliocentric Motion of Mercury p.171
//                  Tables of the Heliocentric Motion of Venus p.271
//                  Tables of the Heliocentric Motion of Mars  p. 383.
//
// One purpose is not just to calculate a position of a solar body but also to be able to create the tables as they are given in the document above.
// This is for historical purposes only.
//

// Time Parameter: UTC in Gregorian Calendar. Internally transformed to TT(or ephemeris time) TDB which is the independent time parameter of the ephemeris
// Beware of the modern time systems in comparison to when these ephemeries were created by Necomb (ca. 1890-1900)

#include "Horner.h"
#include "Position.h"
#include "TrigTable.h"

namespace
{
//    static const double D
}

class Newcomb
{
public:
    Newcomb();
    ~Newcomb();

    Position earth(double const t); // get the position of the Earth
    Position mercury(double const t); // get the position of Mercury
    Position venus(double const t); // get the position of Venus
    Position mars(double const t); // get the position of Mars
    Position jupiter(double const t); // get te position of Jupiter
    Position saturn(double const t); // get the position of Saturn
    Position uranus(double const t); // get the position of Uranus
    Position neptun(double const t); // get the position of Neptun

private:
  //  void term(int const i1, int const i2, int const pow, double const dlc, double const dls, double const drc, double const drs, double const dbc, double const dbs, double & dl, double & dr, double & db, double & u, double & v, TrigTable const & t1, TrigTable const & t2, double const t);
    static Horner const g;  // mean anomaly of Earth
    static Horner const e;  // excentricity of Earth orbit
    static Horner const eps;// mean obliquity of ecliptic
    static Horner const g1; // "mean anomaly" Mercury
    static Horner const g2; // "mean anomaly" Venus
    static Horner const g4; // "mean anomaly" Mars
    static Horner const g5; // "mean anomaly" Jupiter
    static Horner const g6; // "mean anomlay" Saturn
};

