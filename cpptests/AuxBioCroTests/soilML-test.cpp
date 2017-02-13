#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "common_generators.h"
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


Rand_double rootDB_gen {0, 100};
Rand_double LAI_gen {0, 100};
Rand_double k_gen {0, 100};
Rand_double AirTemp_gen {-100, 100};
Rand_double IRad_gen {0, 100};
Rand_double winds_gen {0, 100};
Rand_double RelH_gen {0, 1};
Rand_double rfl_gen {0, 100};
Rand_double rsec_gen {0, 1};
Rand_double rsdf_gen {-100, 100};

Rand_int hydrDist_gen {0, 1};


TEST(soilML, RandomTestData) {

    constexpr auto tolerance = 1E-14;

    for (int i = 0; i < 1e5; ++i) {

        auto precipit = precipit_gen();
        auto transp = evapo_gen();
        double cws[] = { cws_gen() }; // for now
        auto soildepth = soildepth_gen();
        double depths[] = { soildepth }; // for now
        auto fieldc = fieldc_gen();
        auto wiltp = wiltp_gen();
        auto phi1 = phi1_gen();
        auto phi2 = phi2_gen();
        auto soiltype = soiltype_gen();
        auto soTexS = soilTchoose(soiltype);
        auto wsFun = wsFun_gen();
        auto layers = 1; // for now
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
    oss << "precipit: " << precipit << "; transp: " << transp
        << "; cws: " << cws << "; soildepth: "
        << soildepth << "; fieldc: "
        << fieldc << "; wiltp: " << wiltp
        << "; phi1: " << phi1 << "; phi2: "
        << phi2 << "; soiltype: " << soiltype
        << "; wsFun: " << wsFun;
    string input = oss.str();

    /*
    auto old_result = OldsoilML(precipit, transp, cws, soildepth, depths, fieldc, wiltp, phi1, phi2, soTexS, wsFun, layers, rootDB, LAI, k, AirTemp, IRad, winds, RelH, hydrDist, rfl, rsec, rsdf);

    auto new_result = soilML(precipit, transp, cws, soildepth, depths, fieldc, wiltp, phi1, phi2, soTexS, wsFun, layers, rootDB, LAI, k, AirTemp, IRad, winds, RelH, hydrDist, rfl, rsec, rsdf);


    ASSERT_NEAR(new_result.rcoefPhoto, old_result.rcoefPhoto, tolerance) << input;
    ASSERT_NEAR(new_result.rcoefSpleaf, old_result.rcoefSpleaf, tolerance) << input;
    ASSERT_NEAR(new_result.cws[0], old_result.cws[0], tolerance) << input;
    ASSERT_NEAR(new_result.drainage, old_result.drainage, tolerance) << input;
    ASSERT_NEAR(new_result.Nleach, old_result.Nleach, tolerance) << input;
    ASSERT_NEAR(new_result.SoilEvapo, old_result.SoilEvapo, tolerance) << input;
    ASSERT_NEAR(new_result.rootDist[0], old_result.rootDist[0], tolerance) << input;
    */
  }
}
