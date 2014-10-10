
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      wfps.c
**
**  FUNCTION:  void wfps()
**
**  PURPOSE:   Compute the daily water-filled pore space by layer. 
**
**  AUTHOR:    Melannie Hartman  6/93
** 
**  INPUTS:
**    layers          - soil water soil layer structure
**    layers->bulkd[] - bulk density by layer (g/cm3)
**                      (weight per unit volume - soil solids and pores)
**    layers->numlyrs - total number of layers in the soil water model soil
**                      profile
**    layers->swc[]   - soil water content by layer (cm H2O)
**    layers->width[] - the thickness of soil water model layers (cm)
**
**  GLOBAL VARIABLES:
**    PARTDENS - particle density (g/cm3) (2.65)
**
**  LOCAL VARIABLES:
**    ilyr      - current layer in the soil profile
**    porespace - the total porespace of the current soil layer (fraction)
**                = 1.0 - (BulkDensity/ParticleDensity);
**    swcfrac   - fraction of a layer depth that is filled with water.
**
**  OUTPUTS:
**     layers->wfps[] - water-filled pore space by layer (fraction 0.0-1.0) 
**                      (fraction of a porespace that is filled with water)
**
**  CALLED BY:
**     calcdefac()
**     denitrify()
**     nitrify()
**     watrflow()
**
**  CALLS:
**    None
**
*****************************************************************************/

#include "soilwater.h"

    void wfps(LAYERPAR_SPT layers)
    {
      int ilyr;
      double swcfrac;
      double porespace;

      for(ilyr=0; ilyr < layers->numlyrs; ilyr++) {

        swcfrac = layers->swc[ilyr] / layers->width[ilyr];
        porespace = 1.0 - layers->bulkd[ilyr] / PARTDENS;
        layers->wfps[ilyr] = swcfrac/porespace;
      }

      return;
    }
