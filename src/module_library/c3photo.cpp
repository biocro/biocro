#include <cmath>                        // for pow, sqrt, std::abs
#include <algorithm>                    // for std::min
#include "ball_berry_gs.h"              // for ball_berry_gs
#include "FvCB_assim.h"                 // for FvCB_assim
#include "conductance_limited_assim.h"  // for conductance_limited_assim
#include "AuxBioCro.h"                  // for arrhenius_exponential
#include "../framework/constants.h"     // for ideal_gas_constant,
                                        //     celsius_to_kelvin, dr_stomata,
                                        //     dr_boundary
#include "c3photo.h"

using conversion_constants::celsius_to_kelvin;
using physical_constants::dr_boundary;
using physical_constants::dr_stomata;
using physical_constants::ideal_gas_constant;

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
    // Get leaf temperature in Kelvin
    double const Tleaf_K = Tleaf + celsius_to_kelvin;  // K

    // Temperature corrections are from the following sources:
    // - Bernacchi et al. (2003) Plant, Cell and Environment, 26(9), 1419-1430.
    //   https://doi.org/10.1046/j.0016-8025.2003.01050.x
    // - Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259.
    //   https://doi.org/10.1111/j.1365-3040.2001.00668.x
    // Note: Values in Dubois and Bernacchi are incorrect.
    double const Kc = arrhenius_exponential(38.05, 79.43e3, Tleaf_K);              // micromol / mol
    double const Ko = arrhenius_exponential(20.30, 36.38e3, Tleaf_K);              // mmol / mol
    double const Gstar = arrhenius_exponential(19.02, 37.83e3, Tleaf_K);           // micromol / mol
    double const Vcmax = Vcmax0 * arrhenius_exponential(26.35, 65.33e3, Tleaf_K);  // micromol / m^2 / s
    double const Jmax = Jmax0 * arrhenius_exponential(17.57, 43.54e3, Tleaf_K);    // micromol / m^2 / s
    double const Rd = Rd0 * arrhenius_exponential(18.72, 46.39e3, Tleaf_K);        // micromol / m^2 / s

    double const theta = thet + 0.018 * Tleaf - 3.7e-4 * pow(Tleaf, 2);  // dimensionless

    // Light limited
    double const dark_adapted_phi_PSII =
        0.352 + 0.022 * Tleaf - 3.4 * pow(Tleaf, 2) / 1e4;  // dimensionless (Bernacchi et al. (2003))

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

    // TPU rate temperature dependence from Figure 7, Yang et al. (2016) Planta,
    // 243, 687-698. https://doi.org/10.1007/s00425-015-2436-8
    //
    // In Yang et al., the equation in the caption of Figure 7 calculates the
    // maximum rate of TPU utilization, but here we need the rate relative to
    // its value at 25 degrees C (as shown in the figure itself). Using the
    // equation, the rate at 25 degrees C can be found to have the value
    // 306.742, so here we normalize the equation by this value.
    double const TPU_c = 25.5;                                               // dimensionless (fitted constant)
    double const Ha = 62.99e3;                                               // J / mol (enthalpy of activation)
    double const S = 0.588e3;                                                // J / K / mol (entropy)
    double const Hd = 182.14e3;                                              // J / mol (enthalpy of deactivation)
    double const R = ideal_gas_constant;                                     // J / K / mol (ideal gas constant)
    double const top = Tleaf_K * arrhenius_exponential(TPU_c, Ha, Tleaf_K);  // dimensionless
    double const bot = 1.0 + arrhenius_exponential(S / R, Hd, Tleaf_K);      // dimensionless
    double TPU_rate_multiplier = (top / bot) / 306.742;                      // dimensionless

    double TPU = TPU_rate_max * TPU_rate_multiplier;  // micromol / m^2 / s

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

        Gs = 1e-3 * BB_res.gsw;  // mol / m^2 / s

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
        /* .Gs = */ Gs * 1e3,                       // mmol / m^2 / s
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
