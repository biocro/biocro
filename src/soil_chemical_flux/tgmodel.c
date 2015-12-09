
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      tgmodel.c
**
**  FUNCTION:  void trace_gas_model() 
**
**  PURPOSE:   Trace Gas Model - calculates daily N2, N2O, NO fluxes from the
**             soil.
**
**  HISTORY:   New module routine, pulled from dailymoist. -mdh 6/20/00
**  
**  INPUTS:
**    afiel      - field capacity in the top 15cm (vol.frac)
**    ammonium   - ammonium concentration (gN/m^2)
**    avgwfps    - avg wfps in top 3 soil layers (~top 15 cm) 0-1
**    basef      - fraction of base flow
**    bulkd      - bulk density (g/cm^3)
**    CH4        - methane oxidation (gC/ha/day)
**    clay       - fraction of clay
**    critflow   - amount of flow between lyrs to leach (cm H2O)
**    dN2lyr[]   - N2 flux from Denitrification by layer (gN/m2/day) 
**    dN2Olyr[]  - N2O flux from Denitrification by layer (gN/m2/day) 
**    frlechd[]  - leaching fractions (?)
**    inorglch   - inorganic N leaching (gN/m^2) (?)
**    isdecid    - flag, set to 1 for deciduous forest, 0 otherwise
**    isagri     - flag, set to 1 for agricultural system or grassland that
**                 has been fertilized or cultivated, 0 otherwise
**    jday       - current Julian day (1-366)
**    maxt       - max of montly max temperatures (deg C)
**    newCO2     - amount of soil respiration (gC/m^2/day)
**    newminrl   - net mineralization (gN/m^2/day)
**    nitrate[]  - nitrate concentration by lyr (gN/m^2)
**    ppt        - daily precip (cm)
**    sand       - fraction of sand
**    silt       - fraction of silt
**    snow       - snow cover (cm SWE)
**    stormf     - fraction of storm flow
**    stream[]   - stream flow (?)
**    texture    - soil texture index (see swconst.h)
**    time       - current time in years
**    wfluxout[] - amount of flow between lyrs (cm H2O)
**    grass_lai        - LAI of grass/crop leaf canopy (m^2/m^2)
**    tree_lai         - LAI of tree leaf canopy (m^2/m^2)
**
**  GLOBAL VARIABLES:
**    MAXLYR - maximum number of soil water model layers (21)
**    PI     - pi (3.14159265)
**
**  EXTERNAL VARIABLES:
**    sitepar            - site parameter structure
**    sitepar->N2Oadjust - nitrification N2O adjustment factor (0.0-1.0)
**
**  LOCAL VARIABLES:
**    canopy_reduction - reduction factor for NO absorbed by canopy (0-1)
**    dDO              - normalized diffusivity in aggregate soil media (0-1)
**    debug            - flag to set debugging mode, 0 = off, 1 = on
**    ilyr             - current layer in the soil profile
**    krainNO          - increase of NO due to moisture and rain >=1.0
**    netmn_to_no3     - fraction of new net mineralization that goes to NO3
**    newNH4           - new NH4 produced (gN/m^2/day)
**    newNO3           - new NO3 produced (gN/m^2/day)
**    NH4_to_NO        - ammonium converted to NO to reach potential NO flux
**                       (gN/m^2/day)
**    nh4_to_no3       - amount of NH4 converted to NO3 by nitrification
**                       (gN/m^2/day)
**    NO_N2O_ratio     - NO/N2O ratio <= 1.0
**    NOabsorp         - NO absorbed by canopy (gN/m^2/day)
**    npool_sum        - sum of N in nitrate and ammonium pools (gN/m^2)
**    nreduce          - reduction factor on nitrification rates due to
**                       nitrification inhibitors added with fertilizer (0-1)
**    potential_NOflux - maximum possible NO flux based on NO/N2O and krain
**                       (gN/m^2/day)
**    stdbulkd         - standard bulk density based on soil texture (g/cm^3)
**    stdfieldc        - standard field capacity based on soil texture
**                       (vol.frac)
**    total_lai        - total LAI at site (m^2/m^2)
**    turnovfrac       - fraction of new NO3 that goes to N2O
**
**  OUTPUTS:
**    ammonium   - ammonium concentration (gN/m^2)
**    CH4        - methane oxidation (gC/ha/day)
**    dN2lyr[]   - N2 flux from Denitrification by layer (gN/m2/day) 
**    dN2Olyr[]  - N2O flux from Denitrification by layer (gN/m2/day) 
**    Dn2flux    - denitrification N2 flux (gN/m^2/day)
**    Dn2oflux   - denitrification N2O flux (gN/m^2/day)
**    inorglch   - inorganic N leaching (gN/m^2) (?)
**    nitamt     - gross nitrification (gN/m^2/day)
**    nitrate[]  - nitrate concentration by lyr (gN/m^2)
**    Nn2oflux   - nitrification N2O flux (gN/m^2/day)
**    NOflux     - NO flux (gN/m^2/day)
**    NOabsorp_grass   - NO absorbed by grass canopy (gN/m^2/day)
**    NOabsorp_tree    - NO absorbed by tree canopy (gN/m^2/day)
**    stream[]   - stream flow (?)
**
**  CALLED BY:
**    dailymoist() - FORTRAN routine
**
**  CALLS:
**    denitrify()   - calculate N2O flux and N2:N2O ratio due to
**                    denitrification
**    diffusiv()    - estimate normalized diffusivity in soils
**    getsoilprop() - determine soil texture class based on percent of sand,
**                    silt, clay
**    nitrify()     - nitrification, produces N2O gas
**    nox_pulse()   - increase NO due to moisture and rain
**
*****************************************************************************/
#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "soilwater.h"
#include "n2o_model.h"
#include "swconst.h"


    void trace_gas_model(int *jday, double *time, double *newminrl, double *ammonium, double nitrate[],
                         int *texture, double *sand, double *silt, double *clay,
                         double *afiel, double *bulkd, double *maxt, double *ppt,
                         double *snow, double *avgwfps, double *stormf,
                         double *basef, double frlechd[], double stream[],
                         double *inorglch, double *critflow, double wfluxout[],
                         double *newCO2, double *NOflux, double *Nn2oflux, double *Dn2oflux,
                         double *Dn2flux, double *CH4, int *isdecid,
                         int *isagri, double *grass_lai, double *tree_lai,
                         double *NOabsorp_grass, double *NOabsorp_tree,
                         double *nit_amt, double *nreduce, 
                         double dN2lyr[], double dN2Olyr[],SITEPAR_SPT sitepar,LAYERPAR_SPT layers,SOIL_SPT soil)
    {

      /* Local Variables */

      int    debug = 0;
      int    ilyr;
      int *soilclassification;
/*      double netmn_to_no3 = 0.20; */
      double netmn_to_no3 = 0.0;
      double turnovfrac = 0.02;
      double newNH4;
      double newNO3;
      double nh4_to_no3;
      double krainNO;
      double potential_NOflux;
      double dDO;
      double stdbulkd;
      double stdfieldc;
      double NO_N2O_ratio;
      double NH4_to_NO;
      double npool_sum;
      double canopy_reduction;
      double NOabsorp;
      double total_lai;

//      extern SITEPAR_SPT sitepar;

      *Nn2oflux = 0.0;
      *NOflux = 0.0;
      *Dn2oflux = 0.0;
      *Dn2flux = 0.0;

      /* Compute fraction of new mineralization that is converted to NH4 */
      /* and NO3 */

      if (debug) {
        printf("newminrl = %6.4lf\n", *newminrl);
      }

      if (*newminrl <= 0.0) {

        /* Immobilization */
        /* Distribute N loss proportionally between ammonium and nitrate   */
        /* layers.  There is no check that these N pools won't go negative */
        /* once immobilization is accounted for.  It is assumed that the   */
        /* immobilization calculation by the decomp model is moderated by  */
        /* the supply of minerl N.                                         */

        npool_sum = (*ammonium > 0.0) ? *ammonium : 0.0;
        for (ilyr=0; ilyr < MAXLYR; ilyr ++) {
          npool_sum += (nitrate[ilyr] > 0.0) ? nitrate[ilyr] : 0.0;
        }
        if (*ammonium > 0.0) {
          *ammonium += *newminrl * (*ammonium / npool_sum);
        }
        for (ilyr=0; ilyr < MAXLYR; ilyr ++) {
          if (nitrate[ilyr] > 0.0) {
            nitrate[ilyr] += *newminrl * (nitrate[ilyr] / npool_sum);
          }
        }
        newNH4 = 0.0;
        newNO3 = 0.0;
      } else {
        /* Mineralization */
        newNH4 = *newminrl * (1.0 - netmn_to_no3);
        newNO3 = *newminrl * netmn_to_no3;
      }

      if (debug) {
        printf("newNH4 = %6.4lf\n", newNH4);
        printf("newNO3 = %6.4lf\n", newNO3);
      }

      *ammonium += newNH4;

      /* Compute the amount of NH4 that is converted to NO3 due to */
      /* nitrification */

     nitrify(ammonium, &nh4_to_no3, maxt, nreduce,sitepar,layers,soil);
      *nit_amt = nh4_to_no3;

      if (debug) {
        printf("texture = %1d\n", *texture);
        printf("nh4_to_no3 = %6.4lf\n", nh4_to_no3);
        printf("maxt = %6.4lf\n", *maxt);
      }

      /* Compute fraction of new NO3 that is converted to N2O and NO */

      krainNO = nox_pulse(ppt, snow);

      getsoilprop(sand, silt, clay, &stdbulkd, &stdfieldc, texture,&soilclassification);

      /* Use standard field capacity and bulk density according */
      /* to the soil class in the texture triangle -mdh 10/26/99 */
/*      dDO = diffusiv(afiel(1), bulkd, *avgwfps) */
      /* No, change back to soils.in field capacity and bulk density. */
      /* -mdh 6/20/00 */
/*      dDO = diffusiv(&stdfieldc, &stdbulkd, avgwfps); */
      dDO = diffusiv(afiel, bulkd, avgwfps);

      newNO3 += nh4_to_no3;

      if (newNO3 > 1.0E-30) {
        *Nn2oflux = newNO3 * turnovfrac * sitepar->N2Oadjust;
        newNO3 -= *Nn2oflux; 

        /* Another update to NO flux calculation -mdh 10/26/99 */

/*        NO_N2O_ratio = 15.23 + (35.45*atan(0.676*PI*(10*dDO-1.86)))/PI; */
        NO_N2O_ratio = 8.0 + (18.0*atan(0.75*PI*(10*dDO-1.86)))/PI;
        /* If this is an agricultural system adjust the NO to N2O ratio */
        /* cak - 01/28/03 */
        if (*isagri) {
/*          NO_N2O_ratio *= 0.2; */
          NO_N2O_ratio *= 0.5;
        }
        potential_NOflux = NO_N2O_ratio * *Nn2oflux * krainNO;

        if (potential_NOflux <= newNO3) {
          *NOflux = potential_NOflux;
          newNO3 -= *NOflux;
        } else {
          /* take N out of ammonimum to get max NOflux possible */
          NH4_to_NO = min(*ammonium, (potential_NOflux-newNO3));
          *NOflux = newNO3 + NH4_to_NO;
          *ammonium -= NH4_to_NO;
          newNO3 = 0;
        }

        if (*NOflux < 1.0E-30) {
          *NOflux = 0.0;
        }

      } else {
        NO_N2O_ratio = 0.0;
      }

      /* Compute the N2O flux (Dn2oflux) and N2 flux (Dn2flux) due to */
      /* denitrification */

denitrify(newCO2, &newNO3, nitrate, wfluxout, critflow, frlechd,
                stream, basef, stormf, inorglch, Dn2oflux, Dn2flux,
                stdfieldc, stdbulkd, dN2lyr, dN2Olyr, jday,sitepar,layers);


      /* Now compute NOflux from denitrification (new calculation */
      /* -mdh 6/1/00 */
/*      potential_NOflux = NO_N2O_ratio * *Dn2oflux * krainNO; */
      /* For denitrification, krainNO is >= 1.0 -mdh 6/22/00 */

      potential_NOflux = NO_N2O_ratio * *Dn2oflux * min(1.0, krainNO);

      if (potential_NOflux <= *ammonium) {
        /* Take all N out of ammonimum pool */
        *NOflux += potential_NOflux;
        *ammonium -= potential_NOflux;
      } else {
        /* Take N out of available ammonium, then convert some Dn2oflux to */
        /* NOflux */
        *NOflux += *ammonium;
        potential_NOflux -= *ammonium;
        *ammonium = 0.0;
        if (potential_NOflux <= *Dn2oflux) {
          *NOflux += potential_NOflux;
          *Dn2oflux -= potential_NOflux;
        }
      }

      /* Compute the amount of the soil NO flux that is absorped by the canopy, cak - 09/23/03 */
      total_lai = *grass_lai + *tree_lai;
      if (total_lai > 0.0) {
        canopy_reduction = 0.0077 * pow(total_lai,2) + -0.13 * total_lai + 0.99;
        /* We need to retain the soil flux value */

        /* This previous code did not maintain and N balance -Melannie 9/26/2011 */
        /* NOsoil = *NOflux; */
        /* *NOflux *= canopy_reduction; */
        /* NOabsorp = NOsoil - *NOflux; */

        NOabsorp = *NOflux * (1 - canopy_reduction);
        if (NOabsorp > 0.0)
        {
            *NOabsorp_grass = NOabsorp * (*grass_lai / total_lai);
            *NOabsorp_tree = NOabsorp * (*tree_lai / total_lai);
            *NOflux -= NOabsorp;
        }

        /* NO absorped by canopy goes to crop storage and forest storage */
        /* Reset NOflux using the retained value */
        /* *NOflux = NOsoil; */
      }

      if (*NOflux < 1.0E-30) {
        *NOflux = 0.0;
      }
      if (*Nn2oflux < 1.0E-30) {
        *Nn2oflux = 0.0;
      }
      if (*Dn2oflux < 1.0E-30) {
        *Dn2oflux = 0.0;
      }
      if (*Dn2flux < 1.0E-30) {
        *Dn2flux = 0.0;
      }
      
      /* Calculate methane oxidation */
      methane_oxidation(CH4, isdecid, isagri,layers,soil);
  //    wrtsoiln(time, jday,ammonium,nitrate);
      return;
    }
