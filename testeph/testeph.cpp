//
// test fro jpl ephemeis
//
//
#include <iostream>
#include <vector>
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

}


using namespace std;

int main(int argc, char * argv[])
{
//    // skip program name if present
//    if(argc > 0)
//    {
//        argc--;
//        argv++;
//    }
//
//    option::Stats stats(usage, argc, argv);
//    vector<option::Option> options(stats.options_max);
//    vector<option::Option> buffer(stats.buffer_max);
//    option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);
//   
//    if(parse.error())
//    {
//        return 1;
//    }
//   
//    if(argc == 0)
//    {
//        option::printUsage(cout, usage);
//        return 0;
//    }
//   
//    string jplephFileName;
//    string testControlFileName;
//   
//    option::Option opt=options[EPHEMERIS];
//    if(opt.count() > 0)
//    {
//        jplephFileName = opt.arg;
//        cout << "Ephemeris file: " << jplephFileName << endl;
//    } else 
//    {
//        cerr << "No ephemeris file given" << endl;
//        option::printUsage(cout, usage);
//        return 0;
//    }
//   
//    option::Option outOption = options[TESTFILE];
//    if(outOption.count() > 0)
//    {
//        testControlFileName = outOption.arg;
//        cout << "Test control file: " << testControlFileName << endl;
//    } else
//    {
//        cerr << "No test control file given" << endl;
//        option::printUsage(cout, usage);
//        return 0;
//    }


    Jpleph jpleph("jpleph"); // todo should be provided via program option

}


