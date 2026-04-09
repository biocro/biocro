#ifndef BIOCRO_LEAF_BAL_H
#define BIOCRO_LEAF_BAL_H

#include <algorithm>                    // for std::min
#include <cmath>                        // for pow, sqrt
#include <limits>                       // for std::numeric_limits
#include "../framework/constants.h"     // for dr_stomata, dr_boundary
#include "ball_berry_gs.h"              // for ball_berry_gs
#include "c3_temperature_response.h"    // for c3_temperature_response
#include "conductance_helpers.h"        // for sequential_conductance
#include "conductance_limited_assim.h"  // for conductance_limited_assim
#include "FvCB_assim.h"                 // for FvCB_assim
#include "c3photo.h"
#include "leaf_energy_balance.h"
#include "photosynthesis_outputs.h"
#include "../math/roots/multidim/zeros.h"

auto make_leaf_equation_residual(
    // Ambient / fixed physical quantities
    double const air_temperature,     // degrees C
    double const air_pressure,        // Pa
    double const relative_humidity,   // dimensionless
    double const absorbed_shortwave,  // J / m^2 / s
    double const absorbed_longwave,   // J / m^2 / s
    double const gbw_canopy,          // m / s
    double const layer_wind_speed,    // m / s
    double const leaf_width,          // m
    // Photosynthesis parameters
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,  // micromol / m^2 / s
    double const Gstar_at_25,    // micromol / mol
    double const Kc_at_25,       // micromol / mol
    double const Ko_at_25,       // mmol / mol
    double const Vcmax_at_25,    // micromol / m^2 / s
    double const Jmax_at_25,     // micromol / m^2 / s
    double const TPU_rate_max,   // micromol / m^2 / s
    double const RL_at_25,       // micromol / m^2 / s
    double const b0,             // mol / m^2 / s
    double const b1,             // dimensionless
    double const Gs_min,         // mol / m^2 / s
    double const Ca,             // micromol / mol
    double const O2,             // mmol / mol
    double const StomWS,         // dimensionless
    double const electrons_per_carboxylation,
    double const electrons_per_oxygenation,
    double const beta_PSII  // dimensionless
)
{
    // --- Precompute ambient-only quantities (independent of all unknowns) ---

    double const inf = std::numeric_limits<double>::infinity();

    // Check inputs
    if (absorbed_ppfd < 0) {
        throw std::out_of_range("Input `absorbed_ppfd` cannot be negative. Check `solar` is not negative.");
    }

    // The variable that we call `I2` here has been described as "the useful
    // light absorbed by photosystem II" (S. von Caemmerer (2002)) and "the
    // maximum fraction of incident quanta that could be utilized in electron
    // transport" (Bernacchi et al. (2003)). Here we calculate its value using
    // Equation 3 from Bernacchi et al. (2003), except that we have replaced the
    // factor `Q * alpha_leaf` (the product of the incident PPFD `Q` and the
    // leaf absorptance) with the absorbed PPFD, as this is clearly the intended
    // meaning of the `Q * alpha_leaf` factor. See also Equation 8 from the
    // original FvCB paper, where `J` (equivalent to our `I2`) is proportional
    // to the absorbed PPFD rather than the incident PPFD.

    // The alpha constant for calculating Ap is from Eq. 2.26, von Caemmerer, S.
    // Biochemical models of leaf photosynthesis.
    double const alpha_TPU = 0.0;  // dimensionless. Without more information, alpha=0 is often assumed.

    // Adjust Ball-Berry parameters in response to water stress
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);
    double const b1_adj = StomWS * b1;

    // double constexpr epsilon_s = 1.0;  // dimensionless
    auto check_leaf_temp = make_check_leaf_temp(
        absorbed_longwave,
        absorbed_shortwave, air_pressure, air_temperature, gbw_canopy, leaf_width,
        relative_humidity, layer_wind_speed);
    // Return the residual function, capturing all precomputed values
    return [=](std::array<double, 2> x) -> std::array<double, 2> {
        double const Ci = x[0];     // micromol / mol
        double const Tleaf = x[1];  // degrees C

        // Temperature-adjusted photosynthesis parameters
        c3_param_at_tleaf const c3_param = c3_temperature_response(tr_param, Tleaf);

        double const Gstar = Gstar_at_25 * c3_param.Gstar_norm;  // micromol / mol
        double const Jmax = Jmax_at_25 * c3_param.Jmax_norm;     // micromol / m^2 / s
        double const Kc = Kc_at_25 * c3_param.Kc_norm;           // micromol / mol
        double const Ko = Ko_at_25 * c3_param.Ko_norm;           // mmol / mol
        double const RL = RL_at_25 * c3_param.RL_norm;           // micromol / m^2 / s
        double const theta = c3_param.theta;                     // dimensionless
        double const TPU = TPU_rate_max * c3_param.Tp_norm;      // micromol / m^2 / s
        double const Vcmax = Vcmax_at_25 * c3_param.Vcmax_norm;  // micromol / m^2 / s

        double const I2 = absorbed_ppfd * c3_param.phi_PSII * beta_PSII;                                   // micromol / m^2 / s
        double const J = (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) / (2.0 * theta);  // micromol / m^2 / s
        double const Oi = O2 * solo(Tleaf);                                                                // mmol / mol

        // Boundary layer conductance, consistent with current T_leaf
        double const gbw_leaf_ms = calculate_gbw_leaf(
            air_pressure, air_temperature, Tleaf,
            leaf_width, layer_wind_speed);                                      // m / s
        double const gbw_ms = sequential_conductance(gbw_leaf_ms, gbw_canopy);  // m / s
        double const gbw_mol = g_to_molecular(air_pressure,
                                              gbw_ms, Tleaf);  // mol / m^2 / s

        // F[0]: FvCB conductance residual — An(Ci) = Gt * (Ca - Ci)
        FvCB_outputs const FvCB_res = FvCB_assim(
            Ci, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax,
            TPU,
            electrons_per_carboxylation, electrons_per_oxygenation);
        double const An = FvCB_res.An;  // micromol / m^2 / s
        double const An_clamped = std::min(
            An, conductance_limited_assim(Ca, gbw_mol, inf));
        stomata_outputs const BB_res = ball_berry_gs(
            An_clamped * 1e-6, Ca * 1e-6, relative_humidity,
            b0_adj, b1_adj, gbw_mol,
            Tleaf, air_temperature);
        using namespace physical_constants;
        double const Gt = sequential_conductance(gbw_mol / dr_boundary,
                                                 BB_res.gsw / dr_stomata);  // mol / m^2 / s
        double const F0 = An - Gt * (Ca - Ci);                              // micromol / m^2 / s

        // F[1]: Energy balance residual — Penman-Monteith T_leaf(gs) = T_leaf
        double const F1 = check_leaf_temp(Tleaf, BB_res.gsw);  // degrees C

        // Normalize so all residuals are O(1) for Broyden's method
        return {
            F0 / 50.0,  // typical max An  ~50 micromol / m^2 / s
            F1 / 5.0    // typical max ΔT  ~5 degrees C
        };
    };
}

