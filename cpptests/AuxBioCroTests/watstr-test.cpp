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

// Skips ASSERT_NEAR if both values are NaN, both are positive
// infinity, or both are negative infinity.
void assert_near_or_nan(double val1, double val2, double tolerance, string message) {
  if ((isnan(val1) && isnan(val2))
      ||
      (isinf(val1) && (val1 == val2))) {
    return;
  }

  ASSERT_NEAR(val1, val2, tolerance) << message;
}

TEST(watstr, RandomTestData) {

  constexpr auto tolerance = 1E-14;

  for (int i = 0; i < 1e5; ++i) {

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

    // Assemble information to display in case of errors:
    std::ostringstream oss;
    oss << "precipit: " << precipit << "; evapo: " << evapo
        << "; cws: " << cws << "; soildepth: "
        << soildepth << "; fieldc: "
        << fieldc << "; wiltp: " << wiltp
        << "; phi1: " << phi1 << "; phi2: "
        << phi2 << "; soiltype: " << soiltype
        << "; wsFun: " << wsFun;
    string input = oss.str();


    auto old_result = Oldwatstr(precipit, evapo, cws, soildepth, fieldc, wiltp, phi1, phi2, soiltype, wsFun);

    auto new_result = watstr(precipit, evapo, cws, soildepth, fieldc, wiltp, phi1, phi2, soiltype, wsFun);


    assert_near_or_nan(new_result.rcoefPhoto, old_result.rcoefPhoto, tolerance, input);
    assert_near_or_nan(new_result.rcoefSpleaf, old_result.rcoefSpleaf, tolerance, input);
    assert_near_or_nan(new_result.awc, old_result.awc, tolerance, input);
    assert_near_or_nan(new_result.psim, old_result.psim, tolerance, input);
    assert_near_or_nan(new_result.runoff, old_result.runoff, tolerance, input);
    assert_near_or_nan(new_result.Nleach, old_result.Nleach, tolerance, input);

  }
}
