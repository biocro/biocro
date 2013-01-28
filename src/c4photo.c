/*
 *  /src/c4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
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

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c4photo.h"

SEXP c4photo(SEXP Qp, SEXP Tl, SEXP RH, SEXP VMAX, SEXP ALPHA,
	     SEXP KPAR, SEXP THETA, SEXP BETA, SEXP RD, SEXP CA, SEXP B0, SEXP B1, SEXP STOMWS, SEXP WS)
{
	struct c4_str tmp;

	double Bet0,Bet1;

	double vmax, alpha, K, Rd, StomWS;
	double theta, beta;

	vmax = REAL(VMAX)[0];
	alpha = REAL(ALPHA)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];
	K = REAL(KPAR)[0];
	Bet0 = REAL(B0)[0];
	Bet1 = REAL(B1)[0];
	Rd = REAL(RD)[0];
	StomWS = REAL(STOMWS)[0];

	int nq , nt, nr, i;

	SEXP lists, names;
	SEXP GsV;
	SEXP ASSV;
	SEXP CiV;

	nq = length(Qp);nt = length(Tl);nr = length(RH);

	PROTECT(lists = allocVector(VECSXP,3));
	PROTECT(names = allocVector(STRSXP,3));

	PROTECT(GsV = allocVector(REALSXP,nq));
	PROTECT(ASSV = allocVector(REALSXP,nq));
	PROTECT(CiV = allocVector(REALSXP,nq));

	double *pt_Qp = REAL(Qp);
	double *pt_Tl = REAL(Tl);
	double *pt_RH = REAL(RH);
	double *pt_CA = REAL(CA);
	int ws = INTEGER(WS)[0];

	double *pt_GSV = REAL(GsV);
	double *pt_ASSV = REAL(ASSV);
	double *pt_CiV = REAL(CiV);

	/* Start of the loop */
	for(i = 0; i < nq ; i++)
	{

		tmp = c4photoC(*(pt_Qp+i), *(pt_Tl+i), *(pt_RH+i),
			       vmax, alpha, K,theta, beta, Rd, 
			       Bet0, Bet1, StomWS, 
			       *(pt_CA+i), ws);

		*(pt_GSV + i) = tmp.Gs;
		*(pt_ASSV + i) = tmp.Assim;    
		*(pt_CiV + i) = tmp.Ci;    
/* Here it is using the REAL function every time */
/* I should change this to a pointer too at some point */ 
	}

	SET_VECTOR_ELT(lists,0,GsV);
	SET_VECTOR_ELT(lists,1,ASSV);
	SET_VECTOR_ELT(lists,2,CiV);
	SET_STRING_ELT(names,0,mkChar("Gs"));
	SET_STRING_ELT(names,1,mkChar("Assim"));
	SET_STRING_ELT(names,2,mkChar("Ci"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(5);   
	return(lists);
}


/* ball Berry stomatal conductance function */
double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1){

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
		       double Rd, double bb0, double bb1, double StomaWS, double Ca, int ws)
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
	double a2 , Assim ;
	double csurfaceppm ;
	double Gs , miC = 0.0 ;
	double diff, OldAssim = 0.0, Tol = 0.1;
	double kT_IC_P;

/* When the method does not converge I do not use the iterative solution*/
	double Assim0 = 0.0;
	double Gs0 = 0.0;
	double IntCO2 = 0.0;
	/* partial pressure of CO2 at the leaf surface */
  
	/* if(StomaWS < 0.5) ws = 0; */

	Csurface = (Ca * 1e-6) * AP ;
  
	InterCellularCO2 = Csurface * 0.4; /* Initial guestimate */

	KQ10 =  pow(Q10,((Tl - 25.0) / 10.0));

	kT = kparm * KQ10;

	/* First chunk of code see Collatz (1992) */
	Vtn = vmax * pow(2,((Tl-25.0)/10.0));
	Vtd = ( 1 + exp(0.3 * (14-Tl)) ) * (1 + exp( 0.3*(Tl-40) ));
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
			Assim0 = Assim;
			Gs0 = Gs;
			IntCO2 = InterCellularCO2;
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

	tmp.Assim = Assim;
	tmp.Gs = Gs;
	tmp.Ci = miC;
	return(tmp);
}

/* Start of the McMCc4photo.c function */


/* Here I'm including a version of the McMCc4photo function where the likelihood
   is approximated by using exp(-RSS) */
