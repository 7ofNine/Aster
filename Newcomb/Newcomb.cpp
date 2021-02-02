#include <cmath>
#include <functional>
#include "Newcomb.h"
#include "TrigTable.h"

namespace sofa // put sofa in a namespace
{
    #include "sofa.h"
}


// epoche of the data is 1900, Jan 0 Greenwich mean noon. This corresponds to 2,415,020.0
namespace
{
    static double const EPOCH_1900 = 2415020.5;  //TODO: this is probably wrong 1900 January 0 Noon GMT = 1899 December 31 noon 
    static double const BESSEL_EPOCH_1850 = 2396758.203;  // The besselian year 1850.0
    static double const JULIAN_YEAR = 365.25;    // days in Julian year (mean solar days)
    static double const JULIAN_CENTURY = 36525;  // days in a julian century

    // For some values Newcomb doesn't give explicit  formulas but calculltes tables. We use Leverriers values as he often refers to Leverrier.
    static double const PARIS_LONGITUDE = -( 9 * 60 + 20.91);   // in seconds longitude of paris Cassini meridian is 9m 20.91s east of Greenwich (AE 1978)
    static double const EPOCH_LEVERRIER = 2396759 + PARIS_LONGITUDE/86400.0; // Leverrier 1850 Jan 1 mean noon at Paris



    void term(int const i1, int const i2, int const pow,
        double const dlc, double const dls,
        double const drc, double const drs,
        double const dbc, double const dbs,
        double & dl, double & dr, double & db, double & u, double & v,
        TrigTable const & t1, TrigTable const & t2, double const t)
    {
        if (pow == 0)
        {
            TrigTable::addTheorem(t1.cos(i1), t1.sin(i1), t2.cos(i2), t2.sin(i2), u, v);
        }
        else
        {
            u = u * t;
            v = v * t;
        }

        dl = dl + dlc * u + dls * v;
        dr = dr + drc * u + drs * v;
        db = db + dbc * u + dbs * v;
    }

}

using namespace std;
using namespace std::placeholders;

Horner const Newcomb::g   { (358.0*60.0 + 28.0)*60.0 + 33.0, 129596579.10, -0.54, -0.012}; // mean anomaly of Earth in arc seconds. APEA 6, p. 9
Horner const Newcomb::e   { 0.01675104, -0.0004180, -0.000000126 };                        // excentricity of earth orbit. APEA 6, p. 9 
Horner const Newcomb::eps { (23.0*60.0 + 27.0)*60.0 + 8.26,  -46.845, -0.0059, 0.00181 };  // obliquity of the ecliptic in arc seconds. APEA 6, p. 10

// mean anomalies used for the perturbations are not strict mean anomalies. They are adapted to minimize the error withing the 19th century. For details see APEA 6.
// values are in degrees
Horner const Newcomb::g1  { 248.07/360.0, 1494.7235/360.0 };      // mercury's "mean anomaly" 
Horner const Newcomb::g2  { 114.50/360.0,  585.17493/360.0};      // Venus' "mean anomal"
Horner const Newcomb::g4  { 109.856/360.0, 191.39977/360.0};      // Mars' " mean anomaly"
Horner const Newcomb::g5  { 148.031/360.0,  30.34583/360.0};      // Jupiter's mean anomaly
Horner const Newcomb::g6  { 284.716/360.0,  12.21794/360.0};      // Saturn's mean anmaly

Newcomb::Newcomb()
{
}

Newcomb::~Newcomb()
{
}

//void Newcomb::term(int const i1, int const i2, int const pow, 
//          double const dlc, double const dls,
//          double const drc, double const drs,
//          double const dbc, double const dbs,
//          double & dl, double & dr, double & db, double & u, double & v,
//          TrigTable const & t1, TrigTable const & t2, double const t)
//{
//    if (pow == 0)
//    {
//        TrigTable::addTheorem(t1.cos(i1), t1.sin(i1), t2.cos(i2), t2.sin(i2), u, v);
//    } else
//    {
//        u = u*t;
//        v = v*t;
//    }
//
//    dl = dl + dlc*u + dls*v;
//    dr = dr + drc*u + drs*v;
//    db = db + dbc*u + dbs*v;
//}

