//
// test fro jpl ephemeis
//
//
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include "jpleph.h"

#include "optionparser.h"



namespace {

struct Arg: public option::Arg
{

    static void printError(std::string const & msg, option::Option const & option, std::string const & msg2)
    {
        std::cerr << msg << std::string(option.name) << msg2 << std::flush;
    }


    static option::ArgStatus Unknown(option::Option const & option, bool const msg)
    {
        if(msg)
        {
            printError("Unknown option '", option, "'\n");
        }
        return option::ARG_ILLEGAL;
    } 


    static option::ArgStatus Required(option::Option const & option, bool const msg)
    {
        if(option.arg != 0)
        {
            return option::ARG_OK;
        } else
        {
            if(msg)
            {
                printError("Option '", option, "' requires an argument\n");
            }
            return option::ARG_ILLEGAL;
        }
    }
};


    enum OptionIndex { UNKNOWN, EPHEMERIS, TESTFILE };
    const option::Descriptor usage[] =
    {
        {UNKNOWN, 0, "", "" , Arg::None, "USAGE: testeph -e ephemeries -t testfile\n\n"},
        {EPHEMERIS,  0, "e", "ephemries", Arg::Required, "-e, --ephemeries   \t input binary ephemeris file to be tested"},
        {TESTFILE,  0, "t", "testfile", Arg::Required, "-t, --testfile   \t test control file (ASCII)"},
        {UNKNOWN, 0, "", "", Arg::None, "\nExamples:\n"
                                        "testeph -e jpleph -t test432\n"},
        {0,0,0,0,0,0}
    };  


    static double const JDEPOC_DEFAULT     = 2440400.5;
    static std::string const JDEPOC_NAME   = "JDEPOC";
    static std::string const HEADER_MARKER = "EOT";
}


using namespace std;

bool skipToData(ifstream & contolFile);

