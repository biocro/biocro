#include "c3_leaf_photosynthesis.h"
#include "c3photo.h"              // for c3photoC
#include "leaf_energy_balance.h"  // for leaf_energy_balance

using standardBML::c3_leaf_photosynthesis;

string_vector c3_leaf_photosynthesis::get_inputs()
{
    return {
        "absorbed_longwave",            // J / (m^2 leaf) / s
        "absorbed_ppfd",                // micromol / (m^2 leaf) / s
        "absorbed_shortwave",           // J / (m^2 leaf) / s
        "atmospheric_pressure",         // Pa
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "beta_PSII",                    // dimensionless (fraction of absorbed light that reaches photosystem II)
        "Catm",                         // micromol / mol
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "gbw_canopy",                   // m / s
        "Gs_min",                       // mol / m^2 / s
        "height",                       // m
        "jmax",                         // micromol / m^2 / s
        "leafwidth",                    // m
        "O2",                           // mmol / mol
        "Rd",                           // micromol / m^2 / s
        "rh",                           // dimensionless
        "StomataWS",                    // dimensionless
        "temp",                         // degrees C
        "theta",                        // dimensionless
        "tpu_rate_max",                 // micromol / m^2 / s
        "vmax1",                        // micromol / m^2 / s
        "windspeed"                     // m / s
    };
}

string_vector c3_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",             // micromol / m^2 /s
        "Ci",                // micromol / mol
        "Cs",                // micromol / m^2 / s
        "EPenman",           // mmol / m^2 / s
        "EPriestly",         // mmol / m^2 / s
        "gbw",               // mol / m^2 / s
        "GrossAssim",        // micromol / m^2 /s
        "Gs",                // mol / m^2 / s
        "leaf_temperature",  // degrees C
        "RHs",               // dimensionless from Pa / Pa
        "RH_canopy",         // dimensionless
        "Rp",                // micromol / m^2 / s
        "TransR"             // mmol / m^2 / s
    };
}

void c3_leaf_photosynthesis::do_operation() const
{
    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    double const initial_stomatal_conductance =
        c3photoC(
            absorbed_ppfd, ambient_temperature, ambient_temperature,
            rh, vmax1, jmax, tpu_rate_max, Rd, b0,
            b1, Gs_min, Catm, atmospheric_pressure, O2, theta, StomataWS,
            electrons_per_carboxylation,
            electrons_per_oxygenation, beta_PSII, gbw_guess)
            .Gs;  // mol / m^2 / s

    // Calculate a new value for leaf temperature using the estimate for
    // stomatal conductance
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

    double const leaf_temperature = ambient_temperature + et.Deltat;  // degrees C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    const photosynthesis_outputs photo =
        c3photoC(
            absorbed_ppfd, leaf_temperature, ambient_temperature,
            rh, vmax1, jmax,
            tpu_rate_max, Rd, b0, b1, Gs_min, Catm, atmospheric_pressure, O2,
            theta, StomataWS,
            electrons_per_carboxylation, electrons_per_oxygenation, beta_PSII,
            et.boundary_layer_conductance);

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(Ci_op, photo.Ci);
    update(Cs_op, photo.Cs);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(gbw_op, et.boundary_layer_conductance);
    update(GrossAssim_op, photo.GrossAssim);
    update(Gs_op, photo.Gs);
    update(leaf_temperature_op, leaf_temperature);
    update(RHs_op, photo.RHs);
    update(RH_canopy_op, et.RH_canopy);
    update(Rp_op, photo.Rp);
    update(TransR_op, et.TransR);
}
