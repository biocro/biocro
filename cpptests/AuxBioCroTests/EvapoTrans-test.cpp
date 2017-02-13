#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// EvapoTrans /////////////////

Rand_double Rad_gen {0, 1e3};
Rand_double Itot_gen {0, 1e3};
extern Rand_double Temp_generator; // defined in temperature_functions-test.cpp
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

TEST(EvapoTrans, RandomTestData) {
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

TEST(EvapoTrans2, RandomTestData) {
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
