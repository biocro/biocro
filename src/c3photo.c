/*
 *  /src/c3photo.c by Fernando Ezequiel Miguez  Copyright (C) 2009
 *
 */

#include <math.h>
#include "c3photo.h"

/* c3photo function */ 
struct c3_str c3photoC(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax, 
		       double Rd0, double bb0, double bb1, double Ca, double O2, double thet,double StomWS,int ws)
{
	struct c3_str tmp = {0,0,0,0};
	/* Constants */
	const double AP = 101325; /*Atmospheric pressure According to wikipedia (Pa)*/
	const double R = 0.008314472; /* Gas constant */
	const double Spectral_Imbalance = 0.25;
	const double Leaf_Reflectance = 0.2;
	const double Rate_TPu = 23;
	/* Defining biochemical variables */


	double Ci = 0.0, Oi;/*\ref{parm:ci}\ref{parm:Oi}*/
	double Kc, Ko, Gstar;/*\ref{parm:Kc}\ref{parm:K0}\ref{parm:gammaast}*/
	double Vc = 0.0;
	double Vcmax, Rd; /*\ref{parm:Vcmax}\ref{parm:Rd}*/
	double Ac1, Ac2, Ac;
	double Aj1, Aj2, Aj;
	double Ap;
	double Assim, J, I2,Assim0,Gs0;/*\ref{parm:J}*/
	double FEII;
	double theta;
	double Citr1, Citr2, Citr;

	int iterCounter = 0;
	double Gs ;
	double diff, OldAssim = 0.0,OldCi = 0.0, Tol = 0.01;

	if(Ca <= 0)
		Ca = 1e-4;

	/* From Bernacchi 2001. Improved temperature response functions. */
	/* note: values in Dubois and Bernacchi are incorrect */	
	Kc = exp(38.05-79.43/(R*(Tleaf+273.15))); /*\ref{eqn:Kc}*/
	Ko = exp(20.30-36.38/(R*(Tleaf+273.15))); /*\ref{eqn:K0}*/
	Gstar = exp(19.02-37.83/(R*(Tleaf+273.15))); /*\ref{eqn:gammaast}*/

	Vcmax = Vcmax0 * exp(26.35 - 65.33/(R * (Tleaf+273.15))); /*\ref{eqn:Vcmax}*/
	Rd = Rd0 * exp(18.72 - 46.39/(R * (Tleaf+273.15)));/*\ref{eqn:Rd}*/

        /* Effect of temperature on theta */
	theta = thet + 0.018 * Tleaf - 3.7e-4 * pow(Tleaf,2);

	/* Rprintf("Vcmax, %.1f, Rd %.1f, Gstar %.1f, theta %.1f \n",Vcmax,Rd,Gstar,theta); */

	/* Light limited */
	I2 = Qp * (1 - Spectral_Imbalance) * (1 - Leaf_Reflectance)/2;
	FEII = 0.352 + 0.022 * Tleaf - 3.4 * pow(Tleaf,2) / 10000;
	I2 = Qp * FEII * (1 - Leaf_Reflectance) / 2;

	J = (Jmax + I2  - sqrt(pow(Jmax+I2,2) - 4 * theta * I2 * Jmax ))/(2*theta);/*\ref{eqn:J}*/ 

	/* Rprintf("I2, %.1f, FEII %.1f, J %.1f \n",I2,FEII,J); */

	Citr1 = Kc * J * (Ko + O2) - 8*Ko*Gstar*Vcmax;
	Citr2 = Ko * (4 * Vcmax - J);
	Citr = Citr1 / Citr2;

	OldCi = Ca * solc(Tleaf) * 0.7; /* Initial guesstimate */
	Oi = O2 * solo(Tleaf);/*\ref{eqn:Oi}*/

	while(iterCounter < 50)
	  {
    /* Rprintf("Number of loop in c3photo=%i, Tol=%f \n",iterCounter, Tol);
    /* Rprintf("Gs=%f, Assim=%f, Ci=%f, StomWS=%f \n", Gs, Assim, Ci, StomWS);
		  /* Rubisco limited carboxylation */
		  Ac1 =  Vcmax * (Ci - Gstar) ;
		  Ac2 = Ci + Kc * (1 + Oi/Ko);
		  Ac = Ac1/Ac2;


		  /* Light lmited portion */ 
		  Aj1 = J * (Ci - Gstar) ;
		  Aj2 = 4.5*Ci + 10.5*Gstar ;
		  Aj = Aj1/Aj2;
                  if (Aj<0.0) Aj=0.0;

		  /* Limited by tri phos utilization */
		  Ap = (3 * Rate_TPu) / (1 - Gstar / Ci);


		  if(Ac < Aj && Ac < Ap){
			  Vc = Ac;
		  }else
			  if(Aj < Ac && Aj < Ap){
				  Vc = Aj;
			  }else
				  if(Ap < Ac && Ap < Aj){
					  if(Ap < 0) Ap = 0;
					  Vc = Ap;
				  }

		  Assim = Vc - Rd; /* micro mol m^-2 s^-1 */
      
      if(ws == 0) Assim *= StomWS; 
		  /* milimole per meter square per second*/
		  Gs =  ballBerry(Assim,Ca, Tleaf, RH, bb0, bb1);
      
		  if(ws == 1) Gs *= StomWS; 
      
		  if( Gs <= 0 )
			  Gs = 1e-5;

		  if(Gs > 800)
			  Gs = 800;

		  Ci = Ca - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);/*\ref{eqn:ci}*/
		  /*Ci = Ca - (Assim * 1.6 * 100) / Gs ; Harley pg 272 eqn 10. PCE 1992 */

		  if(Ci < 0)
			  Ci = 1e-5;

		 // diff = OldCi - Ci;
		//  if(diff < 0) diff = -diff;
		//  if(diff < Tol){
	//		  break;
	//	  }else{
	//		  OldCi = Ci;
	//	  }
      
      
  	if(iterCounter == 0){
			Assim0 = Assim;
			Gs0 = Gs;
			OldCi = Ci;
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
	tmp.Assim = Assim;
	tmp.Gs = Gs;
	tmp.Ci = Ci;
  tmp.GrossAssim=Assim+Rd;
  if(Assim>0){
//  Rprintf(" in C3photoSynthesis Function : Net Leaf Photosynthesis is %f, Dark Respiration is %f, Gross Assimilation is %f \n", tmp.Assim, Rd, tmp.GrossAssim);
  }
	return(tmp);
}


/* Calculate Quantum Yield */

/* double quant_yield(double sp_im, double leaf_ref, double leaf_fna, */

double solc(double LeafT){

	double tmp;

	if(LeafT > 24 && LeafT < 26){
		tmp = 1;
	}else{
		tmp = (1.673998 - 0.0612936 * LeafT + 0.00116875 * pow(LeafT,2) - 8.874081e-06 * pow(LeafT,3)) / 0.735465;
	}

	return(tmp);
}

double solo(double LeafT){

	double tmp;

	if(LeafT > 24 && LeafT < 26){
		tmp = 1;
	}else{
		tmp = (0.047 - 0.0013087 * LeafT + 2.5603e-05 * pow(LeafT,2) - 2.1441e-07 * pow(LeafT,3)) / 0.026934;
	}

	return(tmp);
}

