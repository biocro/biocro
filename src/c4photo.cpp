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

#include <math.h>
#include "ball_berry.h"
#include "c4photo.h"

struct c4_str c4photoC(double Qp, double Tl, double RH, double vmax, double alpha, 
		       double kparm, double theta, double beta,
		       double Rd, double bb0, double bb1, double StomaWS, double Ca, int water_stress_approach, double upperT, double lowerT)
{

	const double AP = 101325; /*Atmospheric pressure According to wikipedia (Pa)*/
    const double P = AP / 1e3; /* kPa */
	const double Q10 = 2;  /* Q10 increase in a reaction by 10 C temp */

	double M;
	double Assim;

	double Gs;
	double OldAssim = 0.0, Tol = 0.1;

	double Csurface = (Ca * 1e-6) * AP;
	double InterCellularCO2 = Csurface * 0.4; /* Assign an initial guess. */
	double KQ10 =  pow(Q10, (Tl - 25.0) / 10.0);
	double kT = kparm * KQ10;

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = vmax * pow(2, (Tl - 25.0) / 10.0);  //micromole / m^2 / s
    double Vtd = (1 + exp(0.3 * (lowerT - Tl))) * (1 + exp(0.3 * (Tl - upperT)));  // dimensionless
    double VT  = Vtn / Vtd;  // micromole / m^2 / s

	// Collatz 1992. Appendix B. Equation set 5B.
	double Rtn = Rd * pow(2, (Tl - 25) / 10);  //micromole / m^2 / s
	double Rtd =  1 + exp(1.3 * (Tl - 55));  // dimensionless
	double RT = Rtn / Rtd;   //micromole / m^2 / s

	// Collatz 1992. Appendix B. Equation 2B.
	double b0 = VT * alpha * Qp;
	double b1 = VT + alpha * Qp;
	double b2 = theta;

	/* Calculate the 2 roots */
	double M1 = (b1 + sqrt(b1 * b1 - (4 * b0 * b2))) / (2 * b2);
	double M2 = (b1 - sqrt(b1 * b1 - (4 * b0 * b2))) / (2 * b2);

	/* This piece of code selects the smallest root */
	if(M1 < M2)
		M = M1;
	else
		M = M2;

	int iterCounter = 0;
	while (iterCounter < 50) {

		double kT_IC_P = kT * (InterCellularCO2 / P * 1000);
		double a = M * kT_IC_P;
		double b = M + kT_IC_P;
		double c = beta;

		double a2 = (b - sqrt(b * b - (4 * a * c))) / (2 * c);

		Assim = a2 - RT;  // micromole / m^2 / s.

		if (water_stress_approach == 0) Assim *= StomaWS; 

		/* milimole per meter square per second*/
		double csurfaceppm = Csurface * 10;

		/* Need to create the Ball-Berry function */
		Gs =  ballBerry(Assim, csurfaceppm, Tl, RH, bb0, bb1);
		if (water_stress_approach == 1) Gs *= StomaWS; 

		InterCellularCO2 = Csurface - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);

		if (InterCellularCO2 < 0)
			InterCellularCO2 = 1e-5;

		double diff = OldAssim - Assim;
		if (diff < 0) diff = -diff;

		if (diff < Tol) {
			break;
		} else {
			OldAssim = Assim;
		}

		++iterCounter;
	}

	double miC = (InterCellularCO2 / AP) * 1e6;

	if(Gs > 600)
	  Gs = 600;

	struct c4_str tmp;
    tmp.Assim = Assim;
    tmp.Gs = Gs;
    tmp.Ci = miC;
    tmp.GrossAssim=Assim + RT;
    return(tmp);
}

