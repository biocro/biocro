#include <cmath>
#include "ball_berry.hpp"
#include "c4photo.h"
#include "../framework/constants.h"  // for dr_stomata

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

struct c4_str c4photoC(
    double Qp,                    // micromol / m^2 / s
    double leaf_temperature,      // degrees C
    double relative_humidity,     // dimensionless from Pa / Pa
    double vmax,                  // micromol / m^2 / s
    double alpha,                 // mol / mol
    double kparm,                 // mol / m^2 / s
    double theta,                 // dimensionless
    double beta,                  // dimensionless
    double Rd,                    // micromol / m^2 / s
    double bb0,                   // mol / m^2 / s
    double bb1,                   // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double Gs_min,                // mmol / m^2 / s
    double StomaWS,               // dimensionless
    double Ca,                    // micromol / mol
    double atmospheric_pressure,  // Pa
    int water_stress_approach,    // unitless switch
    double upperT,                // degrees C
    double lowerT,                // degrees C
    double gbw                    // mol / m^2 / s
)
{
    constexpr double k_Q10 = 2;  // dimensionless. Increase in a reaction rate per temperature increase of 10 degrees Celsius.

    double Ca_pa = Ca * 1e6 * atmospheric_pressure;  // Pa

    double kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // dimensionless

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = vmax * pow(2, (leaf_temperature - 25.0) / 10.0);                                              // micromole / m^2 / s
    double Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double VT = Vtn / Vtd;                                                                                     // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double Rtn = Rd * pow(2, (leaf_temperature - 25) / 10);  // micromole / m^2 / s
    double Rtd = 1 + exp(1.3 * (leaf_temperature - 55));     // dimensionless
    double RT = Rtn / Rtd;                                   // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation 2B.
    double b0 = VT * alpha * Qp;
    double b1 = VT + alpha * Qp;
    double b2 = theta;

    /* Calculate the 2 roots */
    double M1 = (b1 + sqrt(b1 * b1 - 4 * b0 * b2)) / 2 / b2;
    double M2 = (b1 - sqrt(b1 * b1 - 4 * b0 * b2)) / 2 / b2;

    double M = M1 < M2 ? M1 : M2;  // Use the smallest root.

    // Initialize loop variables. Here we make an initial guess that
    // Ci = 0.4 * Ca.
    double InterCellularCO2{0.4 * Ca_pa};  // Pa
    double Assim{};                        // micromol / m^2 / s
    double Gs{};                           // mol / m^2 / s

    // Start the loop
    double OldAssim = 0.0, Tol = 0.1, diff;
    unsigned int iterCounter = 0;
    unsigned int constexpr max_iterations = 50;
    do {
        // Collatz 1992. Appendix B. Equation 3B.
        double kT_IC_P = kT * InterCellularCO2 / atmospheric_pressure * 1e6;  // micromole / m^2 / s
        double a = M * kT_IC_P;
        double b = M + kT_IC_P;
        double c = beta;

        double gross_assim = (b - sqrt(b * b - 4 * a * c)) / 2 / c;  // micromole / m^2 / s

        Assim = gross_assim - RT;  // micromole / m^2 / s.

        if (water_stress_approach == 0) Assim *= StomaWS;

        Gs = ball_berry(
            Assim * 1e-6,
            Ca * 1e-6,
            relative_humidity,
            bb0,
            bb1,
            gbw);  // mmol / m^2 / s

        if (water_stress_approach == 1) {
            Gs = Gs_min + StomaWS * (Gs - Gs_min);
        }

        if (iterCounter > max_iterations - 10)
            Gs = bb0 * 1e3;  // mmol / m^2 / s. If it has gone through this many iterations, the convergence is not stable. This convergence is inapproriate for high water stress conditions, so use the minimum gs to try to get a stable system.

        // Calculate Ci using the total conductance across the boundary
        // layer and stomata
        InterCellularCO2 =
            Ca_pa - atmospheric_pressure * (Assim * 1e-6) *
                        (dr_boundary / gbw + dr_stomata / (Gs * 1e-3));  // Pa

        if (InterCellularCO2 < 0) {
            InterCellularCO2 = 1e-5;
        }

        diff = fabs(OldAssim - Assim);  // micromole / m^2 / s

        OldAssim = Assim;  // micromole / m^2 / s

    } while (diff >= Tol && ++iterCounter < max_iterations);
    //if (iterCounter > 49)
    //Rprintf("Counter %i; Ci %f; Assim %f; Gs %f; leaf_temperature %f\n", iterCounter, InterCellularCO2 / atmospheric_pressure * 1e6, Assim, Gs, leaf_temperature);

    double Ci = InterCellularCO2 / atmospheric_pressure * 1e6;  // micromole / mol

    c4_str result{
        .Assim = Assim,            // micromole / m^2 /s
        .Gs = Gs,                  // mmol / m^2 / s
        .Ci = Ci,                  // micromole / mol
        .GrossAssim = Assim + RT,  // micromole / m^2 / s
        .iterations = iterCounter  // not a physical quantity
    };

    return result;
}
