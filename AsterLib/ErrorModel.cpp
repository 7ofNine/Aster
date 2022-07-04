#include <iostream>
#include <sstream>

#include "Configuration.h"
#include "ErrorModel.h"



namespace {
    // only fcct14 and cbm10 are defined

    static const std::string MODEL_FOLDER = "weights\\"; // relative folde where error model files are located
    static const std::string MODEL_STATION = ".sta";     // file extension for models of observatories(station) independent of catalog (only for cbm10)
    static const std::string MODEL_OBSERV = ".rules"; //  file extension for star catalog/observatory models
    
}

using namespace std;
std::string const ErrorModel::MODEL_FCCT14 = "fcct14";
std::string const ErrorModel::MODEL_CBM10 = "cbm10";

ErrorModel::ErrorModel(std::string const & errorModel)
{
    if (errorModel == MODEL_FCCT14 || errorModel == MODEL_CBM10)
    {
        try {
            string const modelPathBase = Config::LIBRARY_PATH + MODEL_FOLDER + errorModel;
            if (errorModel == MODEL_FCCT14)
            {   
                string modelFile = modelPathBase + MODEL_OBSERV;
                ifstream rmsFile(modelFile);
                scanFile(rmsFile, &ErrorModel::scanModelffct4);
                rmsFile.close();
            }
            else if (errorModel == MODEL_CBM10)
            {
                ifstream rmsFile(modelPathBase + MODEL_STATION);
                scanFile(rmsFile, &ErrorModel::scanModelcbm10sta);
                rmsFile.close();

                rmsFile.open(modelPathBase + MODEL_OBSERV);
                scanFile(rmsFile, &ErrorModel::scanModelcbm10);

                rmsFile.close();
            }
        }
        catch (invalid_argument & inva)
        {
            throw invalid_argument(inva.what() + string(" ") + errorModel);
        }
   }
        else
        {
            cerr << "ErrorModel::ErrorModel: Unknown error model " << errorModel;
            throw out_of_range("Unknown error model");
        }
}


ErrorModel::~ErrorModel(){}


bool ErrorModel::scanFile(ifstream & source, void (ErrorModel::* server)(std::string const &))
{
    if (source)
    {
        string line;
        while (getline(source, line))
        {
            if (source && !line.empty())
            {
                (this->*server)(line);
            }
        }
    }
    else
    {
        throw invalid_argument("Could not open error model");
        return false;
    }

    return true;
}


void ErrorModel::scanModelffct4(string const & line)
{
    stringstream sline;
    sline.clear();
    sline.str(line);
    string obscode;
    string catcode;
    string dc;
    double rmsa;
    double rmsd;

    sline >> obscode >> catcode >> dc >> rmsa >> dc >> rmsd;
    // remove c= from catcode
    catcode = catcode.substr(2);
    obscatRms.insert({ obscode + catcode, {rmsa, rmsd} });

}

void ErrorModel::scanModelcbm10sta(string const & line)
{
    stringstream sline;
    sline.clear();
    sline.str(line);
    string obscode;
    double rmsa;
    double rmsd;

    if (line.at(0) == '!') // check on first line (and or comments
    {
        return;
    }

    sline >> obscode >> rmsa >> rmsd;
    obscode += ":"; // add a colon 
    obsstatRms.insert({ obscode, {rmsa, rmsd} });
}

void ErrorModel::scanModelcbm10(string const & line)
{
    stringstream sline;
    sline.clear();
    sline.str(line);

    string obscode;
    string dc;
    double rmsa;
    double rmsd;

    if (line.at(0) == '!') // check on first line (and or comments
    {
        return;
    }
    sline >> obscode >> dc >> rmsa >> dc >> rmsd;
    obscatRms.insert({ obscode, { rmsa, rmsd } });
}

