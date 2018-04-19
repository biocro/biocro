#include <math.h>
#include "convergence_iteration.hpp"
#include "collatz_photo.hpp"
#include "ball_berry.h"

state_map collatz_canopy::do_operation (state_map const& s) const
{
    // Initial guesses
    double intercelluar_co2_molar_fraction = s.at("Catm") * 0.4;
    double leaf_temperature = s.at("temp");
    struct collatz_result r = collatz_photo(s.at("solar"), leaf_temperature, s.at("vmax"),
            s.at("alpha"), s.at("kparm"), s.at("theta"), s.at("beta"), s.at("Rd"),
            s.at("upperT"), s.at("lowerT"), s.at("k_Q10"), intercelluar_co2_molar_fraction);

    // Convergence loop
    for (unsigned int n = 0; n < 50; ++n) {
        double const previous_assimilation = r.assimilation;
        r = collatz_photo(s.at("solar"), leaf_temperature, s.at("vmax"),
                s.at("alpha"), s.at("kparm"), s.at("theta"), s.at("beta"), s.at("Rd"),
                s.at("upperT"), s.at("lowerT"), s.at("k_Q10"), intercelluar_co2_molar_fraction);

        if (s.at("water_stress_approach") == 0) r.assimilation *= s.at("StomataWS");

        double gs = ball_berry(r.assimilation, s.at("Catm"), s.at("RH"), s.at("b0"), s.at("b1"));

        if (s.at("water_stress_approach") == 1) gs *= s.at("StomataWS");

        intercelluar_co2_molar_fraction = s.at("Catm") - r.assimilation / gs;

        struct ET_Str et_direct = EvapoTrans2(IDir, Itot, temperature, relative_humidity, layer_wind_speed, LAIc, CanHeight, direct_stomatal_conductance, leafwidth, eteq);
        double leaf_temperature_Idir = temperature + et_direct.Deltat;

        double constexpr tol = 0.01;
        double const diff = fabs(previous_assimilation - r.assimilation);
        if (diff <= tol) break;
    };

    state_map result {
        { "canopy_assimilation_rate", r.assimilation },  // TODO: Fix this! r.assimilation is leaf-level assimlation, not canopy total.
    };

    return result;
}

