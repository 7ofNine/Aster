//                                                                       
//      ASC2EPH creates a binary format JPL Planetary Ephemeris file from
//      one or more ascii text files.                                    
//                                                                       
//$ Disclaimer                                                           
//                                                                       
//!     THIS SOFTWARE AND ANY RELATED MATERIALS WERE CREATED BY THE       
//!     CALIFORNIA INSTITUTE OF TECHNOLOGY (CALTECH) UNDER A U.S.         
//!     GOVERNMENT CONTRACT WITH THE NATIONAL AERONAUTICS AND SPACE       
//!     ADMINISTRATION (NASA). THE SOFTWARE IS TECHNOLOGY AND SOFTWARE    
//!     PUBLICLY AVAILABLE UNDER U.S. EXPORT LAWS AND IS PROVIDED "AS-IS" 
//!     TO THE RECIPIENT WITHOUT WARRANTY OF ANY KIND, INCLUDING ANY      
//!     WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR FITNESS FOR A     
//!     PARTICULAR USE OR PURPOSE (AS SET FORTH IN UNITED STATES UCC      
//!     SECTIONS 2312-2313) OR FOR ANY PURPOSE WHATSOEVER, FOR THE        
//!     SOFTWARE AND RELATED MATERIALS, HOWEVER USED.                     
//!                                                                       
//!     IN NO EVENT SHALL CALTECH, ITS JET PROPULSION LABORATORY, OR NASA 
//!     BE LIABLE FOR ANY DAMAGES AND/OR COSTS, INCLUDING, BUT NOT        
//!     LIMITED TO, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND,      
//!     INCLUDING ECONOMIC DAMAGE OR INJURY TO PROPERTY AND LOST PROFITS, 
//!     REGARDLESS OF WHETHER CALTECH, JPL, OR NASA BE ADVISED, HAVE      
//!     REASON TO KNOW, OR, IN FACT, SHALL KNOW OF THE POSSIBILITY.       
//!                                                                       
//!     RECIPIENT BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF   
//!     THE SOFTWARE AND ANY RELATED MATERIALS, AND AGREES TO INDEMNIFY   
//!     CALTECH AND NASA FOR ALL THIRD-PARTY CLAIMS RESULTING FROM THE    
//!     ACTIONS OF RECIPIENT IN THE USE OF THE SOFTWARE.                  
//!                                                                       
//!                                                                       
//!      This program, 'asc2eph', requires (via standard input) an ascii  
//!      header file ('header.XXX'), followed by one or more ascii ephemer
//!      data files ('ascSYYYY.XXX').  All files must have the same epheme
//!      number, XXX.  Further, the data files must be consecutive in time
//!      with no gaps between them.                                       
//!                                                                       
//!      By default, the output ephemeris will span the same interval as t
//!      text file(s).  If you are interested in only a portion of data, s
//!      below T1 and T2 to the begin and end times of the span you desire
//!      and T2 must be specified in  Julian Ephemeris Days (ET).         
//!                                                                       
//!      A sample sequence of files might be:                             
//!                                                                       
//!        header.405  asc+1920.405 asc+1940.405 asc+1960.405 asc+1980.405
//!                                                                       
//!      This program is written in standard Fortran-77.                  
//!                                                                       
//! **********************************************************************
//!                                                                       
//!                                    *** NOTE ***                       
//!                                                                       
//!      However, the units in which the length of a direct access record 
//!      are PROCESSOR DEPENDENT.  The parameter NRECL, the number of unit
//!      controls the length of a record in the direct access ephemeris.  
//!      The user MUST select the correct value of NRECL by editing one of
//!      comemnted lines defining PARAMETER (NRECL) below.                
//!                                                                       
//! **********************************************************************
//!                                                                       
//!     Updated 02 March  2013 to accommodate more than 400 dynamical para
//!     Updated 02 August 2013 to accommodate reading of TT-TDB           
//!     Updated 15 August 2013 to accommodate negative Julian dates       
//!                                                                       
//! **********************************************************************
// 
// asc2eph is derived from the original asc2eph.f90
// but practically only the definition and meaning of the header files and the structure
// of the used ascii ephemeries files is used
// 

