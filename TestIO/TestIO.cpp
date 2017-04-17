//
// tests for AterLib IO functionality
//
#include "AstrometricObservations.h"
#include "Angle.h"

int main(void)
{
    // create observations list
    AstrometricObservations testObservations("1862.obs");

    // test RA conversion
    AngleRA testRA;
    long double value = testRA("18", "0", "0");
}