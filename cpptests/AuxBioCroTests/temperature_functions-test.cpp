#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


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

TEST(TempToDdryA, MiscellaneousTestData) {
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

TEST(TempToLHV, MiscellaneousTestData) {
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

TEST(TempToSFS, MiscellaneousTestData) {
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

TEST(TempToSWVC, MiscellaneousTestData) {
    for (int i = 0; i < temp_function_trials; ++i) {
        EXPECT_NEAR(SWVC[i], TempToSWVC(temp4[i]), 1E-5);
    }
}