// INTERNAL FORMAT OF THE EPHEMERIS FILES
// --------------------------------------
// 
// On the first record of an export binary file or in the "GROUP 1050 of the ascii
// "header", there are 3 sets of 15 integers each.  (Older versions have only the first 13
// 	integers in each set)
// 
// 	The 15 triplets give information about the location, order and time - coverage of
// 	the chebychev polynomials corresponding to the following 15 items:
// 
// Mercury
// Venus
// Earth - Moon barycenter
// Mars
// Jupiter
// Saturn
// Uranus
// Neptune
// Pluto
// Moon(geocentric)
// Sun
// Earth Nutations in longitude and obliquity(IAU 1980 model)
// Lunar mantle libration
// Lunar mantle angular velocity
// TT - TDB(at geocenter)
// 
// Word(1, i) is the starting location in each data record of the chebychev
// coefficients belonging to the ith item.Word(2, i) is the number of chebychev
// coefficients per component of the ith item, and Word(3, i) is the number of
// complete sets of coefficients in each data record for the ith item.
// 
// Items not stored on the ascii files have 0 coefficients[Word(3, i)].
// 
// Data Records("GROUP 1070")
// 
// These records contain the actual ephemeris data in the form of chebychev
// polynomials.
// 
// The first two double precision words in each data record contain
// 
// Julian date of earliest data in record.
// Julian date of latest data in record.
// 
// The remaining data are chebychev position coefficients for each component of
// each body on the tape.The chebychev coefficients for the planets represent
// the solar system barycentric positions of the centers of the planetary systems.
// 
// There are three Cartesian components(x, y, z), for each of the items #1 - 11;
// there are two components for the 12th item, nutations : d(psi) and d(epsilon);
// there are three components for the 13th item, librations : phi, theta, psi;
// there are three components for the 14th item, mantle omega_x, omega_y, omega_z;
// there is one component for the 15th item, TT - TDB.
// 
// Planetary positions are stored in units of kilometers(TDB - compatible).
// The nutations and librations are stored in units of radians.
// The mantle angular velocities are stored in radians / day.
// TT - TDB is stored in seconds.

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cfloat>

//#include <cstdlib>

#include "optionparser.h"
#include "asc2eph.h"

namespace {

struct Arg: public option::Arg
{

    static void printError(std::string const & msg, option::Option const & option, std::string const & msg2)
    {
 //       std::fprintf(stderr, "%s", msg.c_str());
 //       std::fwrite(option.name, option.namelen, 1, stderr);
 //       std::fprintf(stderr , "%s", msg2.c_str());
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


    enum OptionIndex { UNKNOWN, OUTPUT, HEADER, FILES };
    const option::Descriptor usage[] =
    {
        {UNKNOWN, 0, "", "" , Arg::None, "USAGE: asc2eph -o output -h header -f file1 file2 ...\n\n"},
        {OUTPUT,  0, "o", "output", Arg::Required, "-o, --output   \t output binary ephemeris file"},
        {HEADER,  0, "h", "header", Arg::Required, "-h, --header   \t Ephemeris header file"},
        {FILES,   0, "f", "files",  Arg::Required, "-f, --files    \t ASCII ephemeris files"},
        {UNKNOWN, 0, "", "", Arg::None, "\nExamples:\n"
                                        "asc2eph -o jpleph -h header.431_572 -f ascp00000.431 ascp01000.431 ascp02000.431\n"},
        {0,0,0,0,0,0}
    };  
    
    
    static const std::string HEADER_RECORD_1010 = "GROUP   1010";  
    static const std::string HEADER_RECORD_1030 = "GROUP   1030";
    static const std::string HEADER_RECORD_1040 = "GROUP   1040";
    static const std::string HEADER_RECORD_1041 = "GROUP   1041";
    static const std::string HEADER_RECORD_1050 = "GROUP   1050";
    static const std::string HEADER_RECORD_1070 = "GROUP   1070";


    static const std::string CONSTANT_AU    = "AU";    //constant astronomical unit in km?
    static const std::string CONSTANT_EMRAT = "EMRAT"; // eart moon mass ratio
    static const std::string CONSTANT_DENUM = "DENUM"; //id of the DE (deleopment ephemeris)

    static const int NUM_BODIES = 15;                 // number of indices in i.e. "bodies" in ephemeries file
    static const std::size_t INDEX_START_DATE = 0;    // the first two entries in a data block
    static const std::size_t INDEX_END_DATE   = 1;    // are the entries for first and last date in the block
                                                      // the actual data start at index 2 (in Fortran = 3) 

    static const int OLDMAX = 400;                    // number of constants in file before DE430

                                               //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
    static std::vector<int> const dimensions = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3 ,3, 1 }; // these are fixed values depending on the type of values
                                                                                               // default is 3 except for Nutation (2) and TT-TDB (1) 


