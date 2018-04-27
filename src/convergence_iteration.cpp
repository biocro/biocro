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

//! penman_montieth_leaf_temperature
// Calculate leaf temperature given environmental conditions.
// Based on Thornley and Johnson. Plant and Crop Modelling. 1990.
// Equation 14.11e, page 418.
double penman_montieth_leaf_temperature(
        double const irradiance,  // W / (m^2 ground). Total irradiance, power per ground area for the entire spectrum, not photosynthetically active radiation, quanta per ground area of photosynthetically active wavelengths.
        double const leaf_boundary_layer_conductance,  // m / s
        double const leaf_reflectance,  // dimensionless. Fraction of radiation power incident on the leaf that is absorbed by the leaf. 
        double const leaf_transmission,  // dimensionless.  Fraction of radiation power incident on the leaf that is transmitted through the leaf.
        double const air_temperature // degrees C
        )
{
    constexpr double StefanBoltzmann = 5.67037e-8;       // J / m^2 / s / K^4
    const double Ja = 2 * irradiance * 0.235 * (1 - leaf_reflectance - leaf_transmission) / (1 - leaf_transmission);  // W / m^2. Thornley and Johnson. Equation 14.1a.

    /* This is the original from WIMOVAC*/
    double delta_t = 0.01;  // degrees C
    double rlc; /* Long wave radiation for iterative calculation */
    {
        double ChangeInLeafTemp = 10.0;  // degrees C
        double Counter = 0;
        do {
            /* In WIMOVAC, ga was added to the canopy conductance */
            /* ga = (ga * gbcW)/(ga + gbcW); */

            double OldDeltaT = delta_t;

            rlc = 4 * StefanBoltzmann * pow(273 + air_temperature, 3) * delta_t;  // W / m^2

            /* rlc = net long wave radiation emittted per second = radiation emitted per second - radiation absorbed per second = sigma * (Tair + deltaT)^4 - sigma * Tair^4
             * To make it a linear function of deltaT, do a Taylor series about deltaT = 0 and keep only the zero and first order terms.
             * rlc = sigma * Tair^4 + deltaT * (4 * sigma * Tair^3) - sigma * Tair^4 = 4 * sigma * Tair^3 * deltaT
             * where 4 * sigma * Tair^3 is the derivative of sigma * (Tair + deltaT)^4 evaluated at deltaT = 0,
             */

            const double PhiN2 = Ja - rlc;  // W / m^2

            /* This equation is from Thornley and Johnson pg. 418 */
            const double TopValue = PhiN2 * (1 / leaf_boundary_layer_conductance + 1 / conductance_in_m_per_s) - LHV * delta_t;  // J / m^3
            const double BottomValue = LHV * (SlopeFS + PsycParam * (1 + leaf_boundary_layer_conductance / conductance_in_m_per_s));  // J / m^3 / K
            delta_t = fmin(fmax(TopValue / BottomValue, -10), 10); // kelvin. Confine Deltat to the interval [-10, 10]:

            ChangeInLeafTemp = fabs(OldDeltaT - delta_t);  // kelvin
        } while ( (++Counter <= 10) && (ChangeInLeafTemp > 0.5) );
    }
    return air_temperature + delta_t;
}
