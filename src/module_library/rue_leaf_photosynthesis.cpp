#include <algorithm>                    // for std::min
#include <cmath>                        // for exp
#include "ball_berry_gs.h"              // for ball_berry_gs
#include "BioCro.h"                     // for c3EvapoTrans
#include "AuxBioCro.h"                  // for arrhenius_exponential
#include "photosynthesis_outputs.h"     // for photosynthesis_outputs
#include "conductance_limited_assim.h"  // for conductance_limited_assim
#include "../framework/constants.h"     // for celsius_to_kelvin, dr_stomata,
                                        //     dr_boundary
#include "rue_leaf_photosynthesis.h"

using conversion_constants::celsius_to_kelvin;
using physical_constants::dr_boundary;
using physical_constants::dr_stomata;
using standardBML::rue_leaf_photosynthesis;

// Determine assimilation, stomatal conductance, and Ci following `c3photoC()`.
// Here, rather than using the FvCB model for C3 photosynthesis, gross
// assimilation is determined using a simple RUE model. Water stress is ignored.
// Respiration and stomatal conductance are otherwise calculated using the same
// methods as `c3photoC()`.
photosynthesis_outputs rue_photo(
    double Qp,         // mol / m^2 / s
    double alpha_rue,  // dimensionless
    double Tleaf,      // degrees C
    double RH,         // dimensionless from Pa / Pa
    double Rd0,        // mol / m^2 / s
    double bb0,        // mol / m^2 / s
    double bb1,        // dimensionless
    double Ca,         // dimensionless from mol / mol
    double gbw         // mol / m^2 / s
)
{
    // Convert leaf temperature to Kelvin
    double const tlk = Tleaf + celsius_to_kelvin;  // K

    // Calculate gross assimilation based on incident light and radiation use
    // efficiency
    double const ag = Qp * alpha_rue;  // mol / m^2 / s

    // Determine respiration from the leaf temperature using Arrhenius exponents
    // from Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259.
    // https://doi.org/10.1111/j.1365-3040.2001.00668.x
    double const rd = Rd0 *
                      arrhenius_exponential(
                          18.72,
                          46.39e3,
                          tlk);  // mol / m^2 / s

    // Determine net assimilation
    double an = ag - rd;  // mol / m^2 / s

    // The net CO2 assimilation is the smaller of the RUE-limited and
    // conductance-limited rates. This will prevent the calculated Ci and Cs
    // values from ever being < 0. This seems to be an important restriction to
    // prevent numerical errors while running the Ball-Berry model. Here we
    // just use a very large value for gsw; this may cause a slight understimate
    // of the net assimilation rate in some cases.
    double const an_conductance =
        conductance_limited_assim(Ca * 1e6, gbw, 1e3) * 1e-6;  // mol / m^2 / s

    an = std::min(an_conductance, an);  // mol / m^2 / s

    // Determine stomatal conductance using the Ball-Berry model, equating the
    // leaf and air temperatures (mol / m^2 / s)
    stomata_outputs const BB_res =
        ball_berry_gs(an, Ca, RH, bb0, bb1, gbw, Tleaf, Tleaf);

    double const gs = BB_res.gsw * 1e-3;

    // Determine intercellular CO2 concentration
    double const ci = Ca - an * (dr_boundary / gbw + dr_stomata / gs);  // dimensionless

    return photosynthesis_outputs{
        /* .Assim = */ an * 1e6,                          // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance * 1e6,  // micromol / m^2 / s
        /* .Ci = */ ci * 1e6,                             // micromol / mol
        /* .GrossAssim = */ ag * 1e6,                     // micromol / m^2 / s
        /* .Gs = */ gs * 1e3,                             // mmol / m^2 / s
        /* .Cs = */ BB_res.cs,                            // micromol / m^2 / s
        /* .RHs = */ BB_res.hs,                           // dimensionless from Pa / Pa
        /* .Rp = */ 0.0,                                  // micromol / m^2 / s
        /* .iterations = */ 0                             // not a physical quantity
    };
}

string_vector rue_leaf_photosynthesis::get_inputs()
{
    return {
        "incident_ppfd",               // micromol / (m^2 leaf) / s
        "alpha_rue",                   // dimensionless
        "temp",                        // deg. C
        "rh",                          // dimensionless
        "Rd",                          // micromol / m^2 / s
        "b0",                          // mol / m^2 / s
        "b1",                          // dimensionless
        "Catm",                        // micromol / mol
        "average_absorbed_shortwave",  // J / (m^2 leaf) / s
        "windspeed",                   // m / s
        "height",                      // m
        "specific_heat_of_air",        // J / kg / K
        "minimum_gbw",                 // mol / m^2 / s
        "windspeed_height"             // m
    };
}

string_vector rue_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",             // micromol / m^2 /s
        "GrossAssim",        // micromol / m^2 /s
        "Rp",                // micromol / m^2 / s
        "Ci",                // micromol / mol
        "Gs",                // mmol / m^2 / s
        "TransR",            // mmol / m^2 / s
        "EPenman",           // mmol / m^2 / s
        "EPriestly",         // mmol / m^2 / s
        "leaf_temperature",  // deg. C
        "gbw"                // mol / m^2 / s
    };
}

void rue_leaf_photosynthesis::do_operation() const
{
    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

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
            Catm * 1e-6,           // dimensionless from mol / mol
            gbw_guess              // mol / m^2 / s
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
        initial_stomatal_conductance,
        minimum_gbw,
        windspeed_height);

    const double leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    const photosynthesis_outputs photo =
        rue_photo(
            incident_ppfd * 1e-6,          // mol / m^2 / s
            alpha_rue,                     // dimensionless
            leaf_temperature,              // degrees C
            rh,                            // dimensionless from Pa / Pa
            Rd * 1e-6,                     // mol / m^2 / s
            b0,                            // mol / m^2 / s
            b1,                            // dimensionless
            Catm * 1e-6,                   // dimensionless from mol / mol
            et.boundary_layer_conductance  // mol / m^2 / s
        );

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(GrossAssim_op, photo.GrossAssim);
    update(Rp_op, photo.Rp);
    update(Ci_op, photo.Ci);
    update(Gs_op, photo.Gs);
    update(TransR_op, et.TransR);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(leaf_temperature_op, leaf_temperature);
    update(gbw_op, et.boundary_layer_conductance);
}
