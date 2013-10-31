
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */
 
/*****************************************************************************
**
**  FILE:      diffusiv.c
**
**  FUNCTION:  double diffusiv()
**
**  PURPOSE:   For estimating normalized diffusivity in soils.  Method of
**             Millington and Shearer (1971) Soil Science Literature Source:
**             Davidson, E.A. and S.E. Trumbore (1995).
**             Contributed by Chris Potter, NASA Ames
**  
**  INPUTS:
**    A       - is the fraction of soil bed volume occuppied by field capacity
**              (intra-aggregate pore space), units 0-1
**    bulkden - bulk density (g/cm3)
**    wfps    - water-filled pore space (fraction 0.0-1.0)
**              (fraction of a porespace that is filled with water)
**              volumetric water / porosity
**
**  GLOBAL VARIABLES:
**    None
**
**  LOCAL VARIABLES:
**    debug      - flag to set debugging mode, 0 = off, 1 = on
**    my_theta_V - testing alternate way of computing the volumetric water
**                 content of the soil bed volume
**    PARTDENS   - particle density (g/cm3)
**    pfc        - water content of soil expressed as a percent of field
**                 capacity (%)
**    porosity   - the fraction of soil bed volume occupied by pore space (P)
**                 (A + inter-aggregate pore space), units 0-1
**                  = 1 - bulkden/particle density
**    sw_p       - the fractional liquid saturation of the P component of
**                 total pore volume
**    s_wat      - the fractional liquid saturation of the A component of
**                 total pore volume
**    theta_A    - the volume of water per unit bed volume contained in
**                 intra-aggregate pore space
**    theta_P    - the volume of water per unit bed volume contained in
**                 inter-aggregate pore space
**    theta_V    - the volumetric water content of the soil bed volume
**    tp1 .. tp8 - intermediate variables
**    vfrac      - volumetric fraction    
**
**  OUTPUT:
**    dDO - the normalized diffusivity in aggregate soil media, units 0-1
**
**  CALLED BY:
**    dailymoist()
**
**  CALLS:
**    None
**
*****************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARTDENS 2.65           /* Particle Density (g/cm3) */
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

    double diffusiv(double *A, double *bulkden, double *wfps)
    {
      double porosity;
      double theta_A, theta_P, theta_V, my_theta_V;
      double tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8;
      double vfrac;
      double s_wat, sw_p;
      double pfc;
      double dDO;
      int   debug;

      /* Initialization */
      debug = 0;

      porosity = 1.0 - (*bulkden)/PARTDENS;
      if ((porosity - (*A)) < 0.0) {
        printf("ERROR:  Mis-match between bulk density and field capacity\n");
        printf("value in soils.in file.  Ending simulation!\n");
        exit(1);
      }

      pfc = (*wfps)*100 /((*A)/porosity);
 
      if (pfc >= 100.0) {
        vfrac = ((*wfps)*porosity - *A) / (porosity - (*A));
      } else {
        vfrac = 0.0;
      }
  
      if (pfc < 100.0) {
        theta_V = (pfc/100.0)*(*A);
      } else {
        theta_V = (*A) + (min(vfrac,1.0) * (porosity - (*A)));
      }

      my_theta_V = *wfps * (1.0 - *bulkden/PARTDENS);

      theta_P = (theta_V < (*A)) ? 0.0 : theta_V - (*A);
      theta_A = (theta_V > (*A)) ? (*A) : theta_V;

      if (debug) {
        printf("In SUB, vfrac = %f\n", vfrac);
        printf("In SUB, porosity - A = %f\n", (porosity - (*A)));
        printf("In SUB, (*A) = %f\n", (*A));
        printf("In SUB, (*wfps) = %f\n", (*wfps));
        printf("In SUB, pfc = %f\n", pfc);
        printf("In SUB, porosity = %f\n", porosity);
        printf("In SUB, theta_V = %f\n", theta_V);
        printf("In SUB, my_theta_V = %f\n", my_theta_V);
        printf("In SUB, theta_P = %f\n", theta_P);
        printf("In SUB, theta_A = %f\n", theta_A);
      }

      s_wat = min(1.0, theta_V/(*A));
      sw_p = min(1.0, theta_P/(porosity-(*A)));

      tp1 = pow((1.0 - s_wat), 2.0);
      tp2 = ((*A) - theta_A) / ((*A) + (1.0 - porosity));
      tp3 = pow(tp2,(0.5*tp2 + 1.16));
      tp4 = (1.0 - pow(((porosity-(*A))),
                              (0.5*((porosity-(*A))) + 1.16)));
      tp5 = (porosity-(*A))-theta_P;
      if (tp5 > 0.0) {
        tp6 = pow(tp5, (0.5*tp5 + 1.16));
      } else {
        tp6 = 0.0;
      }
      tp7 = pow((1.0-sw_p), 2.0);
      tp8 = max(0.0, (tp1*tp3*tp4*(tp5-tp6)) /
                      (1.0E-6 + (tp1*tp3*tp4) + tp5 - tp6) * 1.0E7);

      if (debug) printf("From SUB: %f %f %f %f %f %f %f %f\n",
                        tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8);
      dDO = max(0.0, (tp8/1.0E7 + tp7*tp6));
      if (debug) {
	    printf("In SUB, dDO = %f\n", dDO);
      }

/*      dDO /= 0.20;
      dDO = min(dDO, 1.0); */

      return(dDO);
    }
