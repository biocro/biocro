#include "../math/roots/onedim/fixed_point.h"  // for fixed_point
#include "c3_temperature_response.h"           // for c3_temperature_response_parameters
#include "c3photo.h"                           // for c3photoC
#include "leaf_energy_balance.h"               // for leaf_energy_balance
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
        "numit",             // not a physical quantity
        "residual"           // mol / m^2 / s
    };
}

void c3_leaf_photosynthesis::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
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

    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    double const initial_stomatal_conductance =
        c3photoC(
            tr_param, absorbed_ppfd, ambient_temperature, ambient_temperature,
            rh, Gstar_at_25, Kc_at_25, Ko_at_25, Vcmax_at_25, Jmax_at_25,
            Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure, O2,
            StomataWS, electrons_per_carboxylation, electrons_per_oxygenation,
            beta_PSII, gbw_guess)
            .Gs;  // mol / m^2 / s

    photosynthesis_outputs photo;
    energy_balance_outputs et;

    // 1. Set convergence criteria
    root_finding::fixed_point solver(50, 1e-3, 1e-3);

    auto func = [=, &photo, &et](double current_gs) {
        // 2. Solve Energy Balance with current g_s
        et = leaf_energy_balance(
            absorbed_longwave,
            absorbed_shortwave,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leafwidth,
            rh,
            current_gs,
            windspeed);

        double current_Tleaf = ambient_temperature + et.Deltat;  // degrees C

        // 3. Recalculate g_s with current Tleaf
        photo =
            c3photoC(
                tr_param, absorbed_ppfd, current_Tleaf, ambient_temperature,
                rh, Gstar_at_25, Kc_at_25, Ko_at_25, Vcmax_at_25, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure, O2,
                StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation, beta_PSII,
                et.gbw_molecular);

        return photo.Gs;
    };

    using namespace root_finding;
    result_t result = solver.solve(func, initial_stomatal_conductance);

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "c3_leaf_photosynthesis solver reports failed convergence with termination flag:\n    " +
            flag_message(result.flag));
    }

    // Update the outputs
    update(Assim_op, photo.Assim);
    update(Ci_op, photo.Ci);
    update(Cs_op, photo.Cs);
    update(EPenman_op, et.EPenman);
    update(EPriestly_op, et.EPriestly);
    update(gbw_op, et.gbw_molecular);
    update(GrossAssim_op, photo.GrossAssim);
    update(Gs_op, photo.Gs);
    update(leaf_temperature_op, ambient_temperature + et.Deltat);
    update(RHs_op, photo.RHs);
    update(RH_canopy_op, et.RH_canopy);
    update(RL_op, photo.RL);
    update(Rp_op, photo.Rp);
    update(TransR_op, et.TransR);
    update(numit_op, result.iteration);
    update(residual_op, result.residual);
}
