#include <algorithm>                      // for std::min
#include <cmath>                          // for pow, sqrt
#include <limits>                         // for std::numeric_limits
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "c3_temperature_response.h"      // for c3_temperature_response
#include "conductance_helpers.h"          // for sequential_conductance
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "FvCB_assim.h"                   // for FvCB_assim
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "c3photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

double const inf = std::numeric_limits<double>::infinity();

/**
 *  @brief Calculates a difference in net CO2 assimilation rate; this function
 *  will return zero only if Cc satisfies the FvCB + Ball-Berry + 1D gas flow
 *  equations.
 */
double check_c3_assim_rate(
    double const alpha_TPU,                    // dimensionless
    double const b0_adj,                       // mol / m^2 / s
    double const b1_adj,                       // dimensionless
    double const Ca,                           // micromol / mol
    double const Cc,                           // micromol / mol
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw,                          // mol / m^2 / s
    double const gm,                           // mol / m^2 / s
    double const Gstar,                        // micromol / mol
    double const J,                            // micromol / m^2 / s
    double const Kc,                           // micromol / mol
    double const Ko,                           // mmol / mol
    double const Oi,                           // mmol / mol
    double const RH,                           // dimensionless
    double const RL,                           // micromol / m^2 / s
    double const Tambient,                     // degrees C
    double const Tleaf,                        // degrees C
    double const TPU,                          // micromol / m^2 / s
    double const Vcmax                         // micromol / m^2 / s
)
{
    // Use Cc to compute the assimilation rate according to the FvCB model.
    FvCB_outputs const FvCB_res = FvCB_assim(
        Cc,
        Gstar,
        J,
        Kc,
        Ko,
        Oi,
        RL,
        TPU,
        Vcmax,
        alpha_TPU,
        electrons_per_carboxylation,
        electrons_per_oxygenation);

    double const Assim = FvCB_res.An;  // micromol / m^2 / s

    // Use Assim to compute the stomatal conductance according to the
    // Ball-Berry model. If Assim is too high, Cs will take a negative
    // value, which is not allowed by the Ball-Berry model. To avoid this,
    // we clamp Assim to the value that produces Cc = 0, which will also
    // ensure that Cs > 0.
    stomata_outputs const BB_res = ball_berry_gs(
        std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
        Ca * 1e-6,
        RH,
        b0_adj,
        b1_adj,
        gbw,
        Tleaf,
        Tambient);

    double const Gs = BB_res.gsw;  // mol / m^2 / s

    // Using Cc, gm, Gbw, and Gs, make a new estimate of the assimilation
    // rate. If the initial value of Cc was correct, this should be
    // identical to Assim.
    double const Gt =
        sequential_conductance({gbw / dr_boundary, Gs / dr_stomata, gm});  // mol / m^2 / s

    return Assim - Gt * (Ca - Cc);  // micromol / m^2 / s
};

/**
 *  @brief Solves for An, Cc, Ci, and gs
 *
 *  The Dekker method is used to solve the set of coupled equations, because of
 *  known convergence issues when using fixed-point iteration, based on Sun et
 *  al. (2012) "A numerical issue in calculating the coupled carbon and water
 *  fluxes in a climate model." *Journal of Geophysical Research*
 *  https://dx.doi.org/10.1029/2012JD018059
 */