// t given in julian day number
Position Newcomb::earth(double const t)
{   
    double dummy;
    double const T = (t - EPOCH_1900)/JULIAN_CENTURY;
    double const tp =(t - BESSEL_EPOCH_1850)/JULIAN_YEAR;

    TrigTable const earth  (-10, 12, g(T));
    TrigTable const mercury( -1, -1, D2PI*modf(g1(tp), &dummy));
    TrigTable const venus(  -10, -1, D2PI*modf(g2(tp), &dummy));
    TrigTable const mars(     1, 17, D2PI*modf(g4(tp), &dummy));
    TrigTable const jupiter(  1,  5, D2PI*modf(g5(tp), &dummy));
    TrigTable const saturn (  1,  4, D2PI*modf(g6(tp), &dummy));

    double dl = 0.0; // perturbation in longitude
    double dr = 0.0; // perturbation in radius vector
    double db = 0.0; // perturbation in latitude

    double u;
    double v;

    auto termMercury = bind(term, _1, _2, _3, _4, _5, _6 , _7, _8, _9, ref(dl), ref(dr), ref(db), ref(u), ref(v), ref(mercury), ref(earth), T); 
    // perturbation by Mercury (in 1000" of arc and ? digit of logarithm of radius vector
    termMercury( -1,  1, 0, -6, -11, 26, -12, 0, 0);
    termMercury( -1,  2, 0, -3,  -3, -4,   5, 0, 0);
    termMercury( -1,  3, 0, 15,  -1, -1, -18, 0, 0);
    termMercury( -1,  4, 0, 19, -13, -2,  -4, 0, 0);

    auto termVenus = bind(term, _1, _2, _3, _4, _5, _6, _7, _8, _9, ref(dl), ref(dr), ref(db), ref(u), ref(v), ref(venus), ref(earth), T);
    // perturbation by Venus (in 1000" of arc and ? digit of logarihtm of radius vector
    termVenus( -1, 0, 0,   33,   -67 ,  -85,   -39,  -24,  17);
    termVenus( -1, 1, 0, 2353, -4228, -2062, -1146,    4,  -3);
    termVenus( -1, 2, 0,  -65,   -34,    68,   -14,   -6,  92);
    termVenus( -1, 3, 0,   -3,    -8,    14,    -8,   -1,  -7);

    termVenus( -2, 0, 0,   -3,     1,     0,     4,    0,   0);
    termVenus( -2, 1, 0,  -99,    60,    84,   136,  -23,   3);
    termVenus( -2, 2, 0,-4702,  2903,  3593,  5822,  -10,   6);
    termVenus( -2, 3, 0, 1795, -1737,  -596,  -632,  -37,  56);
    termVenus( -2, 4, 0,   30,   -33,    40 ,   33,   -5,  13);

    termVenus( -3, 2, 0,  -13,     1,     0,    21,  -13,  -5);
    termVenus( -3, 3, 0, -666,    27,    44,  1044,   -8,  -1);
    termVenus( -3, 4, 0, 1508,  -397,  -381, -1448, -185, 100);
    termVenus( -3, 5, 0,  763,  -684,   126,   148,   -6,   3);
    termVenus( -3, 6, 0,   12,   -12,    14,    13,    2,  -4);

    termVenus( -4, 3, 0,   -3,    -1,     0,     6,   -4,  -5);
    termVenus( -4, 4, 0, -188,   -93,  -166,   337,    0,   0);
    termVenus( -4, 5, 0, -139,   -38,   -51,   189,   31,   1);
    termVenus( -4, 6, 0,  146,   -42,   -25,   -91,  -12,   0);
    termVenus( -4, 7, 0,    5,    -4,     3,     5,    0,   0);

    termVenus( -5, 5, 0,  -47,   -69,  -134,    93,    0,   0);
    termVenus( -5, 6, 0,  -28,   -25,   -39,    43,    8,   4);
    termVenus( -5, 7, 0, -119,   -33,   -37,   136,   18,   6);
    termVenus( -5, 8, 0,  154,    -1,     0,   -26,    0,   0);

    termVenus( -6, 5, 0,    0,     0,     0,     0,   2,   -6);
    termVenus( -6, 6, 0,   -4,   -38,   -80,     8,   0,    0);
    termVenus( -6, 7, 0,   -4,   -13,   -24,     7,   2,    3);
    termVenus( -6, 8, 0,   -6,    -7,   -10,    10,   2,    3);
    termVenus( -6, 9, 0,   14,     3,     3,   -12,   0,    0);

    termVenus( -7, 7, 0,    8,   -18,   -38,   -17,   0,    0);
    termVenus( -7, 8, 0,    1,    -6,   -12,    -3,   0,    0);
    termVenus( -7, 9, 0,    1,    -3,    -4,     3,   0,    0);
    termVenus( -7,10, 0,    0,     0,    -3,     3,   0,    0);

    termVenus( -8, 8, 0,    9,    -7,   -14,   -19,   0,    0);
    termVenus( -8, 9, 0,    0,     0,    -5,    -4,   0,    0);
    termVenus( -8,12, 0,   -8,   -41,   -43,     8,   5,    9);
    termVenus( -8,13, 0,    0,     0,    -9,    -8,   0,    0);
    termVenus( -8,14, 0,   21,    24,   -25,    22,   0,    0);

    termVenus( -9, 9, 0,    6,    -1,    -2,   -13,   0,    0);
    termVenus( -9,10, 0,    0,     0,    -1,    -4,   0,    0);

    termVenus(-10,10, 0,    3,     1,     3,    -7,   0,    0);

    auto termMars = bind(term, _1, _2, _3, _4, _5, _6, _7, _8, _9, ref(dl), ref(dr), ref(db), ref(u), ref(v), ref(mars), ref(earth), T);
    // perturbation by Mars (in 1000" of arc and ? digit of logarihtm of radius vector
    termMars( 1, -2, 0,    -5,    -4,    -5,     6, 0, 0);
    termMars( 1, -1, 0,  -216,  -167,   -92,   119, 0, 0);
    termMars( 1,  0, 0,    -8,   -47,   -27,    -6, 0, 0);

    termMars( 2, -3, 0,    40,   -10,   -13,   -50, 0, 0);
    termMars( 2, -2, 0,  1963,  -567,  -573, -1976, 0, 8);
    termMars( 2, -1, 0,  1659,  -617,    64,  -137, 0, 0);
    termMars( 2,  0, 0,   -24,    15,   -18,   -25, 8, -2);

    termMars( 3, -4, 0,     1,    -4,    -6,     0, 0, 0);
    termMars( 3, -3, 0,    53,  -118,  -154,   -67, 0, 0);
    termMars( 3, -2, 0,   396,  -153,   -77,  -201, 0, 0);
    termMars( 3, -1, 0,     8,     1,     0,     6, 0, 0);

    termMars( 4, -4, 0,    11,    32,    46,   -17, 0, 0);
    termMars( 4, -3, 0,   131,   483,   461,   125, -7, -1);
    termMars( 4, -2, 0,   526,  -256,    43,    96, 0, 0);
    termMars( 4, -1, 0,     7,    -5,     6,     8, 0, 0);

    termMars( 5, -5, 0,    -7,     1,     0,    12, 0, 0);
    termMars( 5, -4, 0,    49,    69,    87,   -62, 0, 0);
    termMars( 5, -3, 0,   -38,   200,    87,    17, 0, 0);
    termMars( 5, -2, 0,     3,     1,    -1,     3, 0, 0);

    termMars( 6, -6, 0,     0,     0,    -4,    -3, 0, 0);
    termMars( 6, -5, 0,   -20,    -2,    -3,    30, 0, 0);
    termMars( 6, -4, 0,  -104,   -113, -102,    94, 0, 0);
    termMars( 6, -3, 0,   -11,    100,  -27,    -4, 0, 0);

    termMars( 7, -6, 0,     3,     -5,   -9,    -5, 0, 0);
    termMars( 7, -5, 0,   -49,      3,    4,    60, 0, 0);
    termMars( 7, -4, 0,   -78,    -72,  -26,    28, 0, 0);

    termMars( 8, -7, 0,     1,      3,    5,    -1, 0, 0);
    termMars( 8, -6, 0,     6,     -8,  -12,    -9, 0, 0);
    termMars( 8, -5, 0,    51,    -10,   -8,   -44, 0, 0);
    termMars( 8, -4, 0,   -17,    -12,    5,    -6, 0, 0);

    termMars( 9, -7, 0,     2,      3,    5,    -3, 0, 0);
    termMars( 9, -6, 0,    13,    -25,  -30,   -16, 0, 0);
    termMars( 9, -5, 0,    60,    -15,   -4,   -17, 0, 0);

    termMars(10, -7, 0,     2,      5,    7,    -3, 0, 0);
    termMars(10, -6, 0,    -7,     18,   14,     6, 0, 0);
    termMars(10, -5, 0,     5,     -2,    0,     0, 0, 0);

    termMars(11, -7, 0,     9,     15,   17,    10, 0, 0);
    termMars(11, -6, 0,   -12,     42,    8,     3, 0, 0);

    termMars(12, -7, 0,    -4,     -5,   -4,     3, 0, 0);

    termMars(13, -8, 0,   -13,     -1,   -1,    15, 0, 0);
    termMars(13, -7, 0,   -30,    -33,   -4,     3, 0, 0);

    termMars(15, -9, 0,    13,    -16,   -17,  -14, 0, 0);
    termMars(10, -8, 0,     0,      0,    -1,   -6, 0, 0);

    termMars(17,-10, 0,    -2,     -4,    -4,    2, 0, 0);
    termMars(17, -9, 0,   -10,     24,     0,    0, 0, 0);

    auto termJupiter = bind(term, _1, _2, _3, _4, _5, _6, _7, _8, _9, ref(dl), ref(dr), ref(db), ref(u), ref(v), ref(jupiter), ref(earth), T);
    // perturbation by Mars (in 1000" of arc and ? digit of logarihtm of radius vector
    termJupiter(1, -3, 0,    -3,    -1,    -1,    5, 0, 0);
    termJupiter(1, -2, 0,  -155,   -52,   -78,  193, -7, 0);
    termJupiter(1, -1, 0, -7208,    59,    56, 7067,   1, -17);
    termJupiter(1,  0, 0,  -307, -2582,   227,  -89, -16,   0);
    termJupiter(1,  1, 0,     8,   -73,    79,    9,  -1, -23);

    termJupiter(2, -3, 0,    11,    68,   102,  -17, 0, 0);
    termJupiter(2, -2, 0,   136,  2728,  4021, -203, 0, 0);
    termJupiter(2, -1, 0,  -537,  1518,  1376,  486, -13, -166);
    termJupiter(2,  0, 0,   -22,   -70,    -1,   -8, 0, 0);

    termJupiter(3, -4, 0,    -5,     2,     3,    8, 0, 0);
    termJupiter(3, -3, 0,  -162,    27,    43,  278, 0, 0);
    termJupiter(3, -2, 0,    71,   551,   796, -104, -6, 1);
    termJupiter(3, -1, 0,   -31,   208,   172,   26, -1, -18);

    termJupiter(4, -4, 0,    -3,   -16,    -29,   5, 0, 0);
    termJupiter(4, -3, 0,   -43,     9,     13,  73, 0, 0);
    termJupiter(4, -2, 0,    17,    78,    110, -24, 0, 0);
    termJupiter(4, -1, 0,    -1,    23,     17,   1, 0, 0);

    termJupiter(5, -5, 0,     0,     0,     -1,  -3, 0, 0);
    termJupiter(5, -4, 0,    -1,    -5,    -10,   2, 0, 0);
    termJupiter(5, -3, 0,    -7,     2,      3,  12, 0, 0);
    termJupiter(5, -2, 0,     3,     9,     13,  -4, 0, 0);


    auto termSaturn = bind(term, _1, _2, _3, _4, _5, _6, _7, _8, _9, ref(dl), ref(dr), ref(db), ref(u), ref(v), ref(saturn), ref(earth), T);

    termSaturn(1, -2, 0,   -3,   11,   15,  3,  0,  0);
    termSaturn(1, -1, 0,  -77,  412,  422, 79, -1, -6);
    termSaturn(1,  0, 0,   -3, -320,    8, -1,  0,  0);
    termSaturn(1,  1, 0,    0,   -8,    8,  0,  1, -6);

    termSaturn(2, -3, 0,    0,    0,   -3,  -1, 0, 0);
    termSaturn(2, -2, 0,   38, -101, -152, -57, 0, 0);
    termSaturn(2, -1, 0,   45, -103, -103, -44, 0, 0);
    termSaturn(2,  0, 0,    2,  -17,    0,   0, 0, 0);

    termSaturn(3, -2, 0,    7,  -20,  -30,  -11, 0, 0);
    termSaturn(3, -1, 0,    6,  -16,  -16,   -6, 0, 0);

    termSaturn(4, -2, 0,    1,   -3,   -4,   -1, 0, 0);


    return Position();

} 

Position Newcomb::neptun(double const t)
{
    return Position();
}
