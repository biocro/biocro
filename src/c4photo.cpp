/*
 *  /src/c4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 *  Part of this code (see function c4photoC) is based on the C4 photo of 
 *  WIMOVAC. WIMOVAC is copyright of Stephen P Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 *  The ballBerry code is based on code provided by Joe Berry in an e-mail.
 *  The original function was written in Fortran. I translated it to C.
 * 
 */

#include <math.h>
#include "c4photo.h"

/* Ball Berry stomatal conductance function */
double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1)
{

	const double gbw = 1.2; /* According to Collatz et al. (1992) pg. 526*/
	const double ptotPa = 101325; /* Atmospheric pressure */

	double pwaPa, leafTk, Camf, assimn;
	double wa, pwi, wi, gswmol, Cs, acs;
	double aaa, bbb, ccc, ddd;
	double gsmol;
	double hs;

	leafTk = Temp + 273.15;
	pwi = fnpsvp(leafTk);
	pwaPa = RelH * pwi;
	Camf = Cappm * 1e-6;
	assimn = Amu * 1e-6;
  
	/* Calculate mole fraction (mixing ratio) of water vapor in */
	/* atmosphere from partial pressure of water in the atmosphere and*/
	/* the total air pressure */
	wa  = pwaPa / ptotPa;
	/* Get saturation vapor pressure for water in the leaf based on */
	/* current idea of the leaf temperature in Kelvin*/
	/* Already calculated above */
	/* Calculate mole fraction of water vapor in leaf interior */
	wi  = pwi / ptotPa;

	if(assimn < 0.0){
		/* Set stomatal conductance to the minimum value, beta0*/
		gswmol = beta0 ;
		/* Calculate leaf surface relative humidity, hs, (as fraction)*/
		/* for when C assimilation rate is <= 0*/
		/* hs = (beta0 + (wa/wi)*gbw)/(beta0 + gbw); ! unused here??*/
	}
	else{
		/* leaf surface CO2, mole fraction */
		Cs  = Camf - (1.4/gbw)*assimn;
		if(Cs < 0.0)
			Cs = 1;
		/* Constrain the ratio assimn/cs to be > 1.e-6*/
		acs = assimn/Cs;

		if(acs < 1e-6) 	acs = 1e-6;		

		/* Calculate leaf surface relative humidity, hs, from quadratic */
		/* equation: aaa*hs^2 + bbb*hs + ccc = 0 */
		/*  aaa= beta1 * assimn / cs */
		aaa = beta1 * acs;
		/*      bbb= beta0 + gbw - (beta1 * assimn / cs)*/
		bbb = beta0 + gbw - (beta1 * acs);
		ccc = -(wa / wi) * gbw - beta0;

		/* Solve the quadratic equation for leaf surface relative humidity */
		/* (as fraction) */
		ddd = bbb * bbb - 4*aaa*ccc;

		hs  = (-bbb + sqrt(ddd)) / (2* aaa);

		/* Ball-Berry equation (Collatz'91, eqn 1) */
		gswmol = beta1 * hs * acs + beta0;
	}
	gsmol = gswmol * 1000; /* converting to mmol */

	if(gsmol <= 0) gsmol = 1e-5;

	return(gsmol);
}


double fnpsvp(double Tkelvin){
	/* water boiling point = 373.16 oK*/
/* This is the Arden Buck Equation 
http://en.wikipedia.org/wiki/Arden_Buck_equation
 */
	double u, v;
	double tmp, esat;

	tmp = Tkelvin - 273.15;
	u = (18.678 - tmp/234.5)*tmp;
	v = 257.14 + tmp;
	esat = 6.1121 * exp(u/v);
	esat /= 10;

	return(esat);
}


