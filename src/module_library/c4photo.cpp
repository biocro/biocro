#include <cmath>                          // for pow, exp
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_min
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "secant_method.h"                // for find_root_secant_method
#include "c4photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

/*
  The secant method is used to solve for assimilation, Ci, and stomatal conductance,
  because of known convergence issues when using fixed-point iteration, based on
  Sun et al. (2012) "A numerical issue in calculating the coupled carbon and
  water fluxes in a climate model." *Journal of Geophysical Research*
  https://dx.doi.org/10.1029/2012JD018059

*/
photosynthesis_outputs c4photoC(
    double const Qp,                    // micromol / m^2 / s
    double const leaf_temperature,      // degrees C
    double const ambient_temperature,   // degrees C
    double const relative_humidity,     // dimensionless from Pa / Pa
    double const vmax,                  // micromol / m^2 / s
    double const alpha,                 // mol / mol
    double const kparm,                 // mol / m^2 / s
    double const theta,                 // dimensionless
    double const beta,                  // dimensionless
    double const Rd,                    // micromol / m^2 / s
    double const bb0,                   // mol / m^2 / s
    double const bb1,                   // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double const Gs_min,                // mol / m^2 / s
    double const StomaWS,               // dimensionless
    double const Ca,                    // micromol / mol
    double const atmospheric_pressure,  // Pa
    double const upperT,                // degrees C
    double const lowerT,                // degrees C
    double const gbw                    // mol / m^2 / s
)
{
    constexpr double k_Q10 = 2;  // dimensionless. Increase in a reaction rate per temperature increase of 10 degrees Celsius.

    double const Ca_pa = Ca * 1e-6 * atmospheric_pressure;  // Pa

    double const kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // dimensionless

    // Collatz 1992. Appendix B. Equation set 5B.
    double const Vtn = vmax * pow(2, (leaf_temperature - 25.0) / 10.0);                                              // micromole / m^2 / s
    double const Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double const VT = Vtn / Vtd;                                                                                     // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double const Rtn = Rd * pow(2, (leaf_temperature - 25) / 10);  // micromole / m^2 / s
    double const Rtd = 1 + exp(1.3 * (leaf_temperature - 55));     // dimensionless
    double const RT = Rtn / Rtd;                                   // micromole / m^2 / s

    // Collatz 1992. Appendix B. Quadratic coefficients from Equation 2B.
    double const b0 = VT * alpha * Qp;
    double const b1 = -(VT + alpha * Qp);
    double const b2 = theta;

    // Calculate the smaller of the two quadratic roots, as mentioned following
    // Equation 3B in Collatz 1992.
    double const M = quadratic_root_min(b2, b1, b0);  // micromol / m^2 / s

    // Adjust Ball-Berry parameters in response to water stress
    double const bb0_adj = StomaWS * bb0 + Gs_min * (1.0 - StomaWS);
    double const bb1_adj = StomaWS * bb1;

    // Function to compute the biochemical assimilation rate.
    auto collatz_assim = [=](double const InterCellularCO2) {
        // Collatz 1992. Appendix B. Quadratic coefficients from Equation 3B.
        double kT_IC_P = kT * InterCellularCO2 / atmospheric_pressure * 1e6;  // micromole / m^2 / s
        double a = beta;
        double b = -(M + kT_IC_P);
        double c = M * kT_IC_P;

        // Calculate the smaller of the two quadratic roots, as mentioned
        // following Equation 3B in Collatz 1992.
        double gross_assim = quadratic_root_min(a, b, c);  // micromol / m^2 / s
        return gross_assim - RT;
    };

    // Initialize loop variables. These will be updated as a side effect
    // during the secant method's iterations.
    // Here we make an initial guess that Ci = 0.4 * Ca.
    stomata_outputs BB_res;
    double an_conductance{};    // mol / m^2 / s
    double Ci_pa{0.4 * Ca_pa};  // Pa            (initial guess)
    double Gs{1e3};             // mol / m^2 / s (initial guess)

    // This lambda function equals zero
    // only if assim satisfies both collatz assim and Ball Berry model
    auto check_assim_rate = [=, &BB_res, &an_conductance, &Ci_pa, &Gs](double const assim) {
        // The net CO2 assimilation is the smaller of the biochemistry-limited
        // and conductance-limited rates. This will prevent the calculated Ci
        // value from ever being < 0. This is an important restriction to
        // prevent numerical errors during the convergence loop, but does not
        // seem to ever limit the net assimilation rate if the loop converges.
        an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

        double const assim_adj = std::min(assim, an_conductance);  // micromol / m^2 / s

        // If assim is correct, then Ball Berry gives the correct
        // CO2 at leaf surface (Cs) and correct stomatal conductance
        BB_res = ball_berry_gs(
            assim_adj * 1e-6,
            Ca * 1e-6,
            relative_humidity,
            bb0_adj,
            bb1_adj,
            gbw,
            leaf_temperature,
            ambient_temperature);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Using the value of stomatal conductance,
        // Calculate Ci using the total conductance across the boundary
        // layer and stomata
        Ci_pa =
            Ca_pa - atmospheric_pressure * (assim_adj * 1e-6) *
                        (dr_boundary / gbw + dr_stomata / Gs);  // Pa

        double check = collatz_assim(Ci_pa) - assim;
        return check;  // equals zero if correct
    };

    // Find starting guesses for the net CO2 assimilation rate. One is the
    // predicted rate at Ci = 0.4 * Ca, and the other is the predicted rate at
    // Ci = Ca.
    double const assim_guess_0 = collatz_assim(0.4 * Ca_pa);
    double const assim_guess_1 = collatz_assim(Ca_pa);

    // Run the secant method
    double Assim_check{};  // Will be modified by find_root_secant_method
    size_t iterations;     // Will be modified by find_root_secant_method

    double const Assim = find_root_secant_method(
        check_assim_rate,
        assim_guess_0,
        assim_guess_1,
        1000,
        1e-12,
        1e-12,
        Assim_check,
        iterations);

    // Convert Ci units
    double const Ci = Ci_pa / atmospheric_pressure * 1e6;  // micromol / mol

    return photosynthesis_outputs{
        /* .Assim = */ Assim,                       // micromol / m^2 /s
        /* .Assim_check = */ Assim_check,           // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Ci = */ Ci,                             // micromol / mol
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .GrossAssim = */ Assim + RT,             // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .Rp = */ 0,                              // micromol / m^2 / s
        /* .iterations = */ iterations              // not a physical quantity
    };
}
