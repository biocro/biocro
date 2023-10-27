#include "c4_leaf_photosynthesis.h"
#include "c4photo.h"  // for c4photoC
#include "BioCro.h"   // for EvapoTrans2

using standardBML::c4_leaf_photosynthesis;

string_vector c4_leaf_photosynthesis::get_inputs()
{
    return {
        "incident_ppfd",               // micromol / (m^2 leaf) / s
        "temp",                        // deg. C
        "rh",                          // dimensionless
        "vmax1",                       // micromol / m^2 / s
        "alpha1",                      // mol / mol
        "kparm",                       // mol / m^2 / s
        "theta",                       // dimensionless
        "beta",                        // dimensionless
        "Rd",                          // micromol / m^2 / s
        "b0",                          // mol / m^2 / s
        "b1",                          // dimensionless
        "Gs_min",                      // mol / m^2 / s
        "StomataWS",                   // dimensionless
        "Catm",                        // micromol / mol
        "atmospheric_pressure",        // Pa
        "upperT",                      // deg. C
        "lowerT",                      // deg. C
        "average_absorbed_shortwave",  // J / (m^2 leaf) / s
        "absorbed_shortwave",          // J / (m^2 leaf) / s
        "windspeed",                   // m / s
        "leafwidth",                   // m
        "specific_heat_of_air",        // J / kg / K
        "minimum_gbw",                 // mol / m^2 / s
        "et_equation"                  // a dimensionless switch
    };
}

string_vector c4_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",             // micromol / m^2 /s
        "GrossAssim",        // micromol / m^2 /s
        "Rp",                // micromol / m^2 / s
        "Ci",                // micromol / mol
        "Gs",                // mmol / m^2 / s
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
            .Gs;  // mmol / m^2 / s

    // Calculate a new value for leaf temperature
    const ET_Str et =
        EvapoTrans2(
            absorbed_shortwave, average_absorbed_shortwave, ambient_temperature, rh, windspeed,
            initial_stomatal_conductance, leafwidth, specific_heat_of_air,
            minimum_gbw, et_equation);

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