    bool gotoGroup(std::ifstream & headerStream, std::string const & groupName)
    {
        std::string line;
        while(getline(headerStream, line))
        {
            if(std::string::npos != line.find(groupName))
            {
                // skip the subsequent empty line
                if(getline(headerStream, line))
                {
                    return true;
                } else
                {
                    return false;
                }
            }
        }
         return false; // I/O stream is bad, didn't find the group we where looking for
    }

    void groupError(std::string const & groupName)
    {
        std::cerr << "Not a header file. " << groupName << " not found.\n" << std::flush;
    }

    void headerError(std::string const & headerFileName)
    {
        std::cerr << "Error reading header file " << headerFileName << std::endl << std::flush;    
    }

    long double f2cLongDouble(std::string & valueString)
    {
        // morph Fortran D into C++ E
        std::size_t indexD = valueString.find("D");
        if(indexD != std::string::npos)
        {
                valueString.replace(indexD, 1, "e");
        }
        // the final value
        return std::stold(valueString);
    }


    // look for block header
    bool readBlockHeader(std::ifstream & ephemeris, int & recordNumber,  int & numberCoeff)
    {
            ephemeris >> recordNumber >> numberCoeff;
            return ephemeris.good();
    }

    bool readDataBlock(std::ifstream & ephemeris, std::vector<long double> & db, int const numberCoeff)
    {
        for(int i = 0; i < numberCoeff; ++i)
        {
            std::string valueString;
            ephemeris >> valueString;
            if(!ephemeris.good())
            {
                return false;
            }
            db.push_back(f2cLongDouble(valueString));
        }
        return true;
    }


    // we write out our own format. This is not compatible with the original jpl format!!!
    // but hopefully makes more sense
    // writing binary data in C++ is for the pits
    bool writeHeader(std::ofstream & jpleph, std::vector<std::string> const & ttl, std::vector<std::string> const & constantNames,
                          std::vector<long double> const & constantValues,
                          long double const dateStart, long double const dateEnd, long double const dateInterval,
                          long double const au, long double const emrat, int const deNum,
                          std::vector<int> const & index,
                          std::vector<int> const & order,
                          std::vector<int> const & entries,
                          std::streampos & ssPosition)
    {
        if(!write(jpleph, ttl) ||                     // TTL
           !write(jpleph, constantNames) ||           //NCON + CNAM
           !write(jpleph, constantValues))            
        {
            return false;
        }
           
         // now determine location and save it for SS
        ssPosition = jpleph.tellp();
        if(!write(jpleph, dateStart) ||                 // SS to be rewritten later
           !write(jpleph, dateEnd) ||
           !write(jpleph, dateInterval) ||
           !write(jpleph, au) ||
           !write(jpleph, emrat) ||
           !write(jpleph, deNum) ||
           !write(jpleph, index) ||                    // size + IPT,LPT,RPT,TPT are all in three vectors
           !write(jpleph, order) ||
           !write(jpleph, entries) ||
           !write(jpleph, dimensions))                 // vector of dimensions. This is not in the header but defined here 
         {
            return false;
         }

        return true;
    }

