
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      methane_oxidation.c
**
**  FUNCTION:  void methane_oxidation()
**
**  PURPOSE:   Calculate methane oxidation.
**
**  DESCRIPTION:
**    Methane oxidation is a function of soil water content, temperature,
**    porosity, and field capacity.
**
**  REFERENCE:
**    General CH4 Oxidation Model and Comparison of CH4 Oxidation in Natural
**    and Managed Systems.  S.J. Del Grosso, W.J. Parton, A.R. Mosier, D.S.
**    Ojima, C.S. Potter, W. Borken, R. Brumme, K. Butterbach-Bahi, P.M.
**    Crill, K. Dobbie, and K.A. Smith.  2000.  Global Biogeochemical Cycles
**    14:999-1019.
**
**  INPUTS:
**    isdecid - flag, set to 1 for deciduous forest, 0 otherwise
**    isagri  - flag, set to 1 for agricultural system or grassland that has
**              been fertilized or cultivated, 0 otherwise
**
**  GLOBAL VARIABLES:
**    PARTDENS - particle density (g/cm3) (2.65)
**
**  EXTERNAL VARIABLES:
**    layers           - soil water soil layer structure
**    layers->bulkd[]  - bulk density by layer (g/cm3)
**    layers->dpthmn[] - tops of soil layers (depth from surface in cm)
**    layers->dpthmx[] - bottoms of soil layers (depth from surface in cm)
**    layers->fieldc[] - volumetric water content at field capacity for layer
**                       (cm H2O/cm of soil)
**    layers->numlyrs  - total number of layers in the soil water model
**                       soil profile
**    layers->wfps[]   - water-filled pore space by layer (fraction 0.0-1.0) 
**                       (fraction of a porespace that is filled with water)
**    layers->width[]  - the thickness of soil water model layers (cm)
**    soil             - soil temperature structure
**    soil->soiltavg[] - average soil temperature of layer (degrees C)
**
**  LOCAL VARIABLES:
**    agri_adjust   - adjustment factor for agricultural soils
**    bulkdensity   - weighted average of bulk density in top 15 cm of soil
**                    (g/cm3)
**    CH4DEPTH      - maximum depth at which CH4 oxidation occurs
**    CH4max        - maximum CH4 oxidation rate (gC/ha/day)
**    Dopt          - ratio of gas diffusivity through soil at Wopt to gas
**                    diffusivity through air
**    fieldcapacity - weighted average field capacity of top 15 cm of soil
**                    (cm H20 / cm of soil)
**    ilyr          - current layer from which methane oxidation is occurring
**    percentlayer  - percentage of a soil layer that will be used in the CH4
**                    calculations if the soil layer profile does not break at
**                    15 cm
**    soiltemp      - weighted average of soil temperature in top 15 cm of
**                    soil (deg C)
**    soilwater     - soil water content of top 15 cm of soil
**                    (volumetric, cm3 H2O / cm3 soil * 100)
**    temp          - intermediate storage variable for diffusivity
**                    calculation
**    temp_adjust   - adjustment factor for soil temperature
**    watr_adjust   - adjustment factor for water limitation
**    wfps          - weighted average of water filled pore space in top 15 cm
**                    of soil (fraction 0.0-1.0)
**    wfps_adjust   - adjustment factor for water filled pore space
**    Wmax          - maximum water content
**    Wmin          - minimum water content
**    Wopt          - optimum water content
**
**  OUTPUTS:
**    CH4 - methane oxidation (gC/ha/day)
**
**  CALLED BY:
**    trace_gas_model()
**
**  CALLS:
**    diffusiv() - estimate normalized diffusivity in soils
**  
*****************************************************************************/

#include "soilwater.h"
#include "n2o_model.h"
#include <math.h>
#include <stdio.h>

