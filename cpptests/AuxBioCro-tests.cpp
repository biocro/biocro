#include "../src/BioCro.h"
#include "gtest/gtest.h"
#include <cmath>


TEST(lightMETest, One) {
    struct Light_model result = lightME(45, 100, 15);
    EXPECT_DOUBLE_EQ (0.94682042635862518, result.irradiance_direct);
    EXPECT_DOUBLE_EQ (0.053179573641374746, result.irradiance_diffuse);
    EXPECT_DOUBLE_EQ (0.58750769610831033, result.cosine_zenith_angle);
}

TEST(lightMETest, Two) {
    struct Light_model result = lightME(45, 100, 12);
    EXPECT_DOUBLE_EQ (0.94866559021836816, result.irradiance_direct);
    EXPECT_DOUBLE_EQ (0.051334409781631909, result.irradiance_diffuse);
    EXPECT_DOUBLE_EQ (0.79286428947577225, result.cosine_zenith_angle);
}


TEST(lightMETest, Three) {
    struct Light_model result = lightME(45, 100, 12);
    EXPECT_DOUBLE_EQ (0.94866559021836816, result.irradiance_direct);
    EXPECT_DOUBLE_EQ (0.051334409781631909, result.irradiance_diffuse);
    EXPECT_DOUBLE_EQ (0.79286428947577225, result.cosine_zenith_angle);
}

TEST(zenith_angle_calculations, SpringEquinoxAtEquator) {
    for (int hour_of_the_day = 6; hour_of_the_day <= 18; hour_of_the_day++) {
        struct Light_model result = lightME(0, 365/4 - 10, hour_of_the_day);
        if (hour_of_the_day >= 19 && hour_of_the_day <= 21) {
            EXPECT_DOUBLE_EQ(0.10, result.cosine_zenith_angle);
        }
        else {
            EXPECT_NEAR(cos((hour_of_the_day - 12) * acos(-1)/12), result.cosine_zenith_angle, 0.0001);
        }
    }
}

