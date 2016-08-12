#include "../src/BioCro.h"
#include "gtest/gtest.h"
#include <Rmath.h>


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