photosynthesis_outputs unpack_c3_solution(
    root_multidim::result_t<2> solution,
    // The same parameters passed to make_c3_energy_residual
    double const air_temperature,
    double const air_pressure,
    double const relative_humidity,
    double const Ca,
    double const O2,
    double const StomWS,
    double const b0,
    double const b1,
    double const Gs_min,
    double const gbw_canopy,
    double const layer_wind_speed,
    double const leaf_width,
    double const RL_at_25,
    double const Gstar_at_25,
    double const Kc_at_25,
    double const Ko_at_25,
    double const Vcmax_at_25,
    double const Jmax_at_25,
    double const TPU_rate_max,
    double const absorbed_ppfd,
    double const beta_PSII,
    double const electrons_per_carboxylation,
    double const electrons_per_oxygenation,
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,
    double const absorbed_shortwave)
{
    double const Ci = solution.zero[0];     // micromol / mol
    double const Tleaf = solution.zero[1];  // degrees C
    auto check_leaf_temp = make_check_leaf_temp(
        absorbed_longwave,
        absorbed_shortwave, air_pressure, air_temperature, gbw_canopy, leaf_width,
        relative_humidity, layer_wind_speed);
    // Adjust Ball-Berry parameters in response to water stress
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);
    double const b1_adj = StomWS * b1;

    // Temperature-adjusted photosynthesis parameters
    c3_param_at_tleaf const c3_param = c3_temperature_response(tr_param, Tleaf);

    double const Gstar = Gstar_at_25 * c3_param.Gstar_norm;  // micromol / mol
    double const Jmax = Jmax_at_25 * c3_param.Jmax_norm;     // micromol / m^2 / s
    double const Kc = Kc_at_25 * c3_param.Kc_norm;           // micromol / mol
    double const Ko = Ko_at_25 * c3_param.Ko_norm;           // mmol / mol
    double const RL = RL_at_25 * c3_param.RL_norm;           // micromol / m^2 / s
    double const theta = c3_param.theta;                     // dimensionless
    double const TPU = TPU_rate_max * c3_param.Tp_norm;      // micromol / m^2 / s
    double const Vcmax = Vcmax_at_25 * c3_param.Vcmax_norm;  // micromol / m^2 / s

    double const I2 = absorbed_ppfd * c3_param.phi_PSII * beta_PSII;                                   // micromol / m^2 / s
    double const J = (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) / (2.0 * theta);  // micromol / m^2 / s
    double const Oi = O2 * solo(Tleaf);                                                                // mmol / mol

    // Boundary layer conductance, consistent with current T_leaf
    double const gbw_leaf_ms = calculate_gbw_leaf(
        air_pressure, air_temperature, Tleaf,
        leaf_width, layer_wind_speed);                                      // m / s
    double const gbw_ms = sequential_conductance(gbw_leaf_ms, gbw_canopy);  // m / s
    double const gbw_mol = g_to_molecular(air_pressure,
                                          gbw_ms, Tleaf);  // mol / m^2 / s

    // F[0]: FvCB conductance residual — An(Ci) = Gt * (Ca - Ci)
    FvCB_outputs const FvCB_res = FvCB_assim(
        Ci, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax,
        /*alpha_TPU=*/0.0,
        electrons_per_carboxylation, electrons_per_oxygenation);
    double const An = FvCB_res.An;  // micromol / m^2 / s
    stomata_outputs const BB_res = ball_berry_gs(
        An * 1e-6, Ca * 1e-6, relative_humidity,
        b0_adj, b1_adj, gbw_mol,
        Tleaf, air_temperature);

    using namespace physical_constants;
    double const Gt = sequential_conductance(gbw_mol / dr_boundary,
                                             BB_res.gsw / dr_stomata);  // mol / m^2 / s
    double const F0 = An - Gt * (Ca - Ci);                              // micromol / m^2 / s

    // F[1]: Energy balance residual — Penman-Monteith T_leaf(gs) = T_leaf
    double const F1 = check_leaf_temp(Tleaf, BB_res.gsw);  // degrees C

    // Conductance-limited assimilation
    double const Assim_conductance = conductance_limited_assim(Ca, gbw_mol, BB_res.gsw);  // micromol / m^2 / s

    return photosynthesis_outputs{
        /* .Assim =             */ An,
        /* .Assim_conductance = */ Assim_conductance,
        /* .Ci =                */ Ci,
        /* .Cs =                */ BB_res.cs,
        /* .GrossAssim =        */ FvCB_res.Vc,
        /* .Gs =                */ BB_res.gsw,
        /* .RHs =               */ BB_res.hs,
        /* .RL =                */ RL,
        /* .Rp =                */ FvCB_res.Vc * Gstar / Ci,
        /* .residual =          */ solution.residual_norm(),
        /* .iteration =         */ solution.iteration};
}

#endif
