#include <math.h>
#include "convergence_iteration.hpp"
#include "collatz_photo.hpp"
#include "ball_berry.h"

state_map collatz_canopy::do_operation (state_map const& s) const
{
    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double layer_vapor_pressure = s.at("RH") * s.at("saturation_vapor_pressure");

    // Initial guesses
    double intercelluar_co2_molar_fraction = s.at("Catm") * 0.4;
    double leaf_temperature = s.at("temp");
    struct collatz_result r = collatz_photo(s.at("solar"), leaf_temperature, s.at("vmax"),
            s.at("alpha"), s.at("kparm"), s.at("theta"), s.at("beta"), s.at("Rd"),
            s.at("upperT"), s.at("lowerT"), s.at("k_Q10"), intercelluar_co2_molar_fraction);
    double gs = ball_berry(r.assimilation, s.at("Catm"), s.at("RH"), s.at("b0"), s.at("b1"));
    double conductance_in_m_per_s = gs * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double boundary_layer_conductance = leaf_boundary_layer_conductance(s.at("layer_wind_speed"),
                                s.at("leafwidth"), s.at("temp"), leaf_temperature - s.at("temp"), conductance_in_m_per_s, layer_vapor_pressure);  // m / s

    // Convergence loop
    for (unsigned int n = 0; n < 50; ++n) {
        double const previous_assimilation = r.assimilation;

        intercelluar_co2_molar_fraction = s.at("Catm") - r.assimilation / gs;
        r = collatz_photo(s.at("solar"), leaf_temperature, s.at("vmax"),
                s.at("alpha"), s.at("kparm"), s.at("theta"), s.at("beta"), s.at("Rd"),
                s.at("upperT"), s.at("lowerT"), s.at("k_Q10"), intercelluar_co2_molar_fraction);

        if (s.at("water_stress_approach") == 0) r.assimilation *= s.at("StomataWS");

        gs = ball_berry(r.assimilation, s.at("Catm"), s.at("RH"), s.at("b0"), s.at("b1"));

        if (s.at("water_stress_approach") == 1) gs *= s.at("StomataWS");

        double boundary_layer_conductance = leaf_boundary_layer_conductance(s.at("layer_wind_speed"), s.at("leafwidth"), s.at("temp"), leaf_temperature - s.at("temp"), conductance_in_m_per_s, layer_vapor_pressure);  // m / s


        struct ET_Str et_direct = EvapoTrans2(s.at("direct_irradiance"), s.at("total_irradiance"), s.at("temp"), s.at("RH"), s.at("layer_wind_speed"), s.at("layer_lai"), s.at("layer_height"), s.at("direct_stomatal_conductance"), s.at("leafwidth"), s.at("eq_equation"));
        leaf_temperature = s.at("temp") + et_direct.Deltat;

        double constexpr tol = 0.01;
        double const diff = fabs(previous_assimilation - r.assimilation);
        if (diff <= tol) break;
    };

    state_map result {
        { "layer_assimilation_rate", r.assimilation },  // TODO: Fix this! r.assimilation is leaf-level assimlation, not canopy total.
        { "layer_stomatal_conductance", gs },  // TODO: Fix this! r.assimilation is leaf-level assimlation, not canopy total.
    };

    return result;
}