/* c4photo function */ 
struct c4_str c4photoC(double Qp, double Tl, double RH, double vmax, double alpha, 
		       double kparm, double theta, double beta,
		       double Rd, double bb0, double bb1, double StomaWS, double Ca, int ws,double upperT,double lowerT)
{

	struct c4_str tmp;
	/* Constants */
	const double AP = 101325; /*Atmospheric pressure According to wikipedia (Pa)*/
	const double P = AP / 1e3; /* kPa */
	/*const double PS = 38;   Atmospheric pressure of CO2 */
	const double Q10 = 2;  /* Q10 increase in a reaction by 10 C temp */
	/* Defining biochemical variables */

	double Csurface ;
	double InterCellularCO2 ;
	double KQ10 , kT ;
	double Vtn , Vtd , VT ;
	double Rtn , Rtd , RT ;
	double b0 , b1 , b2 ;
	double M1 , M2 , M ;
	int iterCounter ;
	double Quada , Quadb , Quadc ;
	double a2 , Assim ,GrossAssim;
	double csurfaceppm ;
	double Gs , miC = 0.0 ;
	double diff, OldAssim = 0.0, Tol = 0.1;
	double kT_IC_P;

/* When the method does not converge I do not use the iterative solution*/
	// double Assim0 = 0.0; unused
	// double Gs0 = 0.0; unused
	// double IntCO2 = 0.0; unused
	/* partial pressure of CO2 at the leaf surface */
  
	/* if(StomaWS < 0.5) ws = 0; */

	Csurface = (Ca * 1e-6) * AP ;
  
	InterCellularCO2 = Csurface * 0.4; /* Initial guestimate */

	KQ10 =  pow(Q10,((Tl - 25.0) / 10.0));

	kT = kparm * KQ10;

	/* First chunk of code see Collatz (1992) */
//	Vtn = vmax * pow(2,((Tl-25.0)/10.0));
//	Vtd = ( 1 + exp(0.3 * (3.0-Tl)) ) * (1 + exp( 0.3*(Tl-37.5) ));
//	VT  = Vtn / Vtd;

//       This is the code implementing temperature limitations
         Vtn = vmax * pow(2,((Tl-25.0)/10.0));
         Vtd = ( 1 + exp(0.3 * (lowerT-Tl)) ) * (1 + exp( 0.3*(Tl-upperT) ));
         VT  = Vtn / Vtd;


	/* Second chunk of code see Collatz (1992) */
	Rtn = Rd * pow(2 , (Tl-25)/10 ) ;
	Rtd =  1 + exp( 1.3 * (Tl-55) ) ;
	RT = Rtn / Rtd ; 

	/* Third chunk of code again see Collatz (1992) */
	b0 = VT * alpha  * Qp ;
	b1 = VT + alpha  * Qp ;
	b2 = theta ;

	/* Calculate the 2 roots */
	M1 = (b1 + sqrt(b1*b1 - (4 * b0 * b2)))/(2*b2) ;
	M2 = (b1 - sqrt(b1*b1 - (4 * b0 * b2)))/(2*b2) ;

	/* This piece of code selects the smalles root */
	if(M1 < M2)
		M = M1;
	else
		M = M2;

	/* Here the iterations will start */
	iterCounter = 0;

	while(iterCounter < 50)
	{

		kT_IC_P = kT * (InterCellularCO2 / P*1000);
		Quada = M * kT_IC_P;
		Quadb = M + kT_IC_P;
		Quadc = beta ;

		a2 = (Quadb - sqrt(Quadb*Quadb - (4 * Quada * Quadc))) / (2 * Quadc);

		Assim = a2 - RT;

		if(ws == 0) Assim *= StomaWS; 

		/* milimole per meter square per second*/
		csurfaceppm = Csurface * 10 ;

		/* Need to create the Ball-Berry function */
		Gs =  ballBerry(Assim,csurfaceppm, Tl, RH, bb0, bb1) ;
		if(ws == 1) Gs *= StomaWS; 

		InterCellularCO2 = Csurface - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);

		if(InterCellularCO2 < 0)
			InterCellularCO2 = 1e-5;

		if(iterCounter == 0){
			// Assim0 = Assim; set but not used
			// Gs0 = Gs; set but not used
			// IntCO2 = InterCellularCO2; set but not used
		}

		diff = OldAssim - Assim;
		if(diff < 0) diff = -diff;
		if(diff < Tol){
			break;
		}else{
			OldAssim = Assim;
		}

		iterCounter++;

	}

/* This would ignore the optimization due to the iterative procedure
 * when it does not converge. It is turned off now*/


	/* if(diff > Tol){ */
	/* 	Assim = Assim0; */
	/* 	Gs = Gs0; */
	/* 	InterCellularCO2 = IntCO2; */
	/* } */

	/* if(diff > Tol){ */
	/* 	Rprintf("iter %.i diff %.3f \n",iterCounter,diff); */
	/* 	Rprintf("%.2f %.2f %.2f %.2f \n", Qp, Tl, RH, StomaWS); */
	/* 	Rprintf("StomWS %.2f \n",StomaWS); */
	/* 	Rprintf("InterCellular CO2 %.2f \n",InterCellularCO2); */
	/* 	Rprintf("Assim %.2f \n",Assim); */
	/* 	Rprintf("miC %.2f \n",(InterCellularCO2/AP)*1e6); */
	/* 	Rprintf("vmax %.1f alpha %.3f \n", vmax, alpha); */
	/* 	Rprintf("kparm %.3f theta %.3f \n", kparm, theta); */
	/* 	Rprintf("beta %.3f Rd %.3f \n", beta, Rd); */
	/* 	Rprintf("bb0 %.3f bb1 %.3f \n", bb0, bb1); */
	/* 	Rprintf("Ca %.1f ws %i \n", Ca, ws); */
 	/* 	error("Did not converge \n"); */
	/* } */

	miC = (InterCellularCO2 / AP) * 1e6 ;

	if(Gs > 600)
	  Gs = 600;
  GrossAssim=Assim+RT;
	tmp.Assim = Assim;
	tmp.Gs = Gs;
	tmp.Ci = miC;
  tmp.GrossAssim=GrossAssim;
	return(tmp);
}

/* Calculates RSS according to the Collatz model */
/* and given values for the two most important */
/* parameters Vcmax and alpha */
double RSS_C4photo(double oAssim[], double oQp[], double oTemp[], 
		   double oRH[], double vmax, double alpha, double kparm, /*\ref{eqn:Vmax}*/
		   double theta, double beta,
                   double Rd, double Catm, double b0, double b1, double StomWS, int ws,double upperT,double lowerT, int nObs){ /*\ref{eqn:Rd}*/
	struct c4_str tmp;
	int i;
	double RSS = 0.0, diff = 0.0;

	for(i = 0;i < nObs; i++){

		tmp = c4photoC(oQp[i],oTemp[i],oRH[i],vmax,alpha,kparm,theta, beta, Rd,b0,b1,StomWS,Catm,ws,upperT,lowerT);
		diff = oAssim[i] - tmp.Assim;
		RSS += diff * diff;

	}
	return(RSS);
}