int main(int argc, char * argv[])
{
    // write standard finger print
    cout << endl << "Test program for JPL DE ephemeries."  << endl 
         << "Derived from JPL testeph1.f from 2014-03-15." << endl << endl;
    // skip program name if present
    if(argc > 0)
    {
        argc--;
        argv++;
    }

    option::Stats stats(usage, argc, argv);
    vector<option::Option> options(stats.options_max);
    vector<option::Option> buffer(stats.buffer_max);
    option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);
   
    if(parse.error())
    {
        return 1;
    }
   
    if(argc == 0)
    {
        option::printUsage(cout, usage);
        return 0;
    }
   
    string jplephFileName;
    string testControlFileName;
   
    option::Option opt=options[EPHEMERIS];
    if(opt.count() > 0)
    {
        jplephFileName = opt.arg;
        cout << "Ephemeris file: " << jplephFileName << endl;
    } else 
    {
        cerr << "No ephemeris file given" << endl;
        option::printUsage(cout, usage);
        return 0;
    }
   
    option::Option outOption = options[TESTFILE];
    if(outOption.count() > 0)
    {
        testControlFileName = outOption.arg;
        cout << "Test control file: " << testControlFileName << endl;
    } else
    {
        cerr << "No test control file given" << endl;
        option::printUsage(cout, usage);
        return 0;
    }


    // initialise ephemeries
    Jpleph jpleph(jplephFileName); 

    // read constants and display them
    Jpleph::Constants constants;
    double dateStart;
    double dateEnd;
    double dateInterval;
    jpleph.constants(constants, dateStart, dateEnd, dateInterval);

    double jdepoc = JDEPOC_DEFAULT; // default epoch
    cout << showpoint << setw(14) << setprecision(14);
    cout << "Start date     : "  << dateStart    << endl;
    cout << "End date       : "  << dateEnd      << endl;
    cout << "Record interval: "  << dateInterval << endl;
    cout << endl;

    // now show the values on the screen
    for (size_t i = 0; i < constants.size(); ++i)
    {
        if (constants.at(i).name == JDEPOC_NAME)
        {
            jdepoc = constants.at(i).value;
        }
        cout << setw(8) << constants[i].name << ":  " << setw(24) << setprecision(16) << constants[i].value << endl;
    }

    // open test control file
    ifstream testInput(testControlFileName);
    if (!testInput.is_open())
    {
        cerr << endl << endl;
        cerr << "Could not open test control file " << testControlFileName << endl;
        return 0;
    }

    // skip test control file header
    if (!skipToData(testInput))
    {
        cerr << "End of test control file header (EOT) not found" << endl;
    }

    // setup output
    cout << endl;
    cout << "line -- jed --    t#   c#   x#    --- jpl value ---     --- user value --  -- difference --" << endl;


    // read a test case
    string de;
    string date;
    double tdb; 
    int target;
    int center;
    int component;
    double value;
    int line = 0;
    bool ok = true;
    double tdbmin =  DBL_MAX;
    double tdbmax = -DBL_MAX;
    int warnings = 0;

    while (!testInput.eof())
    {
        testInput >> de >> date >> tdb >> target >> center >> component >> value;
        if (!testInput.good()) // skip empty lines (at the end mainly)
        {
            continue;
        }
        if(tdb <= dateStart || tdb >= dateEnd)
        {
            continue;
        }
        Jpleph::Time time;
        time.t1 = tdb;
        Jpleph::Posvel posvel;
        jpleph.dpleph(time, Jpleph::Target(target), Jpleph::Target(center), posvel);

        // the comparison with expected result
        double del;
        // testeph and jpl uses a somewhat shifted and inhomogeneous component numbering
        // translate it to match our posvel structure
        if (target == 14) // nutation
        {
            if (component > 2)
            {
                component += 1;
            }
        }

        if (target == 17) // tt-tdb
        {
            if (component > 1)
            {
                component += 2;
            }
        }

        double calcValue = (component < 4) ? posvel.pos.at(component - 1) : posvel.vel.at(component - 4);
        del = fabs(calcValue - value);

        if (Jpleph::Target::LIBRATIONS == Jpleph::Target(target) && component == 3)
        {
            del = del / (1.0 + 100.0*fabs(tdb - jdepoc) / 365.25);
        }

        line++;

        if (tdb < tdbmin)
        {
            tdbmin = tdb;
        }
        if (tdb > tdbmax)
        {
            tdbmax = tdb;
        }

        cout << setw(5) << line
            << fixed << showpoint
            << setw(10) << setprecision(1) << tdb
            << setw(5) << target
            << setw(5) << center
            << setw(5) << component
            << showpoint
            << setw(21) << setprecision(13)
            << value
            << setw(22) << setprecision(13)
            << calcValue
            << scientific << setw(13) << setprecision(5)
            << del;

        if (del >= 1.0e-13)
        {
            ok = false;
            cout << "  *****  WARNING : difference >= 1.D-13  *****'" << endl;
            warnings++;
        }
        else
        {
            cout << endl;
        }
    }

    if (ok)
    {
        cout << endl;
        cout << "testeph checked successfully against ephemeris file " << endl;
        cout << "over Julian date range " << fixed << showpoint << setw(10) << setprecision(1)<< tdbmin 
             << " to " << setw(10) << setprecision(1) << tdbmax << endl;
    }
    else
    {
        cout << endl << "testeph found problems with ephemeris file" << endl;
        cout << warnings << " warnings in " << line << " lines. " << endl;
        cout << "Failurerate: " << fixed << setw(6) << setprecision(1) << (100.0*(float(warnings) / float(line))) << "%" << endl;

    }
}


bool skipToData(ifstream & controlFile)
{
    string line = "";
    while(getline(controlFile, line))
    {
        if (line.size() >= 3 && line.substr(0, 3) == HEADER_MARKER)
        {
            return true;
        }
    }

    return false;
}