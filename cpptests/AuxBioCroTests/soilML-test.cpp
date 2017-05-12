#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "common_generators.h"
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


Rand_double rootDB_gen {0, 100};
Rand_double LAI_gen {0, 20}; // values above 20 will be rare or non-existent
Rand_double k_gen {0, 100};
Rand_double AirTemp_gen {-20, 45}; // degrees C; values outside this range don't interest us
Rand_double IRad_gen {0, 3000}; // micromoles per square meter per second
Rand_double winds_gen {0, 30}; // m/s (30 m/s is ~67mph)
Rand_double RelH_gen {0, 1};
Rand_double rfl_gen {0, 100};
Rand_double rsec_gen {0, 1};
Rand_double rsdf_gen {0, 2};

Rand_int layers_gen {1, 10};
Rand_int hydrDist_gen {0, 1};


TEST(soilML, RandomTestData) {

    constexpr auto tolerance = 1E-14;

    for (int i = 0; i < 1e5; ++i) {

        auto layers = layers_gen();

        auto precipit = precipit_gen();
        auto transp = evapo_gen();
        double cws[11];
        double cws2[11]; // we need two identical copies since soilML modifies this
        for (int j = 0; j < layers; ++j) {
            cws[j] = cws_gen();
            cws2[j] = cws[j];
        }
        double depths[12];
        depths[0] = 0;
        for (int j = 1; j <= layers; ++j) {
            depths[j] = depths[j - 1] + soildepth_gen();
        }
        auto soildepth = depths[layers];
        auto fieldc = fieldc_gen();
        auto wiltp = wiltp_gen();
        auto phi1 = phi1_gen();
        auto phi2 = phi2_gen();
        auto soiltype = soiltype_gen();
        auto soTexS = soilTchoose(soiltype);
        auto wsFun = wsFun_gen();
        auto rootDB = rootDB_gen();
        auto LAI = LAI_gen();
        auto k = k_gen();
        auto AirTemp = AirTemp_gen();
        auto IRad = IRad_gen();
        auto winds = winds_gen();
        auto RelH = RelH_gen();
        auto hydrDist = hydrDist_gen();
        auto rfl = rfl_gen();
        auto rsec = rsec_gen();
        auto rsdf = rsdf_gen();

    // Assemble information to display in case of errors:
    std::ostringstream oss;
    oss << "precipit: " << precipit << "; transp: " << transp;

    for (int j = 0; j < layers; ++j) {
        oss << "; cws[" << j << "]: " << cws[j];
    }

    oss << "; soildepth: " << soildepth;

    for (int j = 0; j < layers + 1; ++j) {
        oss << "; depths[" << j << "]: " << depths[j];
    }

    oss << "; fieldc: " << fieldc
        << "; wiltp: " << wiltp << "; phi1: " << phi1 << "; phi2: " << phi2
        << "; soiltype: " << soiltype << "; wsFun: " << wsFun << "; rootDB: "
        << rootDB << "; LAI: " << LAI << "; k: " << k << "; AirTemp: "
        << AirTemp << "; IRad: " << IRad << "; winds: " << winds << "; RelH: "
        << RelH << "; hydrDist: " << hydrDist << "; rfl: " << rfl
        << "; rsec: " << rsec << "; rsdf: " << rsdf;

    string input = oss.str();


    auto old_result = OldsoilML(precipit, transp, cws, soildepth, depths, fieldc, wiltp, phi1, phi2, soTexS, wsFun, layers, rootDB, LAI, k, AirTemp, IRad, winds, RelH, hydrDist, rfl, rsec, rsdf);

    auto new_result = soilML(precipit, transp, cws2, soildepth, depths, fieldc, wiltp, phi1, phi2, soTexS, wsFun, layers, rootDB, LAI, k, AirTemp, IRad, winds, RelH, hydrDist, rfl, rsec, rsdf);


    assert_near_or_nan(new_result.rcoefPhoto, old_result.rcoefPhoto, tolerance, "run " + to_string(i) + ", rcoefPhoto: " + input);
    assert_near_or_nan(new_result.rcoefSpleaf, old_result.rcoefSpleaf, tolerance, "run " + to_string(i) + ", rcoefSplear: " + input);
    for (int j = 0; j < layers; ++j) {
        assert_near_or_nan(new_result.cws[j], old_result.cws[j], tolerance, "run " + to_string(i) + ", cws[" + to_string(j) + "]: " + input);
    }
    assert_near_or_nan(new_result.drainage, old_result.drainage, tolerance, "run " + to_string(i) + ", drainage: " + input);
    assert_near_or_nan(new_result.Nleach, old_result.Nleach, tolerance, "run " + to_string(i) + ", Nleach: " + input);
    assert_near_or_nan(new_result.SoilEvapo, old_result.SoilEvapo, tolerance, "run " + to_string(i) + ", SoilEvapo: " + input);
    for (int j = 0; j < layers; ++j) {
        assert_near_or_nan(new_result.rootDist[j], old_result.rootDist[j], tolerance, "run " + to_string(i) + ", rootDist[" + to_string(j) + "]: " + input);
    }
  }
}
