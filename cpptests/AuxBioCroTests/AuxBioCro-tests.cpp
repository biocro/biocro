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

TEST(WINDprofTest, miscellaneous_test_data) {
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

TEST(RHprofTest, miscellaneous_test_data) {
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



/// Temperature Functions ///

constexpr auto temp_function_trials = 15; // For all temp function tests.

Rand_double Temp_generator { -88, 58 }; // lowest and highest recorded temps on
                                        // earth (Celsius)

///////////////// TempToDdryA /////////////////

/*
// For generating sample data only.
TEST(TempToDdryA, survey) {

    double temp[temp_function_trials];

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {
        temp[i] = Temp_generator();
        cout << temp[i];
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {

        cout << TempToDdryA(temp[i]);
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;
}
*/


// sample data

// input
double temp[] = {-12.7818, 12.1054, 0.746929, 1.50745, 40.1369, 5.6874,
                 -66.0127, -65.4668, 57.2424, -44.9646, 35.8258, 32.2919,
                 -9.62336, -31.5249, 22.2476};

double DdryA[] = {1.34959, 1.24362, 1.29198, 1.28874, 1.12425, 1.27095, 1.57626,
                  1.57393, 1.05142, 1.48663, 1.14261, 1.15766, 1.33614, 1.4294,
                  1.20043};

TEST(TempToDdryA, miscellaneous_test_data) {
    for (int i = 0; i < temp_function_trials; ++i) {
        EXPECT_NEAR(DdryA[i], TempToDdryA(temp[i]), 1E-5);
    }
}

///////////////// TempToLHV /////////////////

/*
// For generating sample data only.
TEST(TempToLHV, survey) {

    double temp[temp_function_trials];

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {
        temp[i] = Temp_generator();
        cout << temp[i];
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {

        cout << TempToLHV(temp[i]);
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;
}
*/

// sample data

// input
double temp2[] = {32.0425, 49.0254, 11.3617, 2.59461, 18.3041, 50.2254, 41.3477,
                  -25.2844, -71.1602, 24.8255, 29.9093, -3.1921, 39.6624,
                  20.116, -76.4605};

double LHV[] = {2.42497, 2.38468, 2.47404, 2.49484, 2.45757, 2.38183, 2.40289,
                2.56099, 2.66984, 2.4421, 2.43003, 2.50857, 2.40689, 2.45327,
                2.68242};

TEST(TempToLHV, miscellaneous_test_data) {
    for (int i = 0; i < temp_function_trials; ++i) {
        EXPECT_NEAR(LHV[i], TempToLHV(temp2[i]), 1E-5);
    }
}

///////////////// TempToSFS /////////////////

/*
// For generating sample data only.
TEST(TempToSFS, survey) {

    double temp[temp_function_trials];

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {
        temp[i] = Temp_generator();
        cout << temp[i];
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {

        cout << TempToSFS(temp[i]);
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;
}
*/

// sample data

// input
double temp3[] = {-48.9741, -54.4729, 10.4155, -29.4365, 52.2304, -54.7095,
                  8.53117, -4.46423, -2.35123, -31.2189, -42.8268, 8.86809,
                  45.0348, -4.11092, -11.9092};

double SFS[] = {2.44327, 3.01243, 0.578024, 0.96453, 3.96681, 3.03842, 0.516805,
                0.309467, 0.31763, 1.06427, 1.88654, 0.527172, 3.10687,
                0.310141, 0.359771};

TEST(TempToSFS, miscellaneous_test_data) {
    for (int i = 0; i < temp_function_trials; ++i) {
        EXPECT_NEAR(SFS[i], TempToSFS(temp3[i]), 1E-5);
    }
}

///////////////// TempToSWVC /////////////////

/*
// For generating sample data only.
TEST(TempToSWVC, survey) {

    double temp[temp_function_trials];

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {
        temp[i] = Temp_generator();
        cout << setprecision(10) << temp[i];
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;

    cout << "{";
    for (int i = 0; i < temp_function_trials; ++i) {

        cout << TempToSWVC(temp[i]);
        if (i < temp_function_trials - 1) {
            cout << ", ";
        }
    }
    cout << "}" << endl;
}
*/

// sample data

// input
double temp4[] = {-18.2580501, -8.39407392, -33.27833317, 23.51537921,
                  41.13217144, -7.29201811, 17.48521285, -41.25376405,
                  -86.00115419, 14.86395423, 6.267875647, 2.968594595,
                  -0.1383391028, -68.9424391, 0.9925331285};

double SWVC[] = {1.457519922, 3.250349887, 0.3725371654, 28.98704536,
                 78.39856885, 3.540366004, 19.98015193, 0.1665381098,
                 0.0004263947425, 16.90292043, 9.526557742, 7.563213512,
                 6.050954835, 0.005859188571, 6.567097987};

TEST(TempToSWVC, miscellaneous_test_data) {
    for (int i = 0; i < temp_function_trials; ++i) {
        EXPECT_NEAR(SWVC[i], TempToSWVC(temp4[i]), 1E-5);
    }
}

///////////////// EvapoTrans /////////////////

Rand_double Rad_gen {0, 1e3};
Rand_double Itot_gen {0, 1e3};
// Temp_generator
Rand_double RH_gen {0, 1};
Rand_double WindSpeed_gen {0, 408};
Rand_double LeafAreaIndex_gen {0, 10};
Rand_double CanopyHeight_gen {0, 6.72}; // m? Values > 6.73 give nan for ga1
Rand_double StomataWS_gen {0.4, 1}; // ???
Rand_int ws_gen {0, 1};
Rand_double vmax2_gen {20, 50};
Rand_double alpha2_gen {3e-2, 5e-2};
Rand_double kparm_gen {0.5, 0.9};
Rand_double theta_gen {0.73, 0.93};
Rand_double beta_gen {0.8, 0.98};
Rand_double Rd2_gen {0.7, 0.9};
Rand_double b02_gen {0.05, 0.1};
Rand_double b12_gen {2, 4};
Rand_double upperT_gen {30, 50};
Rand_double lowerT_gen {1.5, 5};
Rand_double Catm_gen {280, 500};

TEST(EvapoTrans, random_test_data) {
    constexpr double tolerance = 1E-14;

    for (int i = 0; i < 1e4; ++i) {

        double Rad = Rad_gen();
        double Itot = Itot_gen();
        double Airtemperature =Temp_generator();
        double RH = RH_gen();
        double WindSpeed = WindSpeed_gen();
        double LeafAreaIndex = LeafAreaIndex_gen();
        double CanopyHeight = CanopyHeight_gen();
        double StomataWS = StomataWS_gen();
        int ws = ws_gen();
        double vmax2 = vmax2_gen();
        double alpha2 = alpha2_gen();
        double kparm = kparm_gen();
        double theta = theta_gen();
        double beta = beta_gen();
        double Rd2 = Rd2_gen();
        double b02 = b02_gen();
        double b12 = b12_gen();
        double upperT = upperT_gen();
        double lowerT = lowerT_gen();
        double Catm = Catm_gen();

        ET_Str old_result =
            OldEvapoTrans(Rad, Itot, Airtemperature,
                          RH, WindSpeed, LeafAreaIndex,
                          CanopyHeight, StomataWS, ws,
                          vmax2, alpha2, kparm,
                          theta, beta, Rd2, b02,
                          b12, upperT, lowerT,
                          Catm);

        ET_Str new_result =
            EvapoTrans(Rad, Itot, Airtemperature,
                       RH, WindSpeed, LeafAreaIndex,
                       CanopyHeight, StomataWS, ws,
                       vmax2, alpha2, kparm,
                       theta, beta, Rd2, b02,
                       b12, upperT, lowerT,
                       Catm);

        EXPECT_NEAR(new_result.TransR, old_result.TransR, tolerance);
        EXPECT_NEAR(new_result.EPenman, old_result.EPenman, tolerance);
        EXPECT_NEAR(new_result.EPriestly, old_result.EPriestly, tolerance);
        EXPECT_NEAR(new_result.Deltat, old_result.Deltat, tolerance);
        EXPECT_NEAR(new_result.LayerCond, old_result.LayerCond, tolerance);
    }
}

///////////////// EvapoTrans2 /////////////////

// Rand_double Rad_gen {0, 1e3};
Rand_double Iave_gen {-100, 100}; // no idea what this should be
// Temp_generator
// Rand_double RH_gen {0, 1};
// Rand_double WindSpeed_gen {0, 408};
// Rand_double LeafAreaIndex_gen {0, 10};
// Rand_double CanopyHeight_gen {0, 6.72};
Rand_double stomatacond_gen {80, 700};
Rand_double leafw_gen {1E-3, 1E-1};
Rand_int eteq_gen {0, 2};

TEST(EvapoTrans2, random_test_data) {
    constexpr double tolerance = 1E-14;

    for (int i = 0; i < 1e4; ++i) {

        double Rad = Rad_gen();
        double Iave = Iave_gen();
        double airTemp =Temp_generator();
        double RH = RH_gen();
        double WindSpeed = WindSpeed_gen();
        double LeafAreaIndex = LeafAreaIndex_gen();
        double CanopyHeight = CanopyHeight_gen();
        double stomatacond = stomatacond_gen();
        double leafw = leafw_gen();
        int eteq = eteq_gen();

        ET_Str old_result =
            OldEvapoTrans2(Rad,
                           Iave,
                           airTemp,
                           RH,
                           WindSpeed,
                           LeafAreaIndex,
                           CanopyHeight,
                           stomatacond,
                           leafw,
                           eteq);

        ET_Str new_result =
            EvapoTrans2(Rad,
                        Iave,
                        airTemp,
                        RH,
                        WindSpeed,
                        LeafAreaIndex,
                        CanopyHeight,
                        stomatacond,
                        leafw,
                        eteq);

        EXPECT_NEAR(new_result.TransR, old_result.TransR, tolerance);
        EXPECT_NEAR(new_result.EPenman, old_result.EPenman, tolerance);
        EXPECT_NEAR(new_result.EPriestly, old_result.EPriestly, tolerance);
        EXPECT_NEAR(new_result.Deltat, old_result.Deltat, tolerance);
        EXPECT_NEAR(new_result.LayerCond, old_result.LayerCond, tolerance);
    }
}
