#include <algorithm>                      // for std::min
#include <cmath>                          // for pow, sqrt
#include <limits>                         // for std::numeric_limits
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "c3_temperature_response.h"      // for c3_temperature_response
#include "conductance_helpers.h"          // for sequential_conductance
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "FvCB_assim.h"                   // for FvCB_assim
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "c3photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;
/*

  The secant method is used to solve for assimilation, Ci, and stomatal conductance,
  because of known convergence issues when using fixed-point iteration, based on
  Sun et al. (2012) "A numerical issue in calculating the coupled carbon and
  water fluxes in a climate model." *Journal of Geophysical Research*
  https://dx.doi.org/10.1029/2012JD018059

*/

photosynthesis_outputs c3photoC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,                // micromol / m^2 / s
    double const Tleaf,                        // degrees C
    double const Tambient,                     // degrees C
    double const RH,                           // dimensionless
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const TPU_rate_max,                 // micromol / m^2 / s
    double const RL_at_25,                     // micromol / m^2 / s
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
    // Define infinity
    double const inf = std::numeric_limits<double>::infinity();

    // Check inputs
    if (absorbed_ppfd < 0) {
        throw std::out_of_range("Input `absorbed_ppfd` cannot be negative. Check `solar` is not negative.");
    }

    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    double const dark_adapted_phi_PSII = c3_param.phi_PSII;  // dimensionless
    double const Gstar = c3_param.Gstar;                     // micromol / mol
    double const Jmax = Jmax_at_25 * c3_param.Jmax_norm;     // micromol / m^2 / s
    double const Kc = c3_param.Kc;                           // micromol / mol
    double const Ko = c3_param.Ko;                           // mmol / mol
    double const RL = RL_at_25 * c3_param.RL_norm;           // micromol / m^2 / s
    double const theta = c3_param.theta;                     // dimensionless
    double const TPU = TPU_rate_max * c3_param.Tp_norm;      // micromol / m^2 / s
    double const Vcmax = Vcmax_at_25 * c3_param.Vcmax_norm;  // micromol / m^2 / s

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
    double const b0_adj = StomWS * b0 + Gs_min * (1.0 - StomWS);
    double const b1_adj = StomWS * b1;

    // Initialize variables before running fixed point iteration in a loop
    // these are updated as a side effect in the secant method iterations
    FvCB_outputs FvCB_res;
    stomata_outputs BB_res;
    double Gs{1e3};     // mol / m^2 / s  (initial guess)
    double Assim{0.0};  // micromol / mol (initial guess)

    // This lambda function equals zero only if Ci satisfies both the FvCB and
    // Ball-Berry models. Here, Ci should be expressed in micromol / mol.
    auto check_assim_rate = [=, &FvCB_res, &BB_res, &Gs, &Assim](double Ci) {
        // Use Ci to compute the assimilation rate according to the FvCB model.
        FvCB_res = FvCB_assim(
            Ci, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation);

        Assim = FvCB_res.An;  // micromol / m^2 / s

        // Use Assim to compute the stomatal conductance according to the
        // Ball-Berry model. If Assim is too high, Cs will take a negative
        // value, which is not allowed by the Ball-Berry model. To avoid this,
        // we clamp Assim to the value that produces Cs = 0; this will result
        // in Gs = infinity.
        BB_res = ball_berry_gs(
            std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
            Ca * 1e-6,
            RH,
            b0_adj,
            b1_adj,
            gbw,
            Tleaf,
            Tambient);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Using Ci and Gs, make a new estimate of the assimilation rate. If
        // the initial value of Ci was correct, this should be identical to
        // Assim.
        double Gt = sequential_conductance(gbw / dr_boundary, Gs / dr_stomata);  // mol / m^2 / s

        return Assim - Gt * (Ca - Ci);  // micromol / m^2 / s
    };

    // Get an upper bound for Ci by finding the most negative value of An (which
    // occurs when Ci = 0), the smallest total conductance to CO2 (which occurs
    // when gsw takes its minimum value b0), and then using Ci = Ca - An / gtc.
    double const A_min =
        FvCB_assim(
            0.0, Gstar, J, Kc, Ko, Oi, RL, TPU, Vcmax, alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation)
            .An;  // micromol / m^2 / s

    double const Ci_max =
        Ca - A_min * (dr_boundary / gbw + dr_stomata / b0_adj);  // micromol / mol

    // Run the Dekker method
    using namespace root_finding;
    dekker solve{500, 1e-12, 1e-12};
    result_t result = solve(
        check_assim_rate,
        0.718 * Ca,
        0,
        Ci_max * 1.01);

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "Ci solver reports failed convergence with termination flag:\n    " +
            flag_message(result.flag));
    }

    // Get final values
    double const Ci = result.root;                                         // micromol / mol
    double const an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

    return photosynthesis_outputs{
        /* .Assim = */ Assim,                       // micromol / m^2 / s
        /* .Assim_check = */ result.residual,       // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Ci = */ Ci,                             // micromol / mol
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .GrossAssim = */ FvCB_res.Vc,            // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .RL = */ RL,                             // micromol / m^2 / s
        /* .Rp = */ FvCB_res.Vc * Gstar / Ci,       // micromol / m^2 / s
        /* .iterations = */ result.iteration        // not a physical quantity
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
