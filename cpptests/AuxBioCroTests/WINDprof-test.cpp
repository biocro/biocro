#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// WINDprof /////////////////

/*
// For generating sample data only.
TEST(WINDprof, survey) {
  Rand_double wind_speed_generator { 0, 100 };
  Rand_double LAI_generator { 0, 1 };
  Rand_int nlayers_generator { 1, 20 };



 for (int i = 0; i < 10; ++i) {
    auto WindSpeed = wind_speed_generator();
    auto LAI = LAI_generator();
    auto nlayers = nlayers_generator();
    cout << setprecision(10) << "{" << WindSpeed << ", " << LAI << ", " << nlayers << "}" << endl;
    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);
    cout << "{";
    for (int layer = 0; layer < nlayers; ++ layer) {
      cout << wind_speed_profile[layer];
      if (layer < nlayers - 1) {
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
}
*/

// sample data
constexpr auto num_trials = 5;

// input
double WINDprof_args[][3] = {
  {8.196468057, 0.08196468057, 18},
  {64.72380617, 0.6472380617, 20},
  {40.70143633, 0.4070143633, 15},
  {95.59181556, 0.9559181556, 17},
  {71.55012775, 0.7155012775, 14}
};

// output
double WINDprof_results[][MAXLAY] = {
  {8.196468057, 8.170383284, 8.144381525, 8.118462514, 8.092625989, 8.066871687,
   8.041199347, 8.015608708, 7.990099509, 7.964671492, 7.939324398, 7.914057969,
   7.88887195, 7.863766083, 7.838740114, 7.81379379, 7.788926855, 7.764139058},

  {64.72380617, 63.27407886, 61.85682352, 60.47131282, 59.11683572, 57.7926971,
   56.49821743, 55.23273239, 53.99559253, 52.78616297, 51.60382302, 50.44796591,
   49.31799847, 48.21334079, 47.13342599, 46.07769984, 45.04562055, 44.03665847,
   43.0502958, 42.08602634},

  {40.70143633, 39.93564881, 39.18426939, 38.447027, 37.72365563, 37.01389432,
   36.317487, 35.63418241, 34.96373402, 34.30589996, 33.66044289, 33.02712993,
   32.40573259, 31.7960267, 31.19779227},

  {95.59181556, 91.90228281, 88.35515402, 84.94493284, 81.66633509, 78.51428054,
   75.48388503, 72.57045291, 69.76946979, 67.07659549, 64.48765737, 61.99864383,
   59.6056981, 57.30511229, 55.09332159, 52.96689882, 50.92254903},

  {71.55012775, 69.0356631, 66.60956353, 64.26872364, 62.01014719, 59.83094321,
   57.72832234, 55.69959325, 53.74215917, 51.85351461, 50.03124212, 48.27300921,
   46.57656534, 44.93973909}
};

TEST(WINDprofTest, MiscellaneousTestData) {
  for (int i = 0; i < num_trials; ++i) {
    double arg_set[3];
    for (int j = 0; j < 3; ++j) {
      arg_set[j] = WINDprof_args[i][j];
    }
    int nlayers = arg_set[2];
    double wind_speed_profile[nlayers];

    WINDprof(arg_set[0], arg_set[1], arg_set[2], wind_speed_profile);

    for (int layer = 0; layer < nlayers; ++layer) {
      EXPECT_NEAR(wind_speed_profile[layer], WINDprof_results[i][layer], 1E-5);
    }
  }
}