#define CH4DEPTH 15.0

    void methane_oxidation(double *CH4, int *isdecid, int *isagri) 
    { 
      int    ilyr;
      double  bulkdensity;
      double  fieldcapacity;
      double soiltemp;
      double soilwater;
      double wfps;
      double CH4max;
      double  Dopt;
      double Wmin;
      double Wmax;
      double  Wopt;
      double agri_adjust;
      double temp_adjust;
      double watr_adjust;
      double wfps_adjust;
      double  percentlayer;
      double  temp;

      extern LAYERPAR_SPT layers;
      extern SOIL_SPT soil;

      /* Compute a weighted average for soil temperature, field capacity, */
      /* bulk density, water filled pore space, and volumetric soil water */
      /* content in top 15 cm of soil profile */
      ilyr = 0;
      bulkdensity = 0.0;
      fieldcapacity = 0.0;
      soiltemp = 0.0;
      soilwater = 0.0;
      wfps = 0.0;
      for (ilyr = 0; ilyr < layers->numlyrs; ilyr++) {
        if (layers->dpthmn[ilyr] < CH4DEPTH) {
          if (layers->dpthmx[ilyr] <= CH4DEPTH) {
            bulkdensity += layers->bulkd[ilyr] * layers->width[ilyr] / 
                           CH4DEPTH;
            fieldcapacity += layers->fieldc[ilyr] * layers->width[ilyr] /
                             CH4DEPTH;
            soiltemp += soil->soiltavg[ilyr] * layers->width[ilyr] / CH4DEPTH;
            soilwater += layers->wfps[ilyr] * layers->width[ilyr] / CH4DEPTH;
            wfps += layers->wfps[ilyr] * layers->width[ilyr] / CH4DEPTH;
          } else if ((layers->dpthmx[ilyr] - layers->dpthmn[ilyr]) > 0.0) {
            percentlayer = (CH4DEPTH - layers->dpthmn[ilyr]) / 
                           (layers->dpthmx[ilyr] - layers->dpthmn[ilyr]);
            bulkdensity += layers->bulkd[ilyr] * layers->width[ilyr] /
                           CH4DEPTH * percentlayer;
            fieldcapacity += layers->fieldc[ilyr] * layers->width[ilyr] /
                             CH4DEPTH * percentlayer;
            soiltemp += soil->soiltavg[ilyr] * layers->width[ilyr] /
                        CH4DEPTH * percentlayer;
            soilwater += layers->wfps[ilyr] * layers->width[ilyr] / CH4DEPTH *
                         percentlayer;
            wfps += layers->wfps[ilyr] * layers->width[ilyr] / CH4DEPTH *
                    percentlayer;
          }
        }
      }
      /* Convert from water filled pore space to volumetric water */
      soilwater = soilwater * (1.0 - (bulkdensity / PARTDENS));
      soilwater *= 100.0;

      /* CH4 oxidation for a deciduous system */
      if (*isdecid) {
        CH4max = 40.0 - 18.3 * bulkdensity;
        temp_adjust = 0.0209 * soiltemp + 0.845;
        /* Use bounded value for wfps_adjust if wfps falls below a critical */
        /* value, cak - 11/12/02 */
        if (wfps <= 0.05) {
          wfps_adjust = 0.1;
        } else {
          wfps_adjust = pow((10.0 * wfps - 0.5) / (1.84 - 0.5), 0.13);
          wfps_adjust *= pow((10.0 * wfps - 55) / (1.84 - 55),
                             (0.13 * (55 - 1.84)) / (1.84 - 0.5));
          wfps_adjust = max(0.1, wfps_adjust);
        }
        *CH4 = CH4max * wfps_adjust * temp_adjust;

      } else {
        /* CH4 oxidation for a grassland/coniferous/tropical system */
        Wmin = 3.0 * fieldcapacity - 0.28;
        Wopt = 6.3 * fieldcapacity - 0.58;
        Wmax = 10.6 * fieldcapacity + 1.9;
        temp = Wopt * 0.1 / (1.0 - (bulkdensity / PARTDENS));
        Dopt = diffusiv(&fieldcapacity, &bulkdensity, &temp);
        CH4max = 53.8 * Dopt + 0.58;
        if ((0.1*soilwater < Wmin) || 0.1*soilwater > Wmax) {
          watr_adjust = 0.1;
        } else {
          watr_adjust = pow(((0.1 * soilwater - Wmin) / (Wopt - Wmin)), 0.4) *
                        pow(((0.1 * soilwater - Wmax) / (Wopt - Wmax)), 
                             ((0.4 * (Wmax - Wopt)) / (Wopt - Wmin))); 
          watr_adjust = max(0.1, watr_adjust);
        }
        if (*isagri) {
          if (Dopt < 0.15) {
            agri_adjust = 0.9;
          } else if (Dopt > 0.28) {
            agri_adjust = 0.28;
          } else {
            agri_adjust = -4.6 * Dopt + 1.6;
          }
        } else {
          agri_adjust = 1.0;
        }
        temp_adjust = (soiltemp * max(0.11, Dopt) * 0.095) + 0.9;
        *CH4 = CH4max * watr_adjust * temp_adjust * agri_adjust;
      }

      return;
    }
