#include "c3_leaf_photosynthesis.h"
#include "c3photo.hpp"  // for c3photoC
#include "BioCro.h"     // for c3EvapoTrans

string_vector c3_leaf_photosynthesis::get_inputs()
{
    return {
        "incident_ppfd",                // micromol / (m^2 leaf) / s
        "temp",                         // deg. C
        "rh",                           // dimensionless
        "vmax1",                        // micromole / m^2 / s
        "jmax",                         // micromole / m^2 / s
        "tpu_rate_max",                 // micromole / m^2 / s
        "Rd",                           // micromole / m^2 / s
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "Catm",                         // micromole / mol
        "atmospheric_pressure",         // Pa
        "O2",                           // mmol / mol
        "theta",                        // dimensionless
        "StomataWS",                    // dimensionless
        "water_stress_approach",        // a dimensionless switch
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "average_absorbed_shortwave",   // J / (m^2 leaf) / s
        "windspeed",                    // m / s
        "height",                       // m
        "specific_heat_of_air"          // J / kg / K
    };
}

string_vector c3_leaf_photosynthesis::get_outputs()
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

void c3_leaf_photosynthesis::do_operation() const
{
    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    double const initial_stomatal_conductance =
        c3photoC(
            incident_ppfd, temp, rh, vmax1, jmax, tpu_rate_max, Rd, b0,
            b1, Gs_min, Catm, atmospheric_pressure, O2, theta, StomataWS,
            water_stress_approach, electrons_per_carboxylation,
            electrons_per_oxygenation)
            .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature using the estimate for
    // stomatal conductance
    const struct ET_Str et =
        c3EvapoTrans(
            average_absorbed_shortwave, temp, rh, windspeed, height,
            specific_heat_of_air, initial_stomatal_conductance);

    double const leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    const struct c3_str photo =
        c3photoC(
            incident_ppfd, leaf_temperature, rh, vmax1, jmax,
            tpu_rate_max, Rd, b0, b1, Gs_min, Catm, atmospheric_pressure, O2,
            theta, StomataWS, water_stress_approach,
            electrons_per_carboxylation, electrons_per_oxygenation);

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
