#include <cmath>
#include "eC4photo.h"
#include "../framework/constants.h"  // for celsius_to_kelvin, ideal_gas_constant

double eC4photoC(
    double QP,
    double TEMP,
    double CA,
    double OA,
    double VCMAX,
    double VPMAX,
    double VPR,
    double JMAX)
{
    const double gs = 3 * 1e-3;
    /* mol m-2 s-1 physical conductance to CO2 leakage*/
    /* const double gammaStar = 0.000193  */
    const double gammaStar = 0.0002239473;
    /* half the reciprocal of Rubisco specificity */
    /*    const double go = 0.047 * gs     at 25 C */
    const double alpha = 0.01; /* alpha in the notes*/
    const double Kp = 80;      /*  mu bar */
    const double theta = 0.7;
    const double R = physical_constants::ideal_gas_constant * 1e-3;  // kJ K^-1 mol^-1

    /* ADDING THE TEMPERATURE RESPONSE FUNCTION */
    const double Ep = 47.1; /* Activation energy of PEPc kj/mol */
    const double Erb = 72;  /* Activation energy of Rubisco kj/mol */
    /*    const double Q10bf = 1.7 */
    const double EKc = 79.43;
    const double EKo = 36.38;
    const double Q10cb = 1.7;

    const double Ko2 = 532.9; /* mbar at 25 C */
    const double Kc2 = 1020;  /*  mu bar at 25 C */

    double Vcmax, Vpmax;
    double Kc, Ko;
    double Aj, Ac, A;

    double Ca = CA;
    double Oa = OA;
    double Vcmax1 = VCMAX;
    double Vpmax1 = VPMAX;
    double Vpr = VPR;
    double Jmax1 = JMAX;

    double Idir = QP;
    double AirTemp = TEMP;

    double Q10p = exp(Ep * (1 / (R * 298.15) - 1 / (R * (AirTemp + conversion_constants::celsius_to_kelvin))));
    double Q10rb = exp(Erb * (1 / (R * 298.15) - 1 / (R * (AirTemp + conversion_constants::celsius_to_kelvin))));
    double Q10Kc = exp(EKc * (1 / (R * 298.15) - 1 / (R * (AirTemp + conversion_constants::celsius_to_kelvin))));
    double Q10Ko = exp(EKo * (1 / (R * 298.15) - 1 / (R * (AirTemp + conversion_constants::celsius_to_kelvin))));

    Vcmax = Vcmax1 * Q10rb;
    Kc = Kc2 * Q10Kc;
    Ko = Ko2 * Q10Ko;
    Vpmax = Vpmax1 * Q10p;
    double Jmax_at_25 = Jmax1 * pow(Q10cb, (AirTemp - 25) / 10);

    double Cm = 0.4 * Ca;
    double Om = Oa;

    double RL = 0.08;
    double Rm = 0.5 * RL;

    /* Light limited */
    double I2 = (Idir * 0.85) / 2;
    double J = (Jmax_at_25 + I2 - sqrt(pow(Jmax_at_25 + I2, 2) - 4 * theta * I2 * Jmax_at_25)) / 2 * theta;
    double Aj0 = 0.4 * J - Rm + gs * Cm;
    double Aj1 = (1 - 0.4) * J / 3 - RL;

    if (Aj0 < Aj1) {
        Aj = Aj0;
    } else {
        Aj = Aj1;
    }

    /* Other part */
    double Vp = (Cm * Vpmax) / (Cm + Kp);
    if (Vpr < Vp) {
        Vp = Vpr;
    }

    /* Alternative formula */
    double Ko1 = Ko * 1e3;
    double Om1 = Om * 1e3;

    double a1 = 1 - alpha / 0.047 * Kc / Ko1;
    double b1 = -((Vp - Rm + gs * Cm) + (Vcmax - RL) +
                  gs * (Kc * (1 + Om1 / Ko1)) + ((alpha / 0.047) * (gammaStar * Vcmax + RL * Kc / Ko1)));
    double c1 = (Vcmax - RL) * (Vp - Rm + gs * Cm) - (Vcmax * gs * gammaStar * Om1 + RL * gs * Kc * (1 + Om1 / Ko1));

    double c3 = pow(b1, 2) - 4 * a1 * c1;
    if (c3 < 0) {
        c3 = 0;
    }
    double Ac0 = (-b1 - sqrt(c3)) / 2 * a1;

    double AcLCO2 = (Cm * Vpmax / (Cm + Kp)) - Rm + gs * Cm;

    if (Ac0 < AcLCO2) {
        Ac = Ac0;
    } else {
        Ac = AcLCO2;
    }

    if (Aj < Ac) {
        A = Aj;
    } else {
        A = Ac;
    }

    return A;
}
