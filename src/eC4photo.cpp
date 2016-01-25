/*
 *  BioCro/src/eC4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
 *
 */

#include "math.h"
#include "eC4photo.h"

double eC4photoC(double QP, double TEMP, double RH, double CA,
		double OA, double VCMAX, double VPMAX, double VPR,
		double JMAX)
{
    /* CONSTANTS */
    const double  gs = 3 * 1e-3 ;
    /* mol m-2 s-1 physical conductance to CO2 leakage*/
    /* const double gammaStar = 0.000193  */
    const double gammaStar =  0.0002239473      ;
    /* half the reciprocal of Rubisco specificity */
    /*    const double go = 0.047 * gs     at 25 C */
    const double a = 0.01 ; /* alpha in the notes*/
    const double Kp = 80 ; /*  mu bar */
    /* const double Kc1 = 650 * 1e-3  mbar at 25 C */
    // const double Kc1 = 1020 * 1e-3; /*  mbar at 25 C */ unused

    const double theta = 0.7;

    /* ADDING THE TEMPERATURE RESPONSE FUNCTION */
    const double Ep  = 47.1 ; /* Activation energy of PEPc kj/mol */
    const double Erb = 72 ; /* Activation energy of Rubisco kj/mol */
    /*    const double Q10bf = 1.7 */
    // const double ERd = 46.39; unused
    // const double Epr = 46.8 ; /* Activation energy of PPDK kj/mol*/ unused
    const double EKc = 79.43;
    const double EKo = 36.38;
    const double Q10cb = 1.7;

    /* double Ko = 450 ;  mbar at 25 C */
    double Ko = 532.9 ; /* mbar at 25 C */
    /* double Kc = 650   mu bar at 25 C */
    double Kc = 1020 ; /*  mu bar at 25 C */
    
    /* VAriables */ 
    double Idir, AirTemp, Ca, Oa;
    double Vcmax, Vpmax, Vpr, Jmax;

    // double Q10pr, Q10Rd; unused
	double Q10p, Q10rb, Q10Kc, Q10Ko;
    double Cm, Om;
    double Rd, Rm;
    // double Rs; unused

    double I2, J, Aj0, Aj1, Aj;
    double Vp, Ko1 , Om1;
    double a1, b1, c1 , c3, Ac0;
    double AcLCO2, Ac, A;
    // double Os, Cs0, Cs1, Cs; unused


    Idir = QP;
    AirTemp = TEMP;
    Ca = CA;
    Oa = OA;
    Vcmax = VCMAX;
    Vpmax = VPMAX;
    Vpr = VPR;
    Jmax = JMAX;

    // Q10pr = exp(Epr *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15)))); unused
    // Q10Rd = exp(ERd *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15)))); unused
    Q10p = exp(Ep *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15))));
    Q10rb = exp(Erb *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15))));
    Q10Kc = exp(EKc *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15))));
    Q10Ko = exp(EKo *(1/(0.008314*298.15)-1/(0.008314*(AirTemp+273.15))));

    Vcmax = Vcmax * Q10rb;
    Kc = Kc * Q10Kc;
    Ko = Ko * Q10Ko;
    Vpmax = Vpmax * Q10p;        
    Jmax = Jmax * pow(Q10cb,(AirTemp-25)/10);

    Cm = 0.4 * Ca ; 
    Om = Oa ;
        
    /*    Rd = 0.01 * Vcmax; */
    Rd = 0.08;
    Rm = 0.5 * Rd ;
    // Rs = 0.5 * Rd; unused
        
    /* Light limited */
    I2 = (Idir * 0.85)/2;
    J = (Jmax + I2  - sqrt(pow(Jmax+I2,2) - 4 * theta * I2 * Jmax ))/2*theta;        
    Aj0 = 0.4 * J - Rm + gs * Cm;        
    Aj1 = (1-0.4)*J/3-Rd;

    if(Aj0 < Aj1){
      Aj = Aj0;
    }else{
      Aj = Aj1;
    }
        

    /* Other part */
    Vp = (Cm * Vpmax) / (Cm + Kp) ;
    if(Vpr < Vp){
      Vp = Vpr;
    }
	        
    /* Alternative formula */
    Ko1 = Ko * 1e3;
    Om1 = Om * 1e3;

    a1 = 1 - a / 0.047 * Kc/Ko1 ;
    b1 = -((Vp - Rm + gs * Cm)+
	   (Vcmax - Rd)+
	   gs*(Kc*(1+Om1/Ko1))+
	   ((a/0.047)*(gammaStar*Vcmax+Rd*Kc/Ko1)));
    c1 = (Vcmax - Rd)*(Vp-Rm+gs*Cm)-
          (Vcmax * gs * gammaStar * Om1 + Rd*gs*Kc*(1+Om1/Ko1));
         
    c3 = pow(b1,2) - 4*a1*c1;
      if(c3 < 0){
	c3 = 0;
      }
    Ac0 = (-b1 - sqrt(c3)) / 2*a1 ;

    AcLCO2 = (Cm * Vpmax/(Cm+Kp)) - Rm + gs * Cm;
    
    if(Ac0 < AcLCO2){
      Ac = Ac0;
    }else{
      Ac = AcLCO2;
    }

    if(Aj < Ac){
      A = Aj;
    }else{
      A = Ac;
    }
        
	/* Unused if-else statments
    Os = a * A / 0.047 * gs + Om;
    if ( Aj <= Ac ) {
      Cs0 = (gammaStar * Os)*(7/3*(Aj + Rd) + (1-0.4)*J/3);
      Cs1 = (1-0.4)*J/3-(Aj) ; 
       Cs = Cs0/Cs1; unused
    } else {
      Cs0 = gammaStar * Os + Kc1*(1+Os/Ko)*((Ac+Rd)/Vcmax);
          if((Ac+Rd) >= Vcmax){
	    Ac = Vcmax - Rd - 0.5 ;
	  }
	  Cs1 = 1 - (Ac+Rd)/Vcmax;
	   Cs = Cs0/Cs1; unused
    }
	*/
    return(A);
}

/* Calculates R-sq according to the Collatz model */
/* and given values for the two most important */
/* parameters Vcmax and alpha */
double RsqeC4photo(double oAssim[], double oQp[], 
		  double oTemp[],  double oRH[],
		  double COa, double O2a,  double Vcmax,
		  double Vpmax, double Vpr, double Jmax, int enObs)
{

  double vec1[enObs];
  int i, j;
  double SST = 0.0, SSE = 0.0, diff;
  double Rsquare = 0.0;

  for(i = 0;i < enObs; i++){

    vec1[i] = eC4photoC(oQp[i],oTemp[i],
		       oRH[i],COa,O2a,Vcmax,Vpmax,
		       Vpr,Jmax);
  }
   
  for(j = 0 ; j < enObs; j++){
    SST += pow(oAssim[j],2);
    diff = oAssim[j] - vec1[j];
    SSE += pow(diff,2);
  }

  Rsquare = (SST - SSE)/SST ; 
  return(Rsquare);
}

