/*
 *  /src/c4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 *  Part of this code (see function c4photoC) is based on the C4 photo of
 *  WIMOVAC. WIMOVAC is copyright of Stephen P Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */
#include <cmath>
#include "ball_berry.hpp"
#include "c4photo.h"

struct c4_str c4photoC(double Qp,  // micromole / m^2 / s
                       double leaf_temperature,  // degrees C
                       double relative_humidity,  // dimensionless from Pa / Pa
                       double vmax,  // micromole / m^2 / s
                       double alpha,  // mol / mol
                       double kparm,  // mol / m%2 / s
                       double theta,
                       double beta,
                       double Rd,
                       double bb0,
                       double bb1,
                       double StomaWS,
                       double Ca,  // micromole / mol
                       int water_stress_approach,
                       double upperT,
                       double lowerT)
{

    constexpr double AP = 101325; // Pa
    constexpr double k_Q10 = 2;  // dimensionless. Increase in a reaction rate per temperature increase of 10 degrees Celsius.

    double Csurface = Ca * 1e-6 * AP;  // Pa
    double InterCellularCO2 = Csurface * 0.4;  // Pa. Use an initial guess.
    double kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // dimensionless

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = vmax * pow(2, (leaf_temperature - 25.0) / 10.0);  // micromole / m^2 / s
    double Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double VT  = Vtn / Vtd;  // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double Rtn = Rd * pow(2, (leaf_temperature - 25) / 10);  // micromole / m^2 / s
    double Rtd = 1 + exp(1.3 * (leaf_temperature - 55));  // dimensionless
    double RT  = Rtn / Rtd;  // micromole / m^2 / s

    // Collatz 1992. Appendix B. Equation 2B.
    double b0 = VT * alpha * Qp;
    double b1 = VT + alpha * Qp;
    double b2 = theta;

    /* Calculate the 2 roots */
    double M1 = (b1 + sqrt(b1 * b1 - 4 * b0 * b2)) / 2 / b2;
    double M2 = (b1 - sqrt(b1 * b1 - 4 * b0 * b2)) / 2 / b2;

    double M = M1 < M2 ? M1 : M2; // Use the smallest root.

    double Assim, Gs;
    {
        double OldAssim = 0.0, Tol = 0.1, diff;
        unsigned int iterCounter = 0;
        unsigned int constexpr max_iterations = 50;
        do {
            // Collatz 1992. Appendix B. Equation 3B.
            double kT_IC_P = kT * InterCellularCO2 / AP * 1e6;  // micromole / m^2 / s
            double a = M * kT_IC_P;
            double b = M + kT_IC_P;
            double c = beta;

            double gross_assim = (b - sqrt(b * b - 4 * a * c)) / 2 / c;  // micromole / m^2 / s

            Assim = gross_assim - RT;  // micromole / m^2 / s.

            if (water_stress_approach == 0) Assim *= StomaWS;

            Gs = ball_berry(Assim * 1e-6, Ca * 1e-6, relative_humidity, bb0, bb1);  // mmol / m^2 / s
            if (water_stress_approach == 1) Gs *= StomaWS;

            if (iterCounter > max_iterations - 10)
                Gs = bb0 * 1e3;  // mmol / m^2 / s. If it has gone through this many iterations, the convergence is not stable. This convergence is inapproriate for high water stress conditions, so use the minimum gs to try to get a stable system.

            //Rprintf("Counter %i; Ci %f; Assim %f; Gs %f; leaf_temperature %f\n", iterCounter, InterCellularCO2 / AP * 1e6, Assim, Gs, leaf_temperature);
            InterCellularCO2 = Csurface - Assim * 1e-6 * 1.6 * AP / (Gs * 0.001);  // Pa

            if (InterCellularCO2 < 0)
                InterCellularCO2 = 1e-5;

            diff = fabs(OldAssim - Assim);  // micromole / m^2 / s

            OldAssim = Assim;  // micromole / m^2 / s


        } while (diff >= Tol && ++iterCounter < max_iterations);
        //if (iterCounter > 49)
            //Rprintf("Counter %i; Ci %f; Assim %f; Gs %f; leaf_temperature %f\n", iterCounter, InterCellularCO2 / AP * 1e6, Assim, Gs, leaf_temperature);
    }

    double Ci = InterCellularCO2 / AP * 1e6;  // micromole / mol

    if (Gs > 600) Gs = 600;

    struct c4_str result {
            .Assim = Assim,           // micromole / m^2 /s
            .Gs = Gs,                 // mmol / m^2 / s
            .Ci = Ci,                 // micromole / mol
            .GrossAssim = Assim + RT  // micromole / m^2 / s
    };

    return result;
}