SEXP McMCc4photo(SEXP ASSIM, SEXP QP, SEXP TEMP,
		 SEXP RH, SEXP NITER, SEXP iVCMAX,
		 SEXP iALPHA, SEXP iKPARM, 
		 SEXP iTHETA, SEXP iBETA,
		 SEXP iRD,
                 SEXP CATM, SEXP B0, SEXP B1, SEXP STOMWS,
                 SEXP SCALE, SEXP SD1, SEXP SD2, SEXP WS, SEXP PRIOR){
	/* First manipulate R objects */
	extern int nObs;
	int niter;
	nObs = length(ASSIM);
	niter = INTEGER(NITER)[0];

	/* Second define the needed variables */
	double oldRSS = 1e6;
	int n1 = 0;

	int i;

	double scale = REAL(SCALE)[0];
	double sd1, sd2;

	double Rd = REAL(iRD)[0];
	double ikparm = REAL(iKPARM)[0];
	double ibeta = REAL(iBETA)[0];
	double itheta = REAL(iTHETA)[0];
	double Catm = REAL(CATM)[0];
	double b0 = REAL(B0)[0];
	double b1 = REAL(B1)[0];
	double StomWS = REAL(STOMWS)[0];
	int ws = INTEGER(WS)[0];

	double index;
	double rnum , rden; 
	double lratio; 
	double lmr;
	double U;

	double pmuVmax = REAL(PRIOR)[0];
	double psdVmax = REAL(PRIOR)[1];
	double pmuAlpha = REAL(PRIOR)[2];
	double psdAlpha = REAL(PRIOR)[3];

	double RSS;
	double rnewVcmax, rnewAlpha;
	double oldAlpha, oldVcmax;

	SEXP lists;
	SEXP names;
	SEXP mat1;
	SEXP accept;


	PROTECT(lists = allocVector(VECSXP,2));
	PROTECT(names = allocVector(STRSXP,2));
	PROTECT(mat1 = allocMatrix(REALSXP,3,niter));
	PROTECT(accept = allocVector(REALSXP,1));

	GetRNGstate();

	oldVcmax = REAL(iVCMAX)[0];
	oldAlpha = REAL(iALPHA)[0];

	sd1 = REAL(SD1)[0] * scale;
	sd2 = REAL(SD2)[0] * scale;

	for(i = 0; i < niter; i++){

		/* Replacing the rnormC4 function */
		index = runif(0,1); 
		if(index < 0.5){ 
			rnewVcmax = oldVcmax + rnorm(0,sd1);
			rnewAlpha = oldAlpha;
		}else{ 
			rnewAlpha = oldAlpha +  rnorm(0, sd2);
			rnewVcmax = oldVcmax;
		} 
		/* This is the end of the random generation */

		rnum = dnorm(rnewVcmax,pmuVmax,psdVmax,0)*dnorm(rnewAlpha,pmuAlpha,psdAlpha,0); 
		rden = dnorm(oldVcmax,pmuVmax,psdVmax,0)*dnorm(oldAlpha,pmuAlpha,psdAlpha,0); 
	  
		lratio = log(rnum) - log(rden); 

		RSS = RSS_C4photo(REAL(ASSIM),REAL(QP),REAL(TEMP),REAL(RH),rnewVcmax,rnewAlpha, ikparm, itheta, ibeta, Rd, Catm, b0, b1, StomWS, ws);

/*       mr = (exp(-RSS) / exp(-oldRSS)) * ratio; */
/* In the previous expression we can take  the log and have instead the 
   log metropolis ratio and have a simpler calculation. We then need to compare this to
   the log of a uniform. */
		lmr = -RSS + oldRSS + lratio;

		U = runif(0,1);
		if(lmr > log(U)){
			oldVcmax = rnewVcmax;
			oldAlpha = rnewAlpha;
			oldRSS = RSS;
			n1++;
		}

		REAL(mat1)[i*3] = oldVcmax ;
		REAL(mat1)[i*3 + 1] = oldAlpha ;
		REAL(mat1)[i*3 + 2] = oldRSS;
	}
     
	PutRNGstate();    

	REAL(accept)[0] = n1;

	SET_VECTOR_ELT(lists,0,accept);
	SET_VECTOR_ELT(lists,1,mat1);

	SET_STRING_ELT(names,0,mkChar("accept"));
	SET_STRING_ELT(names,1,mkChar("resuMC"));

	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(4);

	return(lists);
}

/* Calculates RSS according to the Collatz model */
/* and given values for the two most important */
/* parameters Vcmax and alpha */
double RSS_C4photo(double oAssim[nObs], double oQp[nObs], double oTemp[nObs], 
		   double oRH[nObs], double vmax, double alpha, double kparm, 
		   double theta, double beta,
                   double Rd, double Catm, double b0, double b1, double StomWS, int ws){
	extern int nObs;
	struct c4_str tmp;
	int i;
	double RSS = 0.0, diff = 0.0;

	for(i = 0;i < nObs; i++){

		tmp = c4photoC(oQp[i],oTemp[i],oRH[i],vmax,alpha,kparm,theta, beta, Rd,b0,b1,StomWS,Catm,ws);
		diff = oAssim[i] - tmp.Assim;
		RSS += diff * diff;

	}
	return(RSS);
}
