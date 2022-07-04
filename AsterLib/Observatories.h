#pragma once

#include <unordered_map>
#include <fstream>
#include <string>
#include <tuple>

// The map of observatory codes and their data
//

enum class ObservatoryType {
    Standard  = 0,
    Radar     = 1,
    Test      = 2,
    Undefined
};

using ObsTuple = std::tuple<std::string, long double, long double, long double, std::string, ObservatoryType>;
class Observatory : public ObsTuple
{

public:
    Observatory(const std::string& code, const long double& longitude, const long double& x, const long double& y, const std::string& name, const ObservatoryType type):
        ObsTuple(code, longitude, x, y, name, type)
    {};

    std::string code() const {
        return std::get<0>(*this);
    }
    long double xcoord() {
        return std::get<1>(*this);
    };

    long double ycoord() {
        return std::get<2>(*this);
    };

    long double zcoord() {
        return std::get<3>(*this);
    };

    std::string name() {
        return std::get<4>(*this);
    }

    ObservatoryType type() {
        return std::get<5>(*this);
    }
};

struct ObservatoryComp {
    bool operator()(Observatory const& lhs, Observatory const& rhs)
    {
        return lhs.code() == rhs.code();
    }
};

class Observatories:public std::unordered_map<std::string, Observatory*>{
public:
    Observatories() = default;
    void load(std::ifstream & inStream, const ObservatoryType type = ObservatoryType::Undefined);
};