    bool finalizeHeader(std::ofstream & jpleph, std::streampos const ssPosition, 
                        long double const dateStart, long double const dateEnd,
                        long double const dateInterval)
    {
        jpleph.seekp(ssPosition);
        if(!jpleph.good() ||
           !write(jpleph, dateStart) ||
           !write(jpleph, dateEnd) ||
           !write(jpleph, dateInterval)) 
        {
            return false;
        } else 
        { 
            return true;
        }
    }
}

using namespace std;



int main(int argc, char * argv[])
{
    // the time span covered can be selected by the user(not implemented yet)
    // by default the time span of the output file is the same as the 
    // time span of the provided input files
    long double tStart = - LDBL_MAX; // start date
    long double tEnd   =   LDBL_MAX; // end date
    long double db2z   = - LDBL_MAX; // the end date of the previously read data block
    long double dateStart = - LDBL_MAX;
    long double dateEnd = - LDBL_MAX;
    long double dateInterval = - LDBL_MAX;

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
        option::printUsage(std::cout, usage);
        return 0;
    }

    string headerFileName;
    string outputFileName;
    vector<string> inputFileNames;

    option::Option opt=options[HEADER];
    if(opt.count() > 0)
    {
        headerFileName = opt.arg;
        cout << "Header file: " << headerFileName << endl << flush ;
    } else 
    {
        cerr << "No header file given" << endl << flush;
        option::printUsage(cout, usage);
        return 0;
    }

    option::Option outOption = options[OUTPUT];
    if(outOption.count() > 0)
    {
        outputFileName = outOption.arg;
        cout << "Binary ephemeries file: " << outputFileName << endl << flush;
    } else
    {
        cerr << "No binary output file given" << endl << flush;
        option::printUsage(cout, usage);
        return 0;
    }

    if(options[FILES].count() > 0)
    {
        for(option::Option* opt = options[FILES].first(); opt!= 0; opt = opt->next())
        {
            string outputFileName = opt->arg;
            inputFileNames.push_back(outputFileName);
            cout << "ASCII ephemeris input files: " << outputFileName << endl << flush;
        } 
    } else
    {
        cerr << "No ASCII ephemeries file for input given" << endl << flush;
        option::printUsage(cout, usage);
        return 0;
    }


    // open header file
    ifstream headerStream(headerFileName);
    if(!headerStream.is_open())
    {
        cerr << "Could not open header file " << headerFileName << endl << flush;
        return 0; 
    } 

    // finger print
    cout << "\nJPL ASCII to direct I/O program. C++ version derived from original F90 version.\nCreated: 2016-01-24\n" << flush;
    
    // The header files have a specific structure that we have to parse for
    // They contain seperate GROUPs in a specific sequence. 
    // The GROUP  xxxx (xxxx an integer value) line is followed by an empty line and then the data start according to the group.
    // The group is terminated by an empty line
    //  
    // TODO: do we really need KSIZE or NCOEFF for a C++ program? C++ doesn't care about record lenghts. 
    // These are in the first line and not in a group
    // if needed then implement that here

    // read group 1010
    vector<string> ttl;
    if(!gotoGroup(headerStream, HEADER_RECORD_1010))
    {
        groupError(HEADER_RECORD_1010 );
        return 0;
    } else
    {
        // GROUP 1010 contains three lines for name and range of the original data. Just for information
        // not really used further along except as header marker for the binary file
        string line;
        for(int i = 0; i < 3; ++i)
        {
            if(getline(headerStream, line))
            {
                ttl.push_back(line);
                cout << line << endl;
            } else
            {
                headerError(headerFileName);
                return 0;
            }
        }
    }

    // read group 1030 . Start, end and record span (in days?)
    long double startDate;
    long double endDate;
    long double recordSpan;
    if(!gotoGroup(headerStream, HEADER_RECORD_1030))
    {
        groupError(HEADER_RECORD_1030 );
        return 0;
    } else
    {
        headerStream >> startDate >> endDate >> recordSpan;
        if(!headerStream.good())
        {
            headerError(headerFileName);
            return 0;    
        }
    }

