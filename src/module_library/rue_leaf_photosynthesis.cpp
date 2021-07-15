#include "rue_leaf_photosynthesis.h"
#include "../constants.h"  // for ideal_gas_constant and celsius_to_kelvin
#include "ball_berry.hpp"  // for ball_berry
#include "BioCro.h"        // for c3EvapoTrans
#include "AuxBioCro.h"     // for arrhenius_exponent
#include <cmath>           // for exp

// Define a structure for storing the output of `rue_photo()`
struct rue_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
};

// Determine assimilation, stomatal conductance, and Ci following `c3photoC()`.
// Here, rather than using the FvCB model for C3 photosynthesis, gross
// assimilation is determined using a simple RUE model. Water stress is ignored.
// Respiration and stomatal conductance are otherwise calculated using the same
// methods as `c3photoC()`.
struct rue_str rue_photo(
    double Qp,         // mol / m^2 / s
    double alpha_rue,  // dimensionless
    double Tleaf,      // degrees C
    double RH,         // dimensionless from Pa / Pa
    double Rd0,        // mol / m^2 / s
    double bb0,        // mol / m^2 / s
    double bb1,        // dimensionless
    double Ca          // dimensionless from mol / mol
)
{
    using conversion_constants::celsius_to_kelvin;  // deg. C or K

    // Convert leaf temperature to Kelvin
    double const tlk = Tleaf + celsius_to_kelvin;  // K

    // Calculate gross assimilation based on incident light and radiation use
    // efficiency
    double const ag = Qp * alpha_rue;  // mol / m^2 / s

    // Determine respiration from the leaf temperature using Arrhenius exponents
    // from Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259.
    // https://doi.org/10.1111/j.1365-3040.2001.00668.x
    double const rd = Rd0 *
                      arrhenius_exponent(
                          18.72,
                          46.39e3,
                          tlk);  // mol / m^2 / s

    // Determine net assimilation
    double const an = ag - rd;  // mol / m^2 / s

    // Determine stomatal conductance
    double const gs = ball_berry(an, Ca, RH, bb0, bb1) * 1e-3;  // mol / m^2 / s

    // Determine intercellular CO2 concentration
    double const ci = Ca - an * 1.6 / gs;  // dimensionless

    // Return the results
    struct rue_str result;
    result.Assim = an * 1e6;       // micromol / m^2 / s
    result.Gs = gs * 1e3;          // mmol / m^2 / s
    result.Ci = ci * 1e6;          // micromole / mol
    result.GrossAssim = ag * 1e6;  // micromole / m^2 / s
    return result;
}

string_vector rue_leaf_photosynthesis::get_inputs()
{
    return {
        "incident_ppfd",               // micromol / (m^2 leaf) / s
        "alpha_rue",                   // dimensionless
        "temp",                        // deg. C
        "rh",                          // dimensionless
        "Rd",                          // micromole / m^2 / s
        "b0",                          // mol / m^2 / s
        "b1",                          // dimensionless
        "Catm",                        // micromole / mol
        "average_absorbed_shortwave",  // J / (m^2 leaf) / s
        "windspeed",                   // m / s
        "height",                      // m
        "specific_heat_of_air"         // J / kg / K
    };
}

string_vector rue_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",            // micromole / m^2 /s
        "GrossAssim",       // micromole / m^2 /s
        "Ci",               // micromole / mol
        "Gs",               // mmol / m^2 / s
        "TransR",           // mmol / m^2 / s
        "EPenman",          // mmol / m^2 / s
        "EPriestly",        // mmol / m^2 / s
        "leaf_temperature"  // deg. C
    };
}

void rue_leaf_photosynthesis::do_operation() const
{
    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    const double initial_stomatal_conductance =
        rue_photo(
            incident_ppfd * 1e-6,  // mol / m^2 / s
            alpha_rue,             // dimensionless
            temp,                  // degrees C
            rh,                    // dimensionless from Pa / Pa
            Rd * 1e-6,             // mol / m^2 / s
            b0,                    // mol / m^2 / s
            b1,                    // dimensionless
            Catm * 1e-6            // dimensionless from mol / mol
            )
            .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature
    const struct ET_Str et = c3EvapoTrans(
        average_absorbed_shortwave,
        temp,
        rh,
        windspeed,
        height,
        specific_heat_of_air,
        initial_stomatal_conductance);

    const double leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    const struct rue_str photo =
        rue_photo(
            incident_ppfd * 1e-6,  // mol / m^2 / s
            alpha_rue,             // dimensionless
            leaf_temperature,      // degrees C
            rh,                    // dimensionless from Pa / Pa
            Rd * 1e-6,             // mol / m^2 / s
            b0,                    // mol / m^2 / s
            b1,                    // dimensionless
            Catm * 1e-6            // dimensionless from mol / mol
        );

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(GrossAssim_op, photo.GrossAssim);
    update(Ci_op, photo.Ci);
    update(Gs_op, photo.Gs);
    update(TransR_op, et.TransR);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(leaf_temperature_op, leaf_temperature);
}
