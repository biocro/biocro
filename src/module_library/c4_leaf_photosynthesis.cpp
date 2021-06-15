#include "c4_leaf_photosynthesis.h"
#include "c4photo.h"  // for c4photoC
#include "BioCro.h"   // for EvapoTrans2 and absorbed_shortwave_from_incident_ppfd

string_vector c4_leaf_photosynthesis::get_inputs()
{
    return {
        "par_energy_content",     // J / micromol
        "incident_par",           // J / (m^2 leaf) / s
        "temp",                   // deg. C
        "rh",                     // dimensionless
        "vmax1",                  // micromole / m^2 / s
        "alpha1",                 // mol / mol
        "kparm",                  // mol / m^2 / s
        "theta",                  // dimensionless
        "beta",                   // dimensionless
        "Rd",                     // micromole / m^2 / s
        "b0",                     // mol / m^2 / s
        "b1",                     // dimensionless
        "Gs_min",                 // mol / m^2 / s
        "StomataWS",              // dimensionless
        "Catm",                   // micromole / mol
        "atmospheric_pressure",   // Pa
        "water_stress_approach",  // a dimensionless switch
        "upperT",                 // deg. C
        "lowerT",                 // deg. C
        "incident_average_par",   // J / (m^2 leaf) / s
        "windspeed",              // m / s
        "height",                 // m
        "leafwidth",              // m
        "specific_heat_of_air",   // J / kg / K
        "et_equation"             // a dimensionless switch
    };
}

string_vector c4_leaf_photosynthesis::get_outputs()
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

void c4_leaf_photosynthesis::do_operation() const
{
    // Convert light inputs from energy to molecular flux densities (these
    // inputs should eventually be changed to PPFD)
    const double incident_par_micromol = incident_par / par_energy_content;                  // micromol / m^2 / s
    const double incident_average_par_micromol = incident_average_par / par_energy_content;  // micromol / m^2 / s

    // Determine the absorbed shortwave light energy from the "incident average
    // PAR" and "incident PAR" using hardcoded values (should become inputs
    // later).
    double constexpr par_energy_fraction = 0.5;  // dimensionless
    double constexpr leaf_reflectance = 0.2;     // dimensionless
    double constexpr leaf_transmittance = 0.2;   // dimensionless

    double const absorbed_shortwave_avg =
        absorbed_shortwave_from_incident_ppfd(
            incident_average_par_micromol, 0.235,
            par_energy_fraction, leaf_reflectance, leaf_transmittance);  // J / m^2 / s

    double const absorbed_shortwave =
        absorbed_shortwave_from_incident_ppfd(
            incident_par_micromol, 0.235,
            par_energy_fraction, leaf_reflectance, leaf_transmittance);  // J / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at air temperature
    const double initial_stomatal_conductance =
        c4photoC(
            incident_par_micromol, temp, rh, vmax1, alpha1, kparm, theta, beta,
            Rd, b0, b1, Gs_min * 1e3, StomataWS, Catm, atmospheric_pressure,
            water_stress_approach, upperT, lowerT)
            .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature
    double constexpr LAI = 0.0;  // EvapoTrans2 does not actually use LAI for anything
    const struct ET_Str et =
        EvapoTrans2(
            absorbed_shortwave_avg, absorbed_shortwave, temp, rh, windspeed,
            LAI, height, initial_stomatal_conductance, leafwidth,
            specific_heat_of_air, et_equation);

    const double leaf_temperature = temp + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci using the new leaf temperature
    const struct c4_str photo =
        c4photoC(
            incident_par_micromol, leaf_temperature, rh, vmax1, alpha1, kparm,
            theta, beta, Rd, b0, b1, Gs_min * 1e3, StomataWS, Catm,
            atmospheric_pressure, water_stress_approach, upperT, lowerT);

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
