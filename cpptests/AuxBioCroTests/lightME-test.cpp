#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// lightME /////////////////

constexpr auto PI = 3.141592653589793238L;
constexpr auto RADIANS_PER_DEGREE = PI/180;
constexpr double DEGREES_PER_HOUR = 15;
constexpr int DAY_OF_YEAR_OF_VERNAL_EQUINOX = 365/4 - 10;
constexpr int DAY_OF_YEAR_OF_DECEMBER_SOLSTICE = 365 - 10;
constexpr int NOON_HOUR = 12;
constexpr double LATITUDE_OF_ARCTIC_CIRCLE = 66.5;
constexpr double AXIAL_TILT = 23.5;

// At the equator, during the equinox, when the sun is directly over the
// equator, we expect the zenith angle to equal the hour angle (so that their
// cosines are equal as well).  But lightME makes certain adjustments to
// cosine_zenith_angle in certain cases, so we have to adjust our expectations
// as well.
TEST(ZenithAngleCalculations, SpringEquinoxAtEquator) {

    // This relatively large tolerance is necessary since specifying the time of
    // the vernal equinox only to the nearest day gives a solar declination
    // insufficiently close to zero.
    constexpr double TOLERANCE = 1E-5;

    for (int hour_of_the_day = 6; hour_of_the_day <= 21; hour_of_the_day++) {

        double hour_angle = (hour_of_the_day - 12) * DEGREES_PER_HOUR;
        double hour_angle_in_radians = hour_angle * RADIANS_PER_DEGREE;
        double expected_cosine_zenith_angle = cos(hour_angle_in_radians);

        struct Light_model result = lightME(0, DAY_OF_YEAR_OF_VERNAL_EQUINOX,
                                            hour_of_the_day);
        EXPECT_NEAR(expected_cosine_zenith_angle,
                    result.cosine_zenith_angle,
                    TOLERANCE);
    }
}


// At solar noon, during the December solstice, we expect the zenith angle to be
// equal to the latitude plus the angle of the earth's tilt so that the cosine
// of the zenith angle is equal to the cosine of the sum of latitude and the
// angle of the earth's tilt.
TEST(ZenithAngleCalculations, NoonAtDecemberSolstice) {
    constexpr double TOLERANCE = 1E-15;
    for (int lat = -90; lat <= LATITUDE_OF_ARCTIC_CIRCLE; ++lat) {
        struct Light_model result = lightME(lat,
                                            DAY_OF_YEAR_OF_DECEMBER_SOLSTICE,
                                            NOON_HOUR);
        EXPECT_NEAR(result.cosine_zenith_angle,
                    cos((lat + AXIAL_TILT) * RADIANS_PER_DEGREE),
                    TOLERANCE);
    }
}

// At solar noon, during the equinox, we expect the zenith angle to be
// equal to the latitude so that the cosine of the zenith angle is
// equal to the cosine of the latitude.
TEST(ZenithAngleCalculations, NoonAtSpringEquinox) {
    // This relatively large tolerance is necessary since specifying the time of
    // the vernal equinox only to the nearest day gives a solar declination
    // insufficiently close to zero.
    constexpr double TOLERANCE = 0.0018;
    for (int lat = -90; lat <= 90; ++lat) {
        struct Light_model result = lightME(lat, DAY_OF_YEAR_OF_VERNAL_EQUINOX,
                                            NOON_HOUR);
        EXPECT_NEAR(result.cosine_zenith_angle, cos(lat * RADIANS_PER_DEGREE),
                    TOLERANCE);
    }
}

// For any latitude, during the equinox the adjusted value of CosZenithAngle
// will always be at most 1e-1 at hours 6 (sunrise) and 18 (sunset).
TEST(ZenithAngleCalculations, SunriseSunset) {
    constexpr double UPPER_LIMIT = 1E-1; // Upper limit when sun is near horizon

    srand(time(NULL));

    for (int i = 0; i < 1e6; ++i) {
        double lat = ((rand() % (int) 180e4) / 1e4) - 90;
        for (int td = 6; td <= 18; td += 12) {
            EXPECT_PRED_FORMAT2(::testing::DoubleLE,

                                lightME(lat,DAY_OF_YEAR_OF_VERNAL_EQUINOX, td)
                                .cosine_zenith_angle,

                                UPPER_LIMIT);
        }
    }
}


// Now we test irradiance.

TEST(IrradianceCalculations, SampleValues) {

    // DBL_DIG = 15, so 16 s.d. should be enough
    double expected_direct_irradiance[13] = {
        0.000000000000000e+00, // hour = 6
        2.892963275968324e-04,
        1.491902732207600e-01,
        4.870550993026899e-01,
        6.527225504827220e-01,
        7.169984283919291e-01,
        7.341560564732688e-01,
        7.169984283919291e-01,
        6.527225504827220e-01,
        4.870550993026899e-01,
        1.491902732207600e-01,
        2.892963275968324e-04,
        0.000000000000000e+00 // hour = 18

    };
    double expected_diffuse_irradiance[13] = {
        1.000000000000000e+00,
        9.997107036724032e-01,
        8.508097267792399e-01,
        5.129449006973101e-01,
        3.472774495172779e-01,
        2.830015716080709e-01,
        2.658439435267311e-01,
        2.830015716080709e-01,
        3.472774495172779e-01,
        5.129449006973101e-01,
        8.508097267792399e-01,
        9.997107036724032e-01,
        1.000000000000000e+00
    };

    for (int i = 0; i <= 12; ++i) {

        int td = i + 6; // ranges from 6 to 18

        struct Light_model lm = lightME(87, DAY_OF_YEAR_OF_VERNAL_EQUINOX, td);

        EXPECT_DOUBLE_EQ(lm.direct_irradiance_fraction, expected_direct_irradiance[i]);
        EXPECT_DOUBLE_EQ(lm.diffuse_irradiance_fraction, expected_diffuse_irradiance[i]);
    }
}

// The sum of the direct and the diffuse irradiance should always be 1.
TEST(IrradianceCalculations, DirectDiffuseSum) {
    srand(time(NULL));

    for (int i = 0; i < 1e6; ++i) {
        double lat = ((rand() % (int) 180e4) / 1e4) - 90;
        int DOY = rand() % 365;
        int td = rand() % 25;

        struct Light_model lm = lightME(lat, DOY, td);

        EXPECT_DOUBLE_EQ(lm.direct_irradiance_fraction + lm.diffuse_irradiance_fraction, 1.0);
    }
}

// Miscellaneous sample values
TEST(lightMETest, One) {
    struct Light_model result = lightME(45, 100, 15);
    EXPECT_DOUBLE_EQ (0.94682042635862518, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.053179573641374746, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.58750769610831033, result.cosine_zenith_angle);
}

TEST(lightMETest, Two) {
    struct Light_model result = lightME(45, 100, 12);
    EXPECT_DOUBLE_EQ (0.94866559021836816, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.051334409781631909, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.79286428947577225, result.cosine_zenith_angle);
}


TEST(lightMETest, Three) {
    struct Light_model result = lightME(45, 200, 12);
    EXPECT_DOUBLE_EQ (0.94933880654434322, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.050661193455656631, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.91294566229056384, result.cosine_zenith_angle);
}
