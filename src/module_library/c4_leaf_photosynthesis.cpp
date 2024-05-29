#include "c4_leaf_photosynthesis.h"
#include "c4photo.h"              // for c4photoC
#include "leaf_energy_balance.h"  // for leaf_energy_balance

using standardBML::c4_leaf_photosynthesis;

string_vector c4_leaf_photosynthesis::get_inputs()
{
    return {
        "absorbed_longwave",     // J / (m^2 leaf) / s
        "absorbed_shortwave",    // J / (m^2 leaf) / s
        "alpha1",                // mol / mol
        "atmospheric_pressure",  // Pa
        "b0",                    // mol / m^2 / s
        "b1",                    // dimensionless
        "beta",                  // dimensionless
        "Catm",                  // micromol / mol
        "gbw_canopy",            // m / s
        "Gs_min",                // mol / m^2 / s
        "incident_ppfd",         // micromol / (m^2 leaf) / s
        "kparm",                 // mol / m^2 / s
        "leafwidth",             // m
        "lowerT",                // deg. C
        "Rd",                    // micromol / m^2 / s
        "rh",                    // dimensionless
        "StomataWS",             // dimensionless
        "temp",                  // deg. C
        "theta",                 // dimensionless
        "upperT",                // deg. C
        "vmax1",                 // micromol / m^2 / s
        "windspeed"              // m / s
    };
}

string_vector c4_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",             // micromol / m^2 /s
        "GrossAssim",        // micromol / m^2 /s
        "Rp",                // micromol / m^2 / s
        "Ci",                // micromol / mol
        "Gs",                // mol / m^2 / s
        "Cs",                // micromol / m^2 / s
        "RHs",               // dimensionless from Pa / Pa
        "TransR",            // mmol / m^2 / s
        "EPenman",           // mmol / m^2 / s
        "EPriestly",         // mmol / m^2 / s
        "leaf_temperature",  // deg. C
        "gbw"                // mol / m^2 / s
    };
}

void c4_leaf_photosynthesis::do_operation() const
{
    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    const double initial_stomatal_conductance =
        c4photoC(
            incident_ppfd, ambient_temperature, ambient_temperature,
            rh, vmax1, alpha1, kparm, theta, beta,
            Rd, b0, b1, Gs_min, StomataWS, Catm, atmospheric_pressure,
            upperT, lowerT, gbw_guess)
            .Gs;  // mol / m^2 / s

    // Calculate a new value for leaf temperature
    const energy_balance_outputs et = leaf_energy_balance(
        absorbed_longwave,
        absorbed_shortwave,
        atmospheric_pressure,
        ambient_temperature,
        gbw_canopy,
        leafwidth,
        rh,
        initial_stomatal_conductance,
        windspeed);

    const double leaf_temperature = ambient_temperature + et.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    const photosynthesis_outputs photo =
        c4photoC(
            incident_ppfd, leaf_temperature, ambient_temperature,
            rh, vmax1, alpha1, kparm,
            theta, beta, Rd, b0, b1, Gs_min, StomataWS, Catm,
            atmospheric_pressure, upperT, lowerT,
            et.boundary_layer_conductance);

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(GrossAssim_op, photo.GrossAssim);
    update(Rp_op, photo.Rp);
    update(Ci_op, photo.Ci);
    update(Gs_op, photo.Gs);
    update(Cs_op, photo.Cs);
    update(RHs_op, photo.RHs);
    update(TransR_op, et.TransR);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(leaf_temperature_op, leaf_temperature);
    update(gbw_op, et.boundary_layer_conductance);
}
