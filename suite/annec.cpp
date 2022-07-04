#include <cmath>
#include <iostream>
#include <numbers>

#include "suite.h"


namespace orbfit::suite {

    ///////////////////////////////////////////////////////////////////////////////////
    // 
    //  annec
    // 
    //  Solve elliptic Kepler equation by iteration
    //
    //  Input:  mean            double  the mean anomaly (radians)
    //          eccentricity    double  the eccentricity of the ellipse (0 <= ecc < 1) 
    //  Return: the eccentric anomaly 
    //
    ///////////////////////////////////////////////////////////////////////////////////
    double annec(double const mean, double const ecc)
    {
        static double constexpr  eps = 1.0E-14;
        static double constexpr  nmax = 50;

        double eccold = mean;
        double eccnew = 0.0;
        int i = 0;
        while (i < nmax)
        {
            eccnew = eccold - ecc * (std::sin(eccold) - mean) / (1.0 - ecc * cos(eccold));

            if (std::abs(eccnew - ecc * std::sin(eccnew) - mean) <= eps)   // this checks how close we are and does not rely on the delta from old to new
            {
                return eccnew;
            }
            i++;
        }

        if (i >= nmax)
        {
            std::cerr << "/n/r" << "suit::orbfit::suit::anecc did not converge: Mean anomaly : " << mean << "eccentriciy : " << ecc << std::flush;
            //should we throw exception??? 
        }

    }
}