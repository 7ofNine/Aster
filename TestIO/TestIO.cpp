//
// tests for AterLib IO functionality
//

#include <fstream>
#include "AstrometricObservations.h"
#include "Angle.h"
#include "ErrorModel.h"
#include "Observatories.h"

int main(void)
{
    Observatories observatories;
    std::ifstream obsfile("lib\\OBSCODE.dat");
    observatories.load(obsfile, ObservatoryType::Standard);

    ErrorModel errorModel(ErrorModel::MODEL_CBM10);
    // create observations list
    AstrometricObservations testObservations("1862.obs");

    // test RA conversion
    AngleRA testRA;
    long double value = testRA("18", "0", "0");
}