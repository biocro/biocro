
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      leachdly.c
**
**  FUNCTION:  void leachdly()
**
**  PURPOSE:   Daily nitrate leaching thru soil structure.
**
**  AUTHOR:    Melannie Hartman         2/97
**
**  INPUTS:
**    basef      - the fraction of N leached into baseflow
**    critflow   - critical water flow for leaching of minerls (cm H2O)
**    frlechd[]  - leaching fraction for each minerl, frlechd[0] for N
**    nitrate[]  - total nitrate in soil mineral pool, distributed among soil
**                 water model layers (gN/m2)
**    numlyrs    - total number of layers in the soil water model soil profile
**    stormf     - the fraction of N leached into stormflow
**                 (baseflow_N + stormflow_N = stream(2))
**    stream[]   - Century's stream output (stream flow & amount leached into
**                 stream flow)
**    wfluxout[] - the amount of water moving through the bottom of a soil
**                 layer (cm H2O) (positive is downward, negative is upward)
**
**  GLOBAL VARIABLES:
**    MAXLYR - maximum number of soil water model layers (21)
**
**  LOCAL VARIABLES:
**    amtlea - amount of nitrate-N leached by layer (gN/m2)
**    base_N - nitrate-N leached in base flow (gN/m2)
**    iel    - current element, set to 0 for nitrogen
**    linten - leaching intensity
**    lyr    - current layer in the soil profile
**    nxt    - next layer in soil profile, relative to current layer (lyr)
**    strm_N - nitrate-N leached in storm flow (gN/m2)
**
**  OUTPUTS:
**    inorglch  - inorganic N leaching (gN/m2)
**    nitrate[] - total nitrate in soil mineral pool, distributed among soil
**                water model layers (gN/m2)
**    stream[]  - Century's stream output (stream flow & amount leached into
**                stream flow)
**
**  CALLED BY:
**    denitrify()
**
**  CALLS:
**    None
**
***************************************************************************/

#include "swconst.h"

    void leachdly(double wfluxout[], int numlyrs, double nitrate[],
                  double critflow, double frlechd[], double stream[],
                  double basef, double stormf, double *inorglch)
    {

      int lyr, nxt;
      int iel = 0;  /* nitrogen */
      double amtlea[MAXLYR];
      double strm_N;
      double base_N;
      double linten;

      strm_N = 0.0;
      base_N = 0.0;
      *inorglch = 0.0;

      for(lyr=0; lyr < numlyrs; lyr++) {
        amtlea[lyr] = 0.0;
        nxt = lyr + 1;

        /* wfluxout > 0. indicates a saturated water flow out of layer lyr */

        if ((wfluxout[lyr] >  0.0) && (nitrate[lyr] > 1.0E-10)) {
          linten = min(1.0 - (critflow - wfluxout[lyr]) / critflow, 1.0);
          linten = max(linten, 0.0);
          amtlea[lyr] = (frlechd[iel]) * nitrate[lyr] * linten;

          /* If you are at the bottom layer, compute storm flow. */
          if (lyr == numlyrs-1) {
            strm_N = amtlea[lyr] * stormf;
          }
          if (amtlea[lyr] < nitrate[lyr]) {
            nitrate[lyr] = nitrate[lyr] - amtlea[lyr];
            nitrate[nxt] = nitrate[nxt] + (amtlea[lyr] - strm_N);
          }
        }
      }

      /* Compute base flow and mineral stream flows. */
      /* Check for underflow. -mdh */

      if (nitrate[nxt] >= 1.0E-10) {
        base_N = nitrate[nxt] * basef;
        nitrate[nxt] = nitrate[nxt] - base_N;
      }

      /* Note: stream flow indices differ from mineral element */ 
      /*       indices by 1 (eg  stream(2) is stream flow for nitrogen). */

      stream[iel+1] = stream[iel+1] + (strm_N + base_N);
      *inorglch = strm_N + base_N;

      return;
    }