    //read group 1040. Number and names of constants
    int numConstants; // number of named constants
    vector<string> constantNames;
    if(!gotoGroup(headerStream, HEADER_RECORD_1040))
    {
        groupError(HEADER_RECORD_1040);
        return 0;
    } else 
    {
        headerStream >> numConstants;
        if(!headerStream.good())
        {
            headerError(headerFileName);
            return 0;
        }
        for (int i = 0; i < numConstants; ++i)
        {
            string name;
            headerStream >> name;
            if(!headerStream.good())
            {
                headerError(headerFileName);
                return 0;
            }

            constantNames.push_back(name);
        }

    }

    //read group 1041. The actual values for the named constants
    int numValues; // number of named constants (should be same as numConstants?)
    vector<long double> constantValues;
    long double au;
    long double emrat;
    int numde;
    if(!gotoGroup(headerStream, HEADER_RECORD_1041))
    {
        groupError(HEADER_RECORD_1041);
        return 0;
    } else
    {
        headerStream >> numValues;
        if(!headerStream.good())
        {
            headerError(headerFileName);
            return 0;
        }

        for (int i = 0; i < numValues; ++i)
        {
            string valueString;
            headerStream >> valueString;
            if(!headerStream.good())
            {
                headerError(headerFileName);
                return 0;
            }

            long double value = f2cLongDouble(valueString);

            // retrieve values for au, emrat and the DE number
            if(constantNames[i] == CONSTANT_AU)
            {
                au = value;
            }
            if(constantNames[i] == CONSTANT_EMRAT)
            {
                emrat = value;
            }
            if(constantNames[i] == CONSTANT_DENUM)
            {
                numde = int(value);
            }

            constantValues.push_back(value);
        }
    }

    // now show the values on the screen
    for(int i = 0; i < numValues; ++i)
    {
        cout << setw(8) << constantNames[i] << ":  " << setw(24) << setprecision(16) << constantValues[i] << endl;
    }

    //read group 1050. The record descriptor
    //(GROUP 1070) the first line of the record contains a counter(should > 0) and the size of ther record entries of long double values
    //(GROUP 1070) first two entries in the record are the startdate and the enddate of the record
    //first row is the index into the record for "body"
    // second row is the number of the polynomial coefficients for the chebycheff polynomial i.e. the order
    // the third row is the number of subrecords for the body given by the index.
    vector<int> index;
    vector<int> order;
    vector<int> entries;         

    if(!gotoGroup(headerStream, HEADER_RECORD_1050))
    {
        groupError(HEADER_RECORD_1050);
        return 0;
    } else
    {
        // read record reference indices
        // what is what?
        // there are 15 entries
        // 1st line: index where record starts for body id index (Fortran starts at 1)
        int currentIndex;
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            headerStream >> currentIndex;
            if(!headerStream.good())
            {
                headerError(headerFileName);
                return 0;
            }
            index.push_back(currentIndex);
        }

