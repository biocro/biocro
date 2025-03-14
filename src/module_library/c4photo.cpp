#include <cmath>                          // for pow, exp, std::abs
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_min
#include "c4photo.h"
#include "secant_method.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

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

    double Ca_pa = Ca * 1e-6 * atmospheric_pressure;  // Pa

    double kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // dimensionless

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = vmax * pow(2, (leaf_temperature - 25.0) / 10.0);                                              // micromole / m^2 / s
    double Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double VT = Vtn / Vtd;                                                                                     // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double Rtn = Rd * pow(2, (leaf_temperature - 25) / 10);  // micromole / m^2 / s
    double Rtd = 1 + exp(1.3 * (leaf_temperature - 55));     // dimensionless
    double RT = Rtn / Rtd;                                   // micromole / m^2 / s

    // Collatz 1992. Appendix B. Quadratic coefficients from Equation 2B.
    double b0 = VT * alpha * Qp;
    double b1 = -(VT + alpha * Qp);
    double b2 = theta;

    // Calculate the smaller of the two quadratic roots, as mentioned following
    // Equation 3B in Collatz 1992.
    double M = quadratic_root_min(b2, b1, b0);  // micromol / m^2 / s

    // Adjust Ball-Berry parameters in response to water stress
    double const bb0_adj = StomaWS * bb0 + Gs_min * (1.0 - StomaWS);
    double const bb1_adj = StomaWS * bb1;

    // Function to compute the biochemical assimilation rate.
    auto collatz_assim = [&](double InterCellularCO2) {
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
    double InterCellularCO2{0.4 * Ca_pa};  // Pa
    double Assim{};                        // micromol / m^2 / s
    double Gs{};                           // mol / m^2 / s

    // This lambda function equals zero
    // only if assim satisfies both collatz assim and Ball Berry model
    auto check_assim_rate = [&](double assim) {
        // If assim is correct, then Ball Berry gives the correct
        // CO2 at leaf surface (Cs) and correct stomatal conductance
        BB_res = ball_berry_gs(
            assim * 1e-6,
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
        InterCellularCO2 =
            Ca_pa - atmospheric_pressure * (Assim * 1e-6) *
                        (dr_boundary / gbw + dr_stomata / Gs);  // Pa

        double check = collatz_assim(InterCellularCO2) - assim;
        return check;  // equals zero if correct
    };

    secant_parameters secpar;
    // Initial guesses for the secant method
    double A0 = collatz_assim(Ca_pa);
    double A1 = collatz_assim(InterCellularCO2);
    Assim = find_root_secant_method(
        check_assim_rate, A0, A1, secpar);
    // unit change
    double Ci = InterCellularCO2 / atmospheric_pressure * 1e6;  // micromole / mol

    return photosynthesis_outputs{
        /* .Assim = */ Assim,               // micromol / m^2 /s
        /* .Assim_cehck = */ secpar.check,  // micromol / m^2 / s
        /* .Ci = */ Ci,                     // micromol / mol
        /* .GrossAssim = */ Assim + RT,     // micromol / m^2 / s
        /* .Gs = */ Gs,                     // mol / m^2 / s
        /* .Cs = */ BB_res.cs,              // micromol / m^2 / s
        /* .RHs = */ BB_res.hs,             // dimensionless from Pa / Pa
        /* .Rp = */ 0,                      // micromol / m^2 / s
        /* .iterations = */ secpar.counter  // not a physical quantity
    };
}
