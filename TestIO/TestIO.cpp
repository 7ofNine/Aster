//
// tests for AterLib IO functionality
//
#include "AstrometricObservations.h"
#include "Angle.h"
#include "ErrorModel.h"

int main(void)
{
    ErrorModel errorModel(ErrorModel::MODEL_CBM10);
    // create observations list
    AstrometricObservations testObservations("1862.obs");

    // test RA conversion
    AngleRA testRA;
    long double value = testRA("18", "0", "0");
}