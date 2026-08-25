#include "../math/roots/onedim/dekker.h"  // for dekker
#include "c4photo.h"                      // for c4photoC, solve_c4_gs
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "c4_leaf_photosynthesis.h"

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
        "lowerT",                // degrees C
        "rh",                    // dimensionless
        "RL_at_25",              // micromol / m^2 / s
        "StomataWS",             // dimensionless
        "temp",                  // degrees C
        "theta",                 // dimensionless
        "upperT",                // degrees C
        "Vcmax_at_25",           // micromol / m^2 / s
        "windspeed"              // m / s
    };
}

string_vector c4_leaf_photosynthesis::get_outputs()
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
        "RL",                // micromol / m^2 / s
        "Rp",                // micromol / m^2 / s
        "TransR",            // mmol / m^2 / s
        "iteration_C4_Gs",   // not a physical quantity
        "residual_C4_Gs"     // mol / m^2 / s
    };
}

void c4_leaf_photosynthesis::do_operation() const
{
    // Solve for gs
    root_finding::result_t const result = solve_c4_gs(
        absorbed_longwave,     // J / (m^2 leaf) / s
        absorbed_shortwave,    // J / (m^2 leaf) / s
        alpha1,                // mol / mol
        ambient_temperature,   // degrees C
        atmospheric_pressure,  // Pa
        b0,                    // mol / m^2 / s
        b1,                    // dimensionless
        beta,                  // dimensionless
        Catm,                  // micromol / mol
        gbw_canopy,            // m / s
        Gs_min,                // mol / m^2 / s
        incident_ppfd,         // micromol / m^2 / s
        kparm,                 // mol / m^2 / s
        leafwidth,             // m
        lowerT,                // degrees C
        rh,                    // dimensionless
        RL_at_25,              // micromol / m^2 / s
        StomataWS,             // dimensionless
        theta,                 // dimensionless
        upperT,                // degrees C
        Vcmax_at_25,           // micromol / m^2 / s
        windspeed              // m / s
    );

    // Get final values
    double const Gs = result.root;  // mol / m^2 / s

    energy_balance_outputs const et = leaf_energy_balance(
        absorbed_longwave,
        absorbed_shortwave,
        atmospheric_pressure,
        ambient_temperature,
        gbw_canopy,
        leafwidth,
        rh,
        Gs,
        windspeed);

    double const Tleaf = ambient_temperature + et.Deltat;  // degrees C

    photosynthesis_outputs const photo = c4photoC(
        incident_ppfd,
        Tleaf,
        ambient_temperature,
        rh,
        Vcmax_at_25,
        alpha1,
        kparm,
        theta,
        beta,
        RL_at_25,
        b0,
        b1,
        Gs_min,
        StomataWS,
        Catm,
        atmospheric_pressure,
        upperT,
        lowerT,
        et.gbw_molar);

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(Ci_op, photo.Ci);
    update(Cs_op, photo.Cs);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(gbw_op, et.gbw_molar);
    update(GrossAssim_op, photo.GrossAssim);
    update(Gs_op, photo.Gs);
    update(leaf_temperature_op, Tleaf);
    update(RHs_op, photo.RHs);
    update(RH_canopy_op, et.RH_canopy);
    update(RL_op, photo.RL);
    update(Rp_op, photo.Rp);
    update(TransR_op, et.TransR);
    update(iteration_C4_Gs_op, result.iteration);
    update(residual_C4_Gs_op, result.residual);
}
