#include <cmath>                        // for pow, sqrt, std::abs
#include <algorithm>                    // for std::min
#include "ball_berry_gs.h"              // for ball_berry_gs
#include "FvCB_assim.h"                 // for FvCB_assim
#include "conductance_limited_assim.h"  // for conductance_limited_assim
#include "c3_temperature_response.h"    // for c3_temperature_response
#include "../framework/constants.h"     // for dr_stomata, dr_boundary
#include "c3photo.h"
#include "secant_method.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

photosynthesis_outputs c3photoC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,                // micromol / m^2 / s
    double const Tleaf,                        // degrees C
    double const Tambient,                     // degrees C
    double const RH,                           // dimensionless
    double const Vcmax0,                       // micromol / m^2 / s
    double const Jmax0,                        // micromol / m^2 / s
    double const TPU_rate_max,                 // micromol / m^2 / s
    double const Rd0,                          // micromol / m^2 / s
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Ca,                           // micromol / mol
    double const AP,                           // Pa (TEMPORARILY UNUSED)
    double const O2,                           // millimol / mol (atmospheric oxygen mole fraction)
    double const StomWS,                       // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const gbw                           // mol / m^2 / s
)
{
    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    double const dark_adapted_phi_PSII = c3_param.phi_PSII;  // dimensionless
    double const Gstar = c3_param.Gstar;                     // micromol / mol
    double const Jmax = Jmax0 * c3_param.Jmax_norm;          // micromol / m^2 / s
    double const Kc = c3_param.Kc;                           // micromol / mol
    double const Ko = c3_param.Ko;                           // mmol / mol
    double const Rd = Rd0 * c3_param.Rd_norm;                // micromol / m^2 / s
    double const theta = c3_param.theta;                     // dimensionless
    double const TPU = TPU_rate_max * c3_param.Tp_norm;      // micromol / m^2 / s
    double const Vcmax = Vcmax0 * c3_param.Vcmax_norm;       // micromol / m^2 / s

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
    double const I2 =
        absorbed_ppfd * dark_adapted_phi_PSII * beta_PSII;  // micromol / m^2 / s

    double const J =
        (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta * I2 * Jmax)) /
        (2.0 * theta);  // micromol / m^2 / s

    double const Oi = O2 * solo(Tleaf);  // mmol / mol

    // The alpha constant for calculating Ap is from Eq. 2.26, von Caemmerer, S.
    // Biochemical models of leaf photosynthesis.
    double const alpha_TPU = 0.0;  // dimensionless. Without more information, alpha=0 is often assumed.

    // Adjust Ball-Berry parameters in response to water stress
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);
    double const b1_adj = StomWS * b1;

    // Initialize variables before running fixed point iteration in a loop
    // these are updated as a side effect in the secant method iterations
    FvCB_outputs FvCB_res;
    stomata_outputs BB_res;
    double Gs{1e3};  // mol / m^2 / s      (initial guess)
    double Ci{0.0};  // micromol / mol     (initial guess)

    // this lambda function equals zero
    // only if assim satisfies both FvCB and Ball Berry model
    auto check_assim_rate = [&](double assim) {
        // If assim is correct, then Ball Berry gives the correct
        // CO2 at leaf surface (Cs) and correct stomatal conductance
        BB_res = ball_berry_gs(
            assim * 1e-6,
            Ca * 1e-6,
            RH,
            b0_adj,
            b1_adj,
            gbw,
            Tleaf,
            Tambient);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Using the value of stomatal conductance,
        // Calculate Ci using the total conductance across the boundary layer
        // and stomata
        Ci = Ca - assim *
                      (dr_boundary / gbw + dr_stomata / Gs);  // micromol / mol

        // Using Ci compute the assim under the FvCB
        FvCB_res = FvCB_assim(
            Ci, Gstar, J, Kc, Ko, Oi, Rd, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation);

        return FvCB_res.An - assim;  // equals zero if correct
    };

    // compute upper bound as guess
    double assim_ub = FvCB_assim(
                          Ca, Gstar, J, Kc, Ko, Oi, Rd, TPU, Vcmax, alpha_TPU,
                          electrons_per_carboxylation,
                          electrons_per_oxygenation)
                          .An;
    if (assim_ub < 0) {
        assim_ub = 0;
    }
    assim_ub = std::min(Ca * gbw / dr_boundary, assim_ub);

    secant_parameters secpar;
    double co2_assim_rate =
        find_root_secant_method(
            check_assim_rate, Rd, assim_ub, secpar);

    return photosynthesis_outputs{
        /* .Assim = */ co2_assim_rate,         // micromol / m^2 / s
        /* .Assim_check = */ secpar.check,     // micromol / m^2 / s
        /* .Ci = */ Ci,                        // micromol / mol
        /* .GrossAssim = */ FvCB_res.Vc,       // micromol / m^2 / s
        /* .Gs = */ Gs,                        // mol / m^2 / s
        /* .Cs = */ BB_res.cs,                 // micromol / m^2 / s
        /* .RHs = */ BB_res.hs,                // dimensionless from Pa / Pa
        /* .Rp = */ FvCB_res.Vc * Gstar / Ci,  // micromol / m^2 / s
        /* .iterations = */ secpar.counter     // not a physical quantity
    };
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
