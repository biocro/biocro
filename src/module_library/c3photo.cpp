#include <cmath>                             // for pow, sqrt, std::abs
#include <algorithm>                         // for std::min
#include "ball_berry_gs.h"                   // for ball_berry_gs
#include "FvCB_assim.h"                      // for FvCB_assim
#include "conductance_limited_assim.h"       // for conductance_limited_assim
#include "c3_temperature_response.h"         // for c3_temperature_response
#include "temperature_response_functions.h"  // for arrhenius_exponential
#include "../framework/constants.h"          // for dr_stomata, dr_boundary
#include "c3photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

photosynthesis_outputs c3photoC(
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
    double const thet,                         // dimensionless
    double const StomWS,                       // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const gbw                           // mol / m^2 / s
)
{
    // Calculate values of key parameters at leaf temperature. Temperature
    // corrections are from the following sources:
    // - Bernacchi et al. (2003) Plant, Cell and Environment, 26(9), 1419-1430.
    //   https://doi.org/10.1046/j.0016-8025.2003.01050.x
    // - Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259.
    //   https://doi.org/10.1111/j.1365-3040.2001.00668.x
    // - Yang et al. (2016) Planta, 243, 687-698.
    //   https://doi.org/10.1007/s00425-015-2436-8
    // Note: Values in Dubois and Bernacchi are incorrect.
    // Note about Tp_c: The value in Yang et al. (2016) is given as 25.50, but
    //   the value of Tp_norm at 25 degrees C is 306.742 when using this value
    //   of c. Here we use 25.5 - log(306.742) = 19.77399 to ensure Tp_norm = 1
    //   at 25 degrees C.
    c3_param_at_tleaf t_param = c3_temperature_response(
        {/* Gstar_c = */ 19.02,
         /* Gstar_Ea = */ 37.83e3,
         /* Jmax_c = */ 17.57,
         /* Jmax_Ea = */ 43.54e3,
         /* Kc_c = */ 38.05,
         /* Kc_Ea = */ 79.43e3,
         /* Ko_c = */ 20.30,
         /* Ko_Ea = */ 36.38e3,
         /* phi_PSII_0 = */ 0.352,
         /* phi_PSII_1 = */ 0.022,
         /* phi_PSII_2 = */ -3.4e-4,
         /* Rd_c = */ 18.72,
         /* Rd_Ea = */ 46.39e3,
         /* theta_0 = */ thet,
         /* theta_1 = */ 0.018,
         /* theta_2 = */ -3.7e-4,
         /* Tp_c = */ 19.77399,
         /* Tp_Ha = */ 62.99e3,
         /* Tp_Hd = */ 182.14e3,
         /* Tp_S = */ 0.588e3,
         /* Vcmax_c = */ 26.35,
         /* Vcmax_Ea = */ 65.33e3},
        Tleaf);

    double const Kc = t_param.Kc;                           // micromol / mol
    double const Ko = t_param.Ko;                           // mmol / mol
    double const Gstar = t_param.Gstar;                     // micromol / mol
    double const Vcmax = Vcmax0 * t_param.Vcmax_norm;       // micromol / m^2 / s
    double const Jmax = Jmax0 * t_param.Jmax_norm;          // micromol / m^2 / s
    double const Rd = Rd0 * t_param.Rd_norm;                // micromol / m^2 / s
    double const theta = t_param.theta;                     // dimensionless
    double const dark_adapted_phi_PSII = t_param.phi_PSII;  // dimensionless
    double const TPU = TPU_rate_max * t_param.Tp_norm;      // micromol / m^2 / s

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
    FvCB_outputs FvCB_res;
    stomata_outputs BB_res;
    double an_conductance{};            // micromol / m^2 / s
    double Gs{1e3};                     // mol / m^2 / s      (initial guess)
    double Ci{0.0};                     // micromol / mol     (initial guess)
    double co2_assimilation_rate{0.0};  // micromol / m^2 / s (initial guess)
    double const Tol{0.01};             // micromol / m^2 / s
    int iterCounter{0};
    int max_iter{1000};

    // Run iteration loop
    while (iterCounter < max_iter) {
        double OldAssim = co2_assimilation_rate;  // micromol / m^2 / s

        // The net CO2 assimilation is the smaller of the biochemistry-limited
        // and conductance-limited rates. This will prevent the calculated Ci
        // value from ever being < 0. This seems to be an important restriction
        // to prevent numerical errors during the convergence loop, but does not
        // actually limit the net assimilation rate if the loop converges.
        an_conductance =
            conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

        FvCB_res = FvCB_assim(
            Ci,
            Gstar,
            J,
            Kc,
            Ko,
            Oi,
            Rd,
            TPU,
            Vcmax,
            alpha_TPU,
            electrons_per_carboxylation,
            electrons_per_oxygenation);

        co2_assimilation_rate = std::min(FvCB_res.An, an_conductance);  // micromol / m^2 / s

        BB_res = ball_berry_gs(
            co2_assimilation_rate * 1e-6,
            Ca * 1e-6,
            RH,
            b0_adj,
            b1_adj,
            gbw,
            Tleaf,
            Tambient);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Calculate Ci using the total conductance across the boundary layer
        // and stomata
        Ci = Ca - co2_assimilation_rate *
                      (dr_boundary / gbw + dr_stomata / Gs);  // micromol / mol

        if (std::abs(OldAssim - co2_assimilation_rate) < Tol) {
            break;
        }

        ++iterCounter;
    }

    return photosynthesis_outputs{
        /* .Assim = */ co2_assimilation_rate,       // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Ci = */ Ci,                             // micromol / mol
        /* .GrossAssim = */ FvCB_res.Vc,            // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .Rp = */ FvCB_res.Vc * Gstar / Ci,       // micromol / m^2 / s
        /* .iterations = */ iterCounter             // not a physical quantity
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
