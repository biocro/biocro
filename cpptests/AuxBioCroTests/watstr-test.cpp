#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// watstr /////////////////

Rand_double precipit_gen {-100, 100};
Rand_double evapo_gen {-100, 100};
Rand_double cws_gen {-100, 100};
Rand_double soildepth_gen {-100, 100};
Rand_double fieldc_gen {-100, 100};
Rand_double wiltp_gen {-100, 100};
Rand_double phi1_gen {-100, 100};
Rand_double phi2_gen {-100, 100};

Rand_int soiltype_gen {0, 10};
Rand_int wsFun_gen {0, 4};

TEST(watstr, RandomTestData) {

  constexpr auto tolerance = 1E-14;

  for (int i = 0; i < 1e6; ++i) {

    auto precipit = precipit_gen();
    auto evapo = evapo_gen();
    auto cws = cws_gen();
    auto soildepth = soildepth_gen();
    auto fieldc = fieldc_gen();
    auto wiltp = wiltp_gen();
    auto phi1 = phi1_gen();
    auto phi2 = phi2_gen();
    auto soiltype = soiltype_gen();
    auto wsFun = wsFun_gen();
    

    auto old_result = Oldwatstr(precipit, evapo, cws, soildepth, fieldc, wiltp, phi1, phi2, soiltype, wsFun);

    auto new_result = watstr(precipit, evapo, cws, soildepth, fieldc, wiltp, phi1, phi2, soiltype, wsFun);


    EXPECT_NEAR(new_result.rcoefPhoto, old_result.rcoefPhoto, tolerance) << input;
    EXPECT_NEAR(new_result.rcoefSpleaf, old_result.rcoefSpleaf, tolerance) << input;
    EXPECT_NEAR(new_result.awc, old_result.awc, tolerance) << input;
    EXPECT_NEAR(new_result.psim, old_result.psim, tolerance) << input;
    EXPECT_NEAR(new_result.runoff, old_result.runoff, tolerance) << input;
    EXPECT_NEAR(new_result.Nleach, old_result.Nleach, tolerance) << input;

  }
}