photosynthesis_outputs c3photoC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,                // micromol / m^2 / s
    double const Tleaf,                        // degrees C
    double const Tambient,                     // degrees C
    double const RH,                           // dimensionless
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gstar_at_25,                  // micromol / mol
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const TPU_rate_max,                 // micromol / m^2 / s
    double const RL_at_25,                     // micromol / m^2 / s
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Ca,                           // micromol / mol
    double const atmospheric_pressure,         // Pa
    double const O2,                           // millimol / mol (atmospheric oxygen mole fraction)
    double const StomWS,                       // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const gbw                           // mol / m^2 / s
)
{
    // Check inputs
    if (absorbed_ppfd < 0) {
        throw std::out_of_range("Input `absorbed_ppfd` cannot be negative. Check `solar` is not negative.");
    }

    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    double const dark_adapted_phi_PSII = c3_param.phi_PSII;                // dimensionless
    double const gm = gm_at_25 * c3_param.gm_norm * atmospheric_pressure;  // mol / m^2 / s
    double const Gstar = Gstar_at_25 * c3_param.Gstar_norm;                // micromol / mol
    double const Jmax = Jmax_at_25 * c3_param.Jmax_norm;                   // micromol / m^2 / s
    double const Kc = Kc_at_25 * c3_param.Kc_norm;                         // micromol / mol
    double const Ko = Ko_at_25 * c3_param.Ko_norm;                         // mmol / mol
    double const RL = RL_at_25 * c3_param.RL_norm;                         // micromol / m^2 / s
    double const theta = c3_param.theta;                                   // dimensionless
    double const TPU = TPU_rate_max * c3_param.Tp_norm;                    // micromol / m^2 / s
    double const Vcmax = Vcmax_at_25 * c3_param.Vcmax_norm;                // micromol / m^2 / s

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
    double I2 = absorbed_ppfd * dark_adapted_phi_PSII * beta_PSII;  // micromol / m^2 / s

    double const J =
        (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) /
        (2.0 * theta);  // micromol / m^2 / s

    double const Oi = O2 * solo(Tleaf);  // mmol / mol

    // The alpha constant for calculating Ap is from Eq. 2.26, von Caemmerer, S.
    // Biochemical models of leaf photosynthesis.
    double const alpha_TPU = 0.0;  // dimensionless. Without more information, alpha=0 is often assumed.

    // Adjust Ball-Berry parameters in response to water stress
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);  // mol / m^2 / s
    double const b1_adj = StomWS * b1;                            // dimensionless

    // Use partial application to fix all inputs to `check_c3_assim_rate` except
    // Cc. To solve the photosynthesis equations, a root of this function must
    // be found.
    auto check_c3_assim_rate_partial = [=](double const Cc) {
        return check_c3_assim_rate(
            alpha_TPU,                    // dimensionless
            b0_adj,                       // mol / m^2 / s
            b1_adj,                       // dimensionless
            Ca,                           // micromol / mol
            Cc,                           // micromol / mol
            electrons_per_carboxylation,  // self-explanatory units
            electrons_per_oxygenation,    // self-explanatory units
            gbw,                          // mol / m^2 / s
            gm,                           // mol / m^2 / s
            Gstar,                        // micromol / mol
            J,                            // micromol / m^2 / s
            Kc,                           // micromol / mol
            Ko,                           // mmol / mol
            Oi,                           // mmol / mol
            RH,                           // dimensionless
            RL,                           // micromol / m^2 / s
            Tambient,                     // degrees C
            Tleaf,                        // degrees C
            TPU,                          // micromol / m^2 / s
            Vcmax                         // micromol / m^2 / s
        );
    };

    // Get an upper bound for Cc by finding the most negative value of An (which
    // occurs when Cc = 0), the smallest total conductance to CO2 (which occurs
    // when gsw takes its minimum value b0), and then using Cc = Ca - An / gtc.
    double const A_min =
        FvCB_assim(
            0.0, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation)
            .An;  // micromol / m^2 / s

    double const g_min =
        sequential_conductance({gbw / dr_boundary, b0_adj / dr_stomata, gm});  // mol / m^2 / s

    double const Cc_max = Ca - A_min / g_min;  // micromol / mol

    // Run the Dekker method
    using namespace root_finding;
    dekker solve{500, 1e-12, 1e-12};
    result_t result = solve(
        check_c3_assim_rate_partial,
        0.718 * Ca,    // guess
        0,             // lower
        Cc_max * 1.01  // upper
    );

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "Cc solver reports failed convergence:\n    " +
            result.message());
    }

    // Get final values
    double const Cc = result.root;  // micromol / mol

    FvCB_outputs const FvCB_res = FvCB_assim(
        Cc,
        Gstar,
        J,
        Kc,
        Ko,
        Oi,
        RL,
        TPU,
        Vcmax,
        alpha_TPU,
        electrons_per_carboxylation,
        electrons_per_oxygenation);

    double const Assim = FvCB_res.An;  // micromol / m^2 / s

    stomata_outputs const BB_res = ball_berry_gs(
        std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
        Ca * 1e-6,
        RH,
        b0_adj,
        b1_adj,
        gbw,
        Tleaf,
        Tambient);

    double const Gs = BB_res.gsw;                                          // mol / m^2 / s
    double const Ci = Cc + Assim / gm;                                     // micromol / mol
    double const an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

    return photosynthesis_outputs{
        /* .Assim = */ Assim,                       // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Cc = */ Cc,                             // micromol / mol
        /* .Ci = */ Ci,                             // micromol / mol
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .GrossAssim = */ FvCB_res.Vc,            // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .RL = */ RL,                             // micromol / m^2 / s
        /* .Rp = */ FvCB_res.Vc * Gstar / Cc,       // micromol / m^2 / s
        /* .residual = */ result.residual,          // micromol / m^2 / s
        /* .iteration = */ result.iteration         // not a physical quantity
    };
}

