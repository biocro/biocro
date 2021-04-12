/*
 *  BioCro/src/eC4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
 *
 */

#include <cmath>
#include "eC4photo.h"

double eC4photoC(double QP, double TEMP, double RH, double CA,
		double OA, double VCMAX, double VPMAX, double VPR,
		double JMAX)
{
    const double  gs = 3 * 1e-3 ;
    /* mol m-2 s-1 physical conductance to CO2 leakage*/
    /* const double gammaStar = 0.000193  */
    const double gammaStar =  0.0002239473      ;
    /* half the reciprocal of Rubisco specificity */
    /*    const double go = 0.047 * gs     at 25 C */
    const double alpha = 0.01 ; /* alpha in the notes*/
    const double Kp = 80 ; /*  mu bar */
    const double theta = 0.7;
    const double R = 0.008314472; // kJ K^-1 mol^-1

    /* ADDING THE TEMPERATURE RESPONSE FUNCTION */
    const double Ep  = 47.1 ; /* Activation energy of PEPc kj/mol */
    const double Erb = 72 ; /* Activation energy of Rubisco kj/mol */
    /*    const double Q10bf = 1.7 */
    const double EKc = 79.43;
    const double EKo = 36.38;
    const double Q10cb = 1.7;

    const double Ko2 = 532.9 ; /* mbar at 25 C */
    const double Kc2 = 1020 ; /*  mu bar at 25 C */

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

    double Q10p = exp(Ep *(1/(R*298.15)-1/(R*(AirTemp+273.15))));
    double Q10rb = exp(Erb *(1/(R*298.15)-1/(R*(AirTemp+273.15))));
    double Q10Kc = exp(EKc *(1/(R*298.15)-1/(R*(AirTemp+273.15))));
    double Q10Ko = exp(EKo *(1/(R*298.15)-1/(R*(AirTemp+273.15))));

    Vcmax = Vcmax1 * Q10rb;
    Kc = Kc2 * Q10Kc;
    Ko = Ko2 * Q10Ko;
    Vpmax = Vpmax1 * Q10p;        
    double Jmax = Jmax1 * pow(Q10cb,(AirTemp-25)/10);

    double Cm = 0.4 * Ca ; 
    double Om = Oa ;

    double Rd = 0.08;
    double Rm = 0.5 * Rd ;

    /* Light limited */
    double I2 = (Idir * 0.85)/2;
    double J = (Jmax + I2  - sqrt(pow(Jmax+I2,2) - 4 * theta * I2 * Jmax ))/2*theta;        
    double Aj0 = 0.4 * J - Rm + gs * Cm;        
    double Aj1 = (1-0.4)*J/3-Rd;

    if (Aj0 < Aj1){
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
    double b1 = -((Vp - Rm + gs * Cm) + (Vcmax - Rd) +
            gs * (Kc * (1 + Om1 / Ko1)) + ((alpha / 0.047) * (gammaStar * Vcmax + Rd * Kc / Ko1)));
    double c1 = (Vcmax - Rd) * (Vp - Rm + gs * Cm) - (Vcmax * gs * gammaStar * Om1 + Rd * gs * Kc * (1 + Om1 / Ko1));

    double c3 = pow(b1,2) - 4 * a1 * c1;
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

    return(A);
}

