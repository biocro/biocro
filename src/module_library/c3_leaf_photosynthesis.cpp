#include "c3_temperature_response.h"      // for c3_temperature_response_parameters
#include "c3photo.h"                      // for c3photoC, solve_c3_gs
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "c3_leaf_photosynthesis.h"

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
        "gm_at_25",                     // mol / m^2 / s / Pa
        "gm_Ha",                        // J / mol
        "gm_Hd",                        // J / mol
        "gm_S",                         // J / K / mol
        "Gs_min",                       // mol / m^2 / s
        "Gstar_at_25",                  // micromol / mol
        "Gstar_Ea",                     // J / mol
        "height",                       // m
        "Jmax_at_25",                   // micromol / m^2 / s
        "Jmax_Ea",                      // J / mol
        "Kc_at_25",                     // micromol / mol
        "Kc_Ea",                        // J / mol
        "Ko_at_25",                     // mmol / mol
        "Ko_Ea",                        // J / mol
        "leafwidth",                    // m
        "O2",                           // mmol / mol
        "phi_PSII_0",                   // dimensionless
        "phi_PSII_1",                   // (degrees C)^(-1)
        "phi_PSII_2",                   // (degrees C)^(-2)
        "rh",                           // dimensionless
        "RL_at_25",                     // micromol / m^2 / s
        "RL_Ea",                        // J / mol
        "StomataWS",                    // dimensionless
        "temp",                         // degrees C
        "theta_0",                      // dimensionless
        "theta_1",                      // (degrees C)^(-1)
        "theta_2",                      // (degrees C)^(-2)
        "Tp_at_25",                     // micromol / m^2 / s
        "Tp_Ha",                        // J / mol
        "Tp_Hd",                        // J / mol
        "Tp_S",                         // J / K / mol
        "Vcmax_at_25",                  // micromol / m^2 / s
        "Vcmax_Ea",                     // J / mol
        "windspeed"                     // m / s
    };
}

string_vector c3_leaf_photosynthesis::get_outputs()
{
    return {
        "Assim",             // micromol / m^2 /s
        "Cc",                // micromol / mol
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
        "iteration_C3_Gs",   // not a physical quantity
        "residual_C3_Gs"     // mol / m^2 / s
    };
}

void c3_leaf_photosynthesis::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        gm_Ha,
        gm_Hd,
        gm_S,
        Gstar_Ea,
        Jmax_Ea,
        Kc_Ea,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        RL_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_Ea};

    // Solve for gs
    root_finding::result_t const result = solve_c3_gs(
        tr_param,
        absorbed_longwave,            // J / (m^2 leaf) / s
        absorbed_ppfd,                // micromol / (m^2 leaf) / s
        absorbed_shortwave,           // J / (m^2 leaf) / s
        ambient_temperature,          // degrees C
        atmospheric_pressure,         // Pa
        b0,                           // mol / m^2 / s
        b1,                           // dimensionless
        beta_PSII,                    // dimensionless
        Catm,                         // micromol / mol
        electrons_per_carboxylation,  // self-explanatory units
        electrons_per_oxygenation,    // self-explanatory units
        gbw_canopy,                   // m / s
        gm_at_25,                     // mol / m^2 / s / Pa
        Gs_min,                       // mol / m^2 / s
        Gstar_at_25,                  // micromol / mol
        Jmax_at_25,                   // micromol / m^2 / s
        Kc_at_25,                     // micromol / mol
        Ko_at_25,                     // mmol / mol
        leafwidth,                    // m
        O2,                           // mmol / mol
        rh,                           // dimensionless
        RL_at_25,                     // micromol / m^2 / s
        StomataWS,                    // dimensionless
        Tp_at_25,                     // micromol / m^2 / s
        Vcmax_at_25,                  // micromol / m^2 / s
        windspeed                     // m / s
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

    photosynthesis_outputs const photo = c3photoC(
        tr_param,
        absorbed_ppfd,
        Tleaf,
        ambient_temperature,
        rh,
        gm_at_25,
        Gstar_at_25,
        Kc_at_25,
        Ko_at_25,
        Vcmax_at_25,
        Jmax_at_25,
        Tp_at_25,
        RL_at_25,
        b0,
        b1,
        Gs_min,
        Catm,
        atmospheric_pressure,
        O2,
        StomataWS,
        electrons_per_carboxylation,
        electrons_per_oxygenation,
        beta_PSII,
        et.gbw_molar);

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(Cc_op, photo.Cc);
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
    update(iteration_C3_Gs_op, result.iteration);
    update(residual_C3_Gs_op, result.residual);
}