/**
 *  @brief Calculates a difference in stomatal conductance; this function will
 *  return zero only if Gs satisfies the energy balance + FvCB + Ball-Berry + 1D
 *  gas flow equations.
 */
double check_c3_gs(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / (m^2 leaf) / s
    double const absorbed_ppfd,                // micromol / (m^2 leaf) / s
    double const absorbed_shortwave,           // J / (m^2 leaf) / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless
    double const Catm,                         // micromol / mol
    double const current_gs,                   // mol / m^2 / s
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // mol / m^2 / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const leafwidth,                    // m
    double const O2,                           // mmol / mol
    double const rh,                           // dimensionless
    double const RL_at_25,                     // micromol / m^2 / s
    double const StomataWS,                    // dimensionless
    double const Tp_at_25,                     // micromol / m^2 / s
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const windspeed                     // m / s
)
{
    // Solve energy balance with current gs
    energy_balance_outputs const et = leaf_energy_balance(
        absorbed_longwave,
        absorbed_shortwave,
        atmospheric_pressure,
        ambient_temperature,
        gbw_canopy,
        leafwidth,
        rh,
        current_gs,
        windspeed);

    // Get new leaf temperature
    double const current_Tleaf = ambient_temperature + et.Deltat;  // degrees C

    // Recalculate gs with current Tleaf
    photosynthesis_outputs const photo = c3photoC(
        tr_param,
        absorbed_ppfd,
        current_Tleaf,
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

    return photo.Gs - current_gs;  // mol / m^2 / s
};

root_finding::result_t solve_c3_gs(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / (m^2 leaf) / s
    double const absorbed_ppfd,                // micromol / (m^2 leaf) / s
    double const absorbed_shortwave,           // J / (m^2 leaf) / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless
    double const Catm,                         // micromol / mol
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // mol / m^2 / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const leafwidth,                    // m
    double const O2,                           // mmol / mol
    double const rh,                           // dimensionless
    double const RL_at_25,                     // micromol / m^2 / s
    double const StomataWS,                    // dimensionless
    double const Tp_at_25,                     // micromol / m^2 / s
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const windspeed                     // m / s
)
{
    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    double const initial_stomatal_conductance =
        c3photoC(
            tr_param,
            absorbed_ppfd,
            ambient_temperature,
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
            beta_PSII, gbw_guess)
            .Gs;  // mol / m^2 / s

    // Use partial application to fix all inputs to `check_c3_gs` except
    // current_gs. To solve the photosynthesis equations, a root of this
    // function must be found.
    auto check_c3_gs_partial = [=](double const current_gs) {
        return check_c3_gs(
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
            current_gs,                   // mol / m^2 / s
            electrons_per_carboxylation,  // self-explanatory units
            electrons_per_oxygenation,    // self-explanatory units
            gbw_canopy,                   // mol / m^2 / s
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
    };

    // Run the Dekker method
    using namespace root_finding;
    dekker solve(50, 1e-3, 1e-3);
    result_t result = solve(
        check_c3_gs_partial,
        initial_stomatal_conductance,             // guess
        Gs_min,                                   // lower
        100 * initial_stomatal_conductance + 0.1  // upper
    );

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "c3_leaf_photosynthesis solver reports failed convergence:\n    " +
            result.message());
    }

    return result;
}

// This function returns the solubility of O2 in H2O relative to its value at
// 25 degrees C. The equation used here was developed by forming a polynomial
// fit to tabulated solubility values from a reference book, and then a
// subsequent normalization to the return value at 25 degrees C. For more
// details, See Long, Plant, Cell & Environment 14, 729–739 (1991)
// (https://doi.org/10.1111/j.1365-3040.1991.tb01439.x).
double solo(
    double LeafT  // degrees C
)
{
    return (0.047 - 0.0013087 * LeafT + 2.5603e-05 * pow(LeafT, 2) - 2.1441e-07 * pow(LeafT, 3)) / 0.026934;
}