        cout << endl;
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            cout << setw(5) << index[i];
        }
        cout << endl;


        // 2nd line is the order of the tchebycheff polynomial(?)
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            headerStream >> currentIndex;
            if(!headerStream.good())
            {
                headerError(headerFileName);
                return 0;
            }
            order.push_back(currentIndex);
        }
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            cout << setw(5) << order[i];
        }
        cout << endl;


        //3rd line is  number of entries for the rentry during the record period
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            headerStream >> currentIndex;
            if(!headerStream.good())
            {
                headerError(headerFileName);
                return 0;
            }
            entries.push_back(currentIndex);
        }
        for(int i = 0; i < NUM_BODIES; ++i)
        {
            cout << setw(5) << entries[i];
        }
        cout << endl;
    }


     //read group 1070. The actual entries
     //he header contains this for concatentation of the
     // actual data after the header but we read it from separate files, 
     // but require the proper structure of the header
     if(!gotoGroup(headerStream, HEADER_RECORD_1070))
     {
        groupError(HEADER_RECORD_1070);
        return 0;
     } 

     //  close the header file
     headerStream.close();


     // open output file as binary
     ofstream jpleph(outputFileName, ofstream::binary);
     // we have to add the original record 1 and 2 writing here
     // and remeber where the variable stuff is for rewriting at the end
     // random access stream!
     // write header record and get position for ss values
     streampos ssPosition;

     writeHeader(jpleph, ttl, constantNames, constantValues,
                 dateStart, dateEnd, dateInterval,   //SS values
                 au, emrat, numde,
                 index, order, entries,
                 ssPosition);

     int blockCounter = 0;

     blockCounter = 0; // counts the number of data blocks. The original SW also knows record numbers. They are of no use in C++

     if(!jpleph.good())
     {
        cerr << "Could not open binary output file " << outputFileName << endl;
        return 0;
     } 


     bool firstBlock = true; // for controling tracking of block time spans

     // start reading ASCII ephemeris files
    for(size_t j = 0; j < inputFileNames.size(); ++j)    // go through all the input data files
    {
        ifstream ephemeris(inputFileNames[j]);
        
        if(!ephemeris.good())
        {
            cerr << "Could not open input file " << inputFileNames[j] << endl;
            return 0;
        } else 
        {
            cout << "\nOpened input file " << inputFileNames[j] << endl << endl; 
        }
        
        int recordNumber = 0;
        int numberCoeff;
        while(recordNumber == 0)
        {
            if(!readBlockHeader(ephemeris, recordNumber,  numberCoeff))
            {
                cerr << "Error reading input file for block header" << inputFileNames[j] << endl;
                ephemeris.close();
                return 0;
            }
        }

        vector<long double> db;
        
        while(!ephemeris.eof())   // scan though all the data blocks in a single file
        {
            db.clear();
            db.reserve(numberCoeff);
            // we have to repeat this until we hit eof file and then have to continue to the next
            // file if it exists
            // read all coefficients
            if(!readDataBlock(ephemeris, db, numberCoeff))
            {
                cerr << "Error reading coefficients from " << inputFileNames[j] << endl;
                ephemeris.close(); 
                return 0;   
            }

            // TODO: this has to become more complex
            // handling first records, date ranges and changing input files
            // the first two entries in a data block are the start and end date of the block
            if(db[INDEX_END_DATE] < tEnd)
            {
                // not at the end date // todo but we could be reaching file end!
                // Skip this data block if the end of the interval is less       
                // than the specified start time or if it does not begin     
                // where the previous block ended.
                if (db[INDEX_END_DATE] >= tStart && db[INDEX_START_DATE] >= db2z)
                {
                    if(firstBlock)
                    {
                        db2z = db[INDEX_START_DATE];
                        firstBlock = false;
                    }

                    if(db[INDEX_START_DATE] != db2z)
                    {
                        cerr << "Data blocks do not overlap or abut" << endl;
                        return 0;
                    }

                    db2z = db[INDEX_END_DATE]; 

                    // keep time span for the binary ephemeris file
                    if(blockCounter == 0)
                    {
                        dateStart = db[INDEX_START_DATE];
                        dateInterval = db2z - dateStart;
                    }
                    dateEnd = db2z;

                    blockCounter++;
            
                    if(!write(jpleph, db)) // writing everything including start end end date of block
                    {
                        cerr << "Writing block" << blockCounter << " failed" << endl;
                        return 0;                            
                    }

                    // inform user about progess
                    if(blockCounter % 100 == 1)
                    {
                        if(db[INDEX_START_DATE] >= tStart)
                        {
                            cout << setw(5) << blockCounter << " Ephemeris records written. Last JED = " << setw(13) << fixed<< setprecision(2) << db[INDEX_END_DATE] << endl;
                        } else
                        {
                            cout << "Searching for first requested record ..." << endl;
                        }
                    }
                }

                //goto next block, we might hit EOF. then switch to next file
                readBlockHeader(ephemeris, recordNumber,  numberCoeff); 
            }
        }
        ephemeris.close();
    }

    // write info about last record
    cout << setw(5) << blockCounter << " Ephemeris records written. Last JED = " << setw(13) << fixed << setprecision(2) << db2z << endl;
    
    // finalize the header record with the ss values
    finalizeHeader(jpleph, ssPosition, dateStart, dateEnd, dateInterval);
    // close output file
    jpleph << flush;
    jpleph.close();

    cout << "\nOutput file " <<outputFileName << " written" << endl; 
}

