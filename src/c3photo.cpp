/*
 *  /src/c3photo.c by Fernando Ezequiel Miguez  Copyright (C) 2009
 *
 */

#include <math.h>
#include "c3photo.h"

struct c3_str c3photoC(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax, 
               double Rd0, double bb0, double bb1, double Ca, double O2,
               double thet, double StomWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation)
{
    const double AP = 101325; /*Atmospheric pressure According to wikipedia (Pa)*/
    const double R = 0.008314472; /* Gas constant */
    const double Leaf_Reflectance = 0.2;
    const double Rate_TPu = 23;

    double Ci = 0.0;/*\ref{parm:ci}\ref{parm:Oi}*/
    double Vc = 0.0;
    double OldAssim = 0.0, Tol = 0.01;
    double Gs;
    double Assim;
    double Tk = Tleaf + 273.15;

    /* From Bernacchi 2001. Improved temperature response functions. */
    /* Note: Values in Dubois and Bernacchi are incorrect. */    
    double Kc = exp(38.05 - 79.43 / (R * Tk)); /*\ref{eqn:Kc}*/
    double Ko = exp(20.30 - 36.38 / (R * Tk)); /*\ref{eqn:K0}*/
    double Gstar = exp(19.02 - 37.83 / (R * Tk)); /*\ref{eqn:gammaast}*/

    

    double Vcmax = Vcmax0 * exp(26.35 - 65.33 / (R * Tk)); /*\ref{eqn:Vcmax}*/
    double Rd = Rd0 * exp(18.72 - 46.39 / (R * Tk));/*\ref{eqn:Rd}*/

    double theta = thet + 0.018 * Tleaf - 3.7e-4 * pow(Tleaf, 2);

    /* Light limited */
    double FEII = 0.352 + 0.022 * Tleaf - 3.4 * pow(Tleaf, 2) / 10000;
    double I2 = Qp * FEII * (1 - Leaf_Reflectance) / 2;

    double J = (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4 * theta * I2 * Jmax )) / (2 * theta);/*\ref{eqn:J}*/ 

    double Oi = O2 * solo(Tleaf);/*\ref{eqn:Oi}*/

    if(Ca <= 0)
        Ca = 1e-4;


    int iterCounter = 0;
    while (iterCounter < 50) {
          /* Rubisco limited carboxylation */
          double Ac1 =  Vcmax * (Ci - Gstar) ;
          double Ac2 = Ci + Kc * (1 + Oi / Ko);
          double Ac = Ac1 / Ac2;

          /* Light lmited portion */ 
          double Aj1 = J * (Ci - Gstar) ;
          double Aj2 = electrons_per_carboxylation * Ci + 2 * electrons_per_oxygenation * Gstar ;
          double Aj = Aj1 / Aj2;
          if (Aj < 0.0)
              Aj = 0.0;

          /* Limited by tri phos utilization */
          double Ap = (3 * Rate_TPu) / (1 - Gstar / Ci);

          if(Ac < Aj && Ac < Ap){
              Vc = Ac;
          } else if (Aj < Ac && Aj < Ap){
              Vc = Aj;
          } else if (Ap < Ac && Ap < Aj){
              if (Ap < 0)
                  Ap = 0;
              Vc = Ap;
          }

          Assim = Vc - Rd; /* micro mol m^-2 s^-1 */
      
      if (water_stress_approach == 0) Assim *= StomWS; 
          /* milimole per meter square per second*/
          Gs = ballBerry(Assim, Ca, Tleaf, RH, bb0, bb1);
      
          if (water_stress_approach == 1)
              Gs *= StomWS; 
      
          if (Gs <= 0)
              Gs = 1e-5;

          if (Gs > 800)
              Gs = 800;

          Ci = Ca_pa - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);

          if (Ci < 0)
              Ci = 1e-5;

        double diff = OldAssim - Assim;
        if (diff < 0) diff = -diff;
        if (diff < Tol) {
            break;
        } else {
            OldAssim = Assim;
        }
          
          iterCounter++;
    }
    struct c3_str tmp;
    tmp.Assim = Assim;
    tmp.Gs = Gs;
    tmp.Ci = Ci;
    tmp.GrossAssim = Assim + Rd;
    return(tmp);
}


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

