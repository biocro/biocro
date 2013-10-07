
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      denitrify.c
**
**  FUNCTION:  void denitrify()
**
**  PURPOSE:   Calculate N2O flux and N2:N2O ratio due to denitrification.
**
**  DESCRIPTION:
**    Denitrification:  Calculate N2 and N2O gases flux, reduce soil nitrate
**    by amount of total nitrogen gas flux.
**
**    Total Nitrogen losses during denitrification are controlled by soil
**    water content, available soil Carbon, and soil nitrate (NO3-).
**  
**    The fraction of Nitrogen losses as N2 or N2O are controlled by soil
**    water content and soil nitrate (NO3-).
**
**  INPUTS:
**    basef      - the fraction of N leached into baseflow
**    critflow   - critical water flow for leaching of minerals (cm H2O)
**    frlechd[]  - daily leaching fraction for each minerl, frlechd[0] for N
**    inorglch   - N from organic leaching of stream flow (base flow + storm 
**                 flow) (g/m2)
**    jday       - current Julian day (1-366)
**    newCO2     - the amount of microbial respiration that has occurred 
**                 in the past day (gC/m2)
**    newNO3     - the amount of NO3 to be added to the soil column (g/m2)
**    nitrate[]  - total nitrate in soil mineral pool, distributed among soil
**                 water model layers (gN/m2)
**    stdbulkd   - a standard bulk density for the soil based on
**                 sand/silt/clay percentages (g/cm^3)
**    stdfieldc  - a standard field capacity for the soil based on
**                 sand/silt/clay percentages (volumetric fraction)
**    stream[]   - Century's stream output (stream flow & amount leached into
**                 stream flow)
**    stormf     - the fraction of N leached into stormflow
**                 (baseflow_N + stormflow_N = stream(2))
**    wfluxout[] - the amount of water moving through the bottom of a soil
**                 layer (cm H2O) (positive is downward, negative is upward)
**
**  GLOBAL VARIABLES:
**    MAXLYR   - maximum number of soil water model layers (21)
**    PARTDENS - particle density (g/cm3)
**
**  EXTERNAL VARIABLES:
**    layers           - soil water soil layer structure
**    layers->bulkd[]  - bulk density by layer (g/cm3)
**    layers->dpthmn[] - tops of soil layers (depth from surface in cm)
**    layers->dpthmx[] - bottoms of soil layers (depth from surface in cm)
**    layers->fieldc[] - volumetric water content at field capacity for layer
**                       (cm H2O/cm of soil)
**    layers->numlyrs  - total number of layers in the soil water model soil
**                       profile
**    layers->wfps[]   - water-filled pore space by layer (fraction 0.0-1.0) 
**                      (fraction of a porespace that is filled with water)
**    layers->width[]  - the thickness of soil water model layers (cm)
**
**  LOCAL VARIABLES:
**    A[]             - parameters to Parton-Innis functions
**    a               - intermediate variable for calculations
**    CM_per_METER    - 1 meter = 100 cm
**    co2_correction  - correction factor when water filled pore space has
**                      reached a threshold value
**    co2PPM[]        - CO2 concentration by layer (ppm)
**    dD0_fc          - the normalized diffusivity in aggregate soil media, at
**                      a standard field capacity (0-1)
**    debug           - flag to set debugging mode, 0 = off, 1 = on
**    Dtotflux        - total soil (N2 + N2O) denitrification flux
**                      (ppm N/day) 
**    excess          - amount of N flux calculated in excess of what can
**                      actually be removed from soil (gN/m2/day)
**    fDco2           - denitrification flux due to CO2 concentation
**                      (ppm N/day)
**    fDno3           - denitrificaton flux due to soil nitrate (ppm N/day) 
**    fDwfps          - the effect of wfps on Denitrification (0-1)
**    fluxout         - actual total trace gas flux (gN/m2/day)
**    fRno3_co2       - effect of nitrate and CO2 on the ratio N2:N2O
**    fRwfps          - the effect of wfps on the ratio N2:N2O
**    grams_per_ug    - grams per microgram
**    grams_soil[]    - the grams of soil in a layer in a 1m x 1m section
**    ilyr            - current layer from which N2O flux is occurring
**    K               - intermediate value for computing k[]
**    k[]             - proportion of newNO3 to distribute to each layer
**    k1              - intermediate variable for calulations
**    ksum            - k summed over all layers
**    M               - intermediate variable for calulations
**    min_nitrate     - mimimum nitrate concentration required in a soil layer
**                      for trace gas calculation (ppm N)
**    min_nitrate_end - minimum allowable nitrate per layer at end of day
**                      (ppm N)
**    n2frac          - fraction of nitrate flux that goes to N2 (0.0 - 1.0)
**    n2oflux         - N2O flux during Denitrification (g N/m2/day) 
**    n2ofrac         - fraction of nitrate flux that goes to N2O (0.0 - 1.0)
**    nitratePPM[]    - soil nitrate (NO3-) concentration (ppm)
**    ntotflux[]      - soil (N2 + N2O) denitrification flux, by layer
**                      (gN/m2)
**    Rn2n2o          - ratio of N2 to N2O
**    ug_per_gram     - micrograms per gram (ppm)
**    wfps_fc[]       - water filled pore space when layer is at field
**                      capacity (0-1)
**    WFPS_threshold  - treshold value for water filled pore space (0-1)
**    x_inflection    - inflection point on x-axis
**
**  OUTPUTS:
**    dN2lyr[]  - N2 flux from Denitrification by layer (gN/m2/day) 
**    dN2Olyr[] - N2O flux from Denitrification by layer (gN/m2/day) 
**    Dn2flux   - N2 flux from Denitrification (gN/m2/day) 
**    Dn2oflux  - N2O flux from Denitrification (gN/m2/day) 
**    inorglch  - inorganic N leaching (gN/m2)
**    newNO3    - the amount of NO3 to be added to the soil column (g/m2)
**    nitrate[] - total nitrate in soil mineral pool, distributed among soil
**                water model layers (gN/m2)
**    stream[]  - Century's stream output (stream flow & amount leached into
**                stream flow)
**
**  CALLED BY:
**    trace_gas_model()
**
**  CALLS:
**    diffusiv()       - estimate normalized diffusivity in soils
**    f_arctangent()   - arctangent function
**    leachdly()       - daily nitrate leaching thru soil structure
**    wfps()           - compute the daily water-filled pore space by layer
**  
*****************************************************************************/

