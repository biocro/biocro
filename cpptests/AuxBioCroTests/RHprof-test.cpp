#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// RHprof /////////////////

/*
// For generating sample data only.
TEST(RHprof, survey) {
  Rand_double RH_generator { 0, 1 };
  Rand_int nlayers_generator { 1, 20 };

 for (int i = 0; i < 5; ++i) {
    auto RH = RH_generator();
    auto nlayers = nlayers_generator();
    cout << setprecision(10) << "{" << RH << ", " << nlayers << "}" << endl;
    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);
    cout << "{";
    for (int layer = 0; layer < nlayers; ++ layer) {
      cout << relative_humidity_profile[layer];
      if (layer < nlayers - 1) {
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
}
*/

// sample data

// input
double RHprof_args[][2] = {
    {0.6578684265, 19},
    {0.3867285004, 10},
    {0.8407413091, 1},
    {0.2415659511, 4},
    {0.3329553391, 3}
};

// output
double RHprof_results[][MAXLAY] = {
    {0.6698219137, 0.681992596, 0.69438442, 0.7070014037, 0.7198476384,
     0.7329272895, 0.7462445982, 0.7598038828, 0.7736095399, 0.7876660463,
     0.8019779597, 0.8165499209, 0.8313866551, 0.8464929732, 0.8618737735,
     0.8775340433, 0.8934788607, 0.9097133958, 0.9262429128},

    {0.4111878012, 0.437194072, 0.4648451537, 0.4942450749, 0.5255044441,
     0.5587408652, 0.5940793802, 0.6316529396, 0.6716029026, 0.7140795688},

    {0.9858879473},

    {0.2919991927, 0.352961699, 0.4266517309, 0.5157264938},

    {0.4158632934, 0.5194158449, 0.6487536269}
};

constexpr auto num_trials = 5;

TEST(RHprofTest, MiscellaneousTestData) {
  for (int i = 0; i < num_trials; ++i) {
    double arg_set[2];
    for (int j = 0; j < 2; ++j) {
      arg_set[j] = RHprof_args[i][j];
    }
    int nlayers = arg_set[1];
    double relative_humidity_profile[MAXLAY];

    RHprof(arg_set[0], arg_set[1], relative_humidity_profile);

    for (int layer = 0; layer < nlayers; ++layer) {
      EXPECT_NEAR(relative_humidity_profile[layer], RHprof_results[i][layer], 1E-5);
    }

    // Reset RH (the first argument) and check that we get an exception if it is
    // just below 0 or just above 1 but not if it is 0 or 1:

    ASSERT_NO_THROW(RHprof(0, arg_set[1], relative_humidity_profile));
    ASSERT_THROW(RHprof(0 - DBL_MIN, arg_set[1], relative_humidity_profile), std::out_of_range);

    ASSERT_NO_THROW(RHprof(1, arg_set[1], relative_humidity_profile));
    ASSERT_THROW(RHprof(1 + DBL_EPSILON, arg_set[1], relative_humidity_profile), std::out_of_range);

    // Reset nlayers (the second argument) and check that we get an exception if it is
    // less than 1 or greater than MAXLAY but not if it is equal to one of these:

    ASSERT_NO_THROW(RHprof(arg_set[0], 1, relative_humidity_profile));
    ASSERT_THROW(RHprof(arg_set[0], 0, relative_humidity_profile), std::out_of_range);
    ASSERT_THROW(RHprof(arg_set[0], -1, relative_humidity_profile), std::out_of_range);
    
    ASSERT_NO_THROW(RHprof(arg_set[0], MAXLAY, relative_humidity_profile));
    ASSERT_THROW(RHprof(arg_set[0], MAXLAY + 1, relative_humidity_profile), std::out_of_range);
  }
}
