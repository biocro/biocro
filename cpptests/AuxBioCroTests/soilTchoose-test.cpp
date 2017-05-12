#include <gtest/gtest.h> // test framework

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing

TEST(soilTchoose, AllValidValues) {

    for (int i = 0; i <= 10; ++i) {

        auto old_result = OldsoilTchoose(i);
        auto new_result = soilTchoose(i);



        EXPECT_EQ(old_result.silt, new_result.silt);
        EXPECT_EQ(old_result.clay, new_result.clay);
        EXPECT_EQ(old_result.sand, new_result.sand);
        EXPECT_EQ(old_result.air_entry, new_result.air_entry);
        EXPECT_EQ(old_result.b, new_result.b);
        EXPECT_EQ(old_result.Ks, new_result.Ks);
        EXPECT_EQ(old_result.satur, new_result.satur);
        EXPECT_EQ(old_result.fieldc, new_result.fieldc);
        EXPECT_EQ(old_result.wiltp, new_result.wiltp);
        EXPECT_EQ(old_result.bulkd, new_result.bulkd);
    }
}

TEST(soilTchoose, InvalidValue) {
    ASSERT_THROW(auto new_result = soilTchoose(11), std::out_of_range);
}
