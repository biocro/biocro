#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "common_generators.h"
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


///////////////// watstr /////////////////


TEST(watstr, RandomTestData) {

  constexpr auto absolute_tolerance = 0.0;
  constexpr auto relative_tolerance = 2E-11;

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


    assert_near_rel_or_nan(new_result.rcoefPhoto, old_result.rcoefPhoto, absolute_tolerance, relative_tolerance, input);
    assert_near_rel_or_nan(new_result.rcoefSpleaf, old_result.rcoefSpleaf, absolute_tolerance, relative_tolerance, input);
    assert_near_rel_or_nan(new_result.awc, old_result.awc, absolute_tolerance, relative_tolerance, input);
    assert_near_rel_or_nan(new_result.psim, old_result.psim, absolute_tolerance, relative_tolerance, input);
    assert_near_rel_or_nan(new_result.runoff, old_result.runoff, absolute_tolerance, relative_tolerance, input);
    assert_near_rel_or_nan(new_result.Nleach, old_result.Nleach, absolute_tolerance, relative_tolerance, input);

  }
}
