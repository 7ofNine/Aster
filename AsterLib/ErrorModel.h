#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <fstream>
#include <functional>
class ErrorModel
{
public:
    explicit ErrorModel(std::string const & errorModel);
    ~ErrorModel();

    static std::string const MODEL_FCCT14;
    static std::string const MODEL_CBM10;

private:
    bool scanFile(std::ifstream & source, void (ErrorModel::* server)(std::string const &));
    void scanModelffct4(std::string const & line);
    void scanModelcbm10sta(std::string const & line);
    void scanModelcbm10(std::string const & line);
    // 0: rms(a*cosd), 1: rmsd, both in arcsec
    std::unordered_map<std::string, std::array<double, 2>> obscatRms;  // observatory/star catalog rms values (cmb19, fcct14)
    std::unordered_map<std::string, std::array<double, 2>> obsstatRms; // station rms without catalog (cmb10)
};