#include "n2o_model.h"
#include "soilwater.h"
#include <math.h>

#define  M_PI 3.1415926536

    void denitrify(double *newCO2, double *newNO3, double nitrate[MAXLYR],
                   double wfluxout[MAXLYR], double *critflow, double frlechd[],
                   double stream[], double *basef, double *stormf,
                   double *inorglch, double *Dn2oflux, double *Dn2flux,
                   double stdfieldc, double stdbulkd, 
                   double dN2lyr[], double dN2Olyr[], int *jday)
    { 
      int    debug;
      int    ilyr;
      double a;
      double fDno3; 
      double fDco2; 
      double fDwfps; 
      double Dtotflux; 
      double fRno3_co2;
      double fRwfps;
      double A[4];
      double ntotflux[MAXLYR];
      double n2oflux;
      double grams_soil[MAXLYR];
      double nitratePPM[MAXLYR];
      double wfps_fc[MAXLYR];
      double co2PPM[MAXLYR];
      double co2_correction[MAXLYR];
      double Rn2n2o;
      double n2ofrac, n2frac;
      double excess;
      double min_nitrate;
      double min_nitrate_end;
      double fluxout;
      double K;
      double k[MAXLYR], ksum;
      double k1, M;
      double  dD0_fc;
      double  x_inflection;
      double  WFPS_threshold;
      double ug_per_gram;
      double grams_per_ug;
      double  CM_per_METER;

      extern LAYERPAR_SPT layers;
      extern SITEPAR_SPT sitepar;

      /* Initializations */
      debug = 0;
      min_nitrate = 0.1;
      min_nitrate_end = 0.05;
      K = -0.1;
      ug_per_gram = 1.0E6;
      grams_per_ug = 1.0E-6;
      CM_per_METER = 100.0;
      for (ilyr=0; ilyr < MAXLYR; ilyr++) {
        co2PPM[ilyr] = 0.0;
        dN2lyr[ilyr] = 0.0;
        dN2Olyr[ilyr] = 0.0;
      }

      *Dn2oflux = 0.0;
      *Dn2flux = 0.0;

      ksum = 0.0;
      for (ilyr=0; ilyr < layers->numlyrs; ilyr++) {
/*        k[ilyr] = exp(K*layers->dpthmn[ilyr]) -  exp(K*layers->dpthmx[ilyr]); */
        /* Use rooting density from soils.in file to weight CO2 by depth, */
        /* cak - 04/07/04 */
        k[ilyr] = max(layers->tcoeff[ilyr], 0.001);
        ksum += k[ilyr];
        ntotflux[ilyr] = 0.0;
      }

      for (ilyr=0; ilyr < layers->numlyrs; ilyr++) {
        k[ilyr] /= ksum;
        nitrate[ilyr] += k[ilyr] * (*newNO3);
      }
      /* newNO3 used up after being distributed thru the soil profile */
      *newNO3 = 0;

      wfps(layers);

      /* Mineral leaching */
        
      leachdly(wfluxout, layers->numlyrs, nitrate, *critflow, frlechd, stream,
               *basef, *stormf, inorglch);

      for (ilyr=0; ilyr < layers->numlyrs; ilyr++) {

        /* Denitrification occurs over all layers */
        /* Convert nitrate (gN/m2) to nitratePPM (ppm N) */
        grams_soil[ilyr] = layers->bulkd[ilyr] * layers->width[ilyr] *
                           CM_per_METER * CM_per_METER;
        nitratePPM[ilyr] = nitrate[ilyr] / grams_soil[ilyr] * ug_per_gram;
        if (nitratePPM[ilyr] < min_nitrate) {
          if (debug) {
            fprintf(stdout, "CANNOT DENITRIFY layer %1d, nitrate[%1d] = %f\n",
                    ilyr, ilyr, nitrate[ilyr]);
          }
          continue;
        }
        co2PPM[ilyr] = k[ilyr] * (*newCO2) / grams_soil[ilyr] *
                       ug_per_gram;
/*        wfps_fc[ilyr] = stdfieldc / (1.0 - stdbulkd/PARTDENS); */
        wfps_fc[ilyr] = layers->fieldc[ilyr] /
                        (1.0 - layers->bulkd[ilyr]/PARTDENS);
/*        dD0_fc = diffusiv(&stdfieldc, &stdbulkd, &wfps_fc[ilyr]); */
        /* dD0 calc changed 6/20/00 -mdh */
        dD0_fc = diffusiv(&layers->fieldc[ilyr], &layers->bulkd[ilyr],
                          &wfps_fc[ilyr]);
        WFPS_threshold = (dD0_fc >= 0.15) ? 0.80 : (dD0_fc*250 + 43)/100;
        if (layers->wfps[ilyr] <= WFPS_threshold) {
          co2_correction[ilyr] =  co2PPM[ilyr];
        } else {
          a = (dD0_fc >= 0.15) ? 0.004 : (-0.1 * dD0_fc + 0.019);
          co2_correction[ilyr] = co2PPM[ilyr] * (1.0 + a *
                                 (layers->wfps[ilyr] - WFPS_threshold)*100);
        }

        /* Compute the Nitrate effect on Denitrification */
        /* Changed NO3 effect on denitrification based on */
        /* paper  "General model for N2O and N2 gas emissions from */
        /* soils due to denitrification", Del Grosso et. al, GBC, */
        /* 12/00,  -mdh 5/16/00 */

/*        fDno3 = 1.15 * pow(nitratePPM[ilyr], 0.57);*/
        A[0] = 9.23;
        A[1] = 1.556;
        A[2] = 76.91;
        A[3] = 0.00222;

        fDno3 = f_arctangent(nitratePPM[ilyr], A);
        fDno3 = max(0.0, fDno3);

        /* Compute the Carbon Dioxide effect on Denitrification */
        /* (fDco2, ppm N) */
        /* Changed CO2 effect on denitrification based on */
        /* paper "General model for N2O and N2 gas emissions from */
        /* soils due to denitrification", Del Grosso et. al, GBC, */
        /* 12/00,  -mdh 5/16/00 */

/*        fDco2 = 0.1 * pow(co2PPM[ilyr], 1.3);*/
        /* The CO2 effect calculation should take into account the */
        /* corrected CO2 concentration, cak - 07/31/02 */
/*        fDco2 = max(0.0, ((0.1 * pow(co2PPM[ilyr], 1.3)) - min_nitrate)); */
        fDco2 = max(0.0,
                    ((0.1 * pow(co2_correction[ilyr], 1.3)) - min_nitrate));

        /* Compute wfps effect on denitrification, (fDwfps, 0-1) */
        /* Changed wfps effect on denitrification based on */
        /* paper "General model for N2O and N2 gas emissions from */
        /* soils due to denitrification", Del Grosso et. al, GBC, */
        /* 12/00,  -mdh 5/16/00 */

        M = min(0.113, dD0_fc) * (-1.25) + 0.145;
        /* The x_inflection calculation should take into account the */
        /* corrected CO2 concentration, cak - 07/31/02 */
/*        x_inflection = (9.0 - M); */
        x_inflection = (9.0 - M * co2_correction[ilyr]);

        /* Changed fDwfps calculation - cak - 9/18/00 */
/*        fDwfps = 0.5 +
                 (atan(0.6*M_PI*(0.1*layers->wfps[ilyr]*100-x_inflection))) /
                 M_PI; */
        fDwfps = (0.45 +
                         (atan(0.6*M_PI*(10.0*layers->wfps[ilyr]-
                          x_inflection))) / M_PI);
        fDwfps = max(0.0, fDwfps);

        if (debug) {
          fprintf(stdout, "\n%6s  %6s  %6s\n","fDwfps","fDno3","fDco2");
          fprintf(stdout, "%6.4lf  %6.4lf  %6.4lf\n", fDwfps,fDno3,fDco2); 
        }
   
        /* Compute the N fluxes (N2 + N2O) for the current layer, ppm N */
        /* If desired, do not include CO2 effect on denitrification between */
        /* given Julian days, cak - 02/19/04 */
        if (*jday >= sitepar->jdayStart && *jday <= sitepar->jdayEnd) {
          Dtotflux = fDno3;
        } else {
          Dtotflux = (fDno3 < fDco2) ? fDno3 : fDco2;
        }

        /* Minimum value for potential denitrification in top 2 soil layers */
        /* ppm N, 9/18/00 -cindyk */
        if (ilyr < 2) {
          Dtotflux = max(0.066, Dtotflux);
        }
        /* Account for water limitation */
        Dtotflux *= fDwfps;
        if (debug) {
          fprintf(stdout, "Dtotflux = %8.4lf\n", Dtotflux);
        }

        /* Nitrate effect on the ratio of N2 to N2O.  */
        /* Maximum N2/N2O ratio soil respiration function */
        /* Changed the NO3 and CO2 effect on the N2/N2O ratio based on */
        /* paper "General model for N2O and N2 gas emissions from */
        /* soils due to denitrification", Del Grosso et. al, GBC, */
        /* 12/00,  -mdh 5/16/00 */
        /* fRno3_co2 estimates the ratio as a function of electron */
        /* donor to substrate -mdh 5/17/00 */

        k1 = max(1.5, 38.4 - 350 * dD0_fc);
        fRno3_co2 = max(0.16 * k1, k1 * exp(-0.8 * nitratePPM[ilyr]/co2PPM[ilyr]));

        /* WFPS effect on the N2/N2O Ratio */
        /* Changed wfps effect on the N2/N2O ratio based on */
        /* paper "General model for N2O and N2 gas emissions from */
        /* soils due to denitrification", Del Grosso et. al, GBC, */
        /* 12/00,  -mdh 5/16/00 */

        fRwfps = max(0.1, 0.015 * layers->wfps[ilyr]*100 - 0.32);

        /* Compute the N2:N2O Ratio */

        Rn2n2o = fRno3_co2 * fRwfps; 
        if (Rn2n2o < 0.1) {
          Rn2n2o = 0.1;
        }

        if (debug) {
          fprintf(stdout, "\n%6s  %8s  %8s  %9s  %2s  %3s\n","fRwfps",
                  "nitratePPM","co2PPM","fRno3_co2","k1","dD0");
          fprintf(stdout, "%6.4lf  %6.4lf  %6.4lf  %6.4lf  %6.4lf  %6.4lf\n",
                  fRwfps,nitratePPM[ilyr],co2PPM[ilyr],fRno3_co2,k1,dD0_fc);
          fprintf(stdout, "Ratio N2:N2O = %12.10lf\n", Rn2n2o);
        }

        /* Compute N2 and N2O flux by layer (Denitrification) */
        /* convert ppm N to gN/m^2 */
   
        ntotflux[ilyr] = Dtotflux * grams_soil[ilyr] * grams_per_ug;
        n2oflux = ntotflux[ilyr] / (Rn2n2o + 1.0);
        *Dn2oflux += n2oflux;
        dN2Olyr[ilyr] = n2oflux;
        *Dn2flux += ntotflux[ilyr] - n2oflux;
        dN2lyr[ilyr] = ntotflux[ilyr] - n2oflux;
        if (debug) {
          fprintf(stdout, "\n%3s  %6s  %6s  %6s  %6s\n", "lyr", "n2o", "n2",
                  "co2", "no3");
          fprintf(stdout, "%3d  %6.4lf  %6.4lf  %6.4lf\n", ilyr,
                  n2oflux, co2PPM[ilyr], nitratePPM[ilyr]);
        }
      } /* for ilyr */

      if (debug) {
        fprintf(stdout, "\nIn denitrify, Dn2oflux (gN/m2/day) = %12.10lf\n",
                        *Dn2oflux);
        fprintf(stdout, "In denitrify, Dn2flux (gN/m2/day) = %12.10lf\n",
                        *Dn2flux);
      }

      if (*Dn2oflux < 1.0E-25) {
        *Dn2oflux = 0.0;
      }
      if (*Dn2flux < 1.0E-25) {
        *Dn2flux = 0.0;
      }

      if (debug) {
        fprintf(stdout, "In denitrify(1), Dn2oflux = %12.10lf\n", *Dn2oflux);
        fprintf(stdout, "In denitrify(1), Dn2flux = %12.10lf\n", *Dn2flux);
      }

      /* Reduce nitrate in soil by the amount of N2-N N2O-N that is lost */
      /* Do not let nitrate in any layer go below min_nitrate */

      if (*Dn2oflux + *Dn2flux > 1.0E-30) {
        n2ofrac = *Dn2oflux/(*Dn2oflux + *Dn2flux);  
        n2frac = *Dn2flux/(*Dn2oflux + *Dn2flux);  
        excess = 0.0;
   
        for (ilyr=0; ilyr < layers->numlyrs; ilyr++) {
          if (nitratePPM[ilyr] < min_nitrate) {
            /* No trace gas flux from this layer */
            excess += ntotflux[ilyr];
            if (debug) {
              printf("First IF check in loop, excess = %12.10lf\n", excess);
            }
          } else if ((nitrate[ilyr] - ntotflux[ilyr]) >
                      (min_nitrate_end * grams_soil[ilyr] * grams_per_ug)) {
            /* Remove N in calculated trace gas flux from the layer */
            nitrate[ilyr] -= ntotflux[ilyr];
            if (debug) {
              printf("Second IF check in loop, nitrate[%1d] = %12.10lf", ilyr,
                     nitrate[ilyr]);
              printf("    nitratePPM[%1d] = %12.10lf\n", ilyr, nitratePPM[ilyr]);
            }
          } else {
            /* Reduce trace gas flux in layer so soil N won't fall below */
            /* the minimum value */
            fluxout = (nitratePPM[ilyr] - min_nitrate_end) *
                      grams_soil[ilyr] * grams_per_ug; 
            excess += (ntotflux[ilyr] - fluxout); 
            nitrate[ilyr] = min_nitrate_end * grams_soil[ilyr] * grams_per_ug;
            if (debug) {
              printf("Third IF check in loop, excess = %12.10lf\n",excess);
              printf("nitrate[%1d] = %12.10lf", ilyr, nitrate[ilyr]);
              printf("     nitratePPM[%1d] = %12.10lf\n", ilyr, nitratePPM[ilyr]);
            }
          }
        } /* for ilyr */

        *Dn2oflux -= n2ofrac * excess;
        *Dn2flux -= n2frac * excess;

        if (debug) {
          printf("   n2ofrac = %12f\n", n2ofrac);
          printf("   n2frac = %12f\n", n2frac);
          fprintf(stdout, "   n2o_excess = %12.10lf\n", n2ofrac * excess);
          fprintf(stdout, "   n2_excess = %12.10lf\n", n2frac * excess);
        }
      } else {
        *Dn2oflux = 0.0;
        *Dn2flux = 0.0;
      }

      if (debug) {
        fprintf(stdout, "In denitrify(2), Dn2oflux = %12.10lf\n", *Dn2oflux);
        fprintf(stdout, "In denitrify(2), Dn2flux = %12.10lf\n", *Dn2flux);
      }

      return;
    }
