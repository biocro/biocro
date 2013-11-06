
/*              Copyright 1993 Colorado State University                    */
/*                      All Rights Reserved                                 */

/*****************************************************************************
**
**  FILE:      getsoilprop.c
**
**  FUNCTION:  void getsoilprop()
**
**  PURPOSE:   Determine soil texture class based on percent of sand, silt,
**             clay.
**
**             U.S. Department of Agriculture soil textural classes
**
**            1 -- sand            2 -- loamy sand     3 -- sandy loam
**            4 -- silt loam       5 -- loam           6 -- sandy clay loam
**            7 -- silty clay loam 8 -- clay loam      9 -- sandy clay
**           10 -- silty clay     11 -- clay          12 -- peat or silt
**           13 -- volcanic
**
**  INPUTS:
**    aclay   - the fraction of clay in soil, 0.0 - 1.0
**    asand   - the fraction of sand in soil, 0.0 - 1.0
**    asilt   - the fraction of silt in soil, 0.0 - 1.0
**    texture - a constant to designate coarses/fineness of the soil
**              (i.e. COARSE, MEDIUM, FINE, VERYFINE - see n2o_model.h)
**
**  GLOBAL VARIABLES:
**    None
**
**  LOCAL VARIABLES:
**    bulk_density[]   - bulk density for each soil texture class
**    clay             - the portion of the soil made up of clay (0..100)
**    COARSE           - texture type for sandy soil
**    field_capacity[] - volumetric water content at field capacity for each
**                       soil texture class (cm H2O/cm of soil)
**    FINE             - texture type for fine soil
**    ksat             - saturation soil hydraulic 'conductivity' for soil
**                       texture class converted from meters/second to
**                       centimeters/second
**    MEDIUM           - texture type for medium (loamy) soil
**    NCLASS           - maximum number of soil texture classes
**    sand             - the portion of the soil made up of sand (0..100)
**    satcond[]        - saturation soil hydraulic `conductivity' for each
**                       soil texture class (meters/second)
**    silt             - the portion of the soil made up of silt (0..100)
**    soil_indx        - index of current soil type
**    VERYFINE         - texture type for volcanic soil
**
**  OUTPUTS:
**    bulkden  - bulk density for soil texture class (g/cm3)
**    fieldcap - volumetric water content at field capacity for soil texture
**               class (cm H2O/cm of soil)
**
**  CALLED BY:
**    dailymoist()
**
**  CALLS:
**    None
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define NCLASS 14
#define COARSE 1                /* texture type for sandy soil */
#define MEDIUM 2                /* texture type for medium (loamy) soil */
#define FINE 3                  /* texture type for fine soil */
#define VERYFINE 4              /* texture type for volcanic soil */

    void getsoilprop(double *asand, double *asilt, double *aclay, double *bulkden,
                     double *fieldcap, int *texture)
    {
      static double satcond[NCLASS] = {0.0, 0.000176, 0.0001563,
                                      0.00003467, 0.0000072, 0.00000695,
                                      0.0000063, 0.0000017, 0.00000245,
                                      0.000002167, 0.000001033,
                                      0.000001283, 0.0000080, 0.0000080};
      static double bulk_density[NCLASS] = {0.0, 1.55, 1.50, 1.43, 1.36,
                                           1.39, 1.35, 1.24, 1.30, 1.28,
                                           1.19, 1.15, 1.40, 0.75};
      static double field_capacity[NCLASS] = {0.0, 0.14, 0.18, 0.22, 0.32,
                                             0.29, 0.28, 0.40, 0.36,
                                             0.36, 0.46, 0.45, 0.33,
                                             0.70};
      int   soil_indx;
      double ksat;
      double sand, silt, clay;

      sand = *asand * 100;
      silt = *asilt * 100;
      clay = *aclay * 100;

      if ((sand > 100 || sand < 0) || (silt > 100 || silt < 0) ||
          (clay > 100 || clay < 0)) {
        fprintf(stderr, "Invalid texture in getsoilprop: sand = %f,", sand);
        fprintf(stderr, " silt = %f, clay = %f\n", silt, clay);
        exit(1);
      }

      if (*texture == VERYFINE) {
        /* volcanic */
        soil_indx = 13;
      } else if ((sand >= 85) && ((-2.0/3.0)*silt - clay + 10 >= 0)) {
        /* sand */
        soil_indx = 1;
      } else if (((-2.0/3.0)*silt - clay + 10 <= 0) &&
                 (-0.5*silt - clay + 15 >= 0)) {
        /* loamy sand */
        soil_indx = 2;
      } else if (((-0.5*silt-clay+15 <= 0) && (clay <= 20) && (sand >= 52)) ||
                 ((sand <= 52) && (silt <= 50) && (clay <= 7))) {
        /* sandy loam */
        soil_indx = 3;
      } else if ((sand <= 50) && (silt >= 50) && (clay <= 27)) {
        if ((sand <= 20) && (clay <= 10)) {
          /* silt */
          soil_indx = 12;
        } else {
          /* silt loam */
          soil_indx = 4;
        }
      } else if ((sand >= 23 && sand <= 52) && (silt >= 28 && silt <= 50) &&
                 (clay >= 7 && clay <= 27)) {
        /* loam */
        soil_indx = 5;
      } else if ((sand >= 45 && sand <= 80) && (silt <= 28) &&
                 (clay >= 20 && clay <= 35)) {
        /* sandy clay loam */
        soil_indx = 6;
      } else if ((sand <= 20) && (clay >= 27 && clay <= 40)) {
        /* silty clay loam */
        soil_indx = 7;
      } else if ((sand >= 20 && sand <= 45) && (clay >= 27 && clay <= 40)) {
        /* clay loam */
        soil_indx = 8;
      } else if ((sand >= 45 && sand <= 65) && (silt <= 20) &&
                 (clay >= 35 && clay <= 55)) {
        /* sandy clay */
        soil_indx = 9;
      } else if ((sand <= 20) && (silt >= 40 && silt <= 60) &&
                 (clay >= 40 && clay <= 60)) {
        /* silty clay */
        soil_indx = 10;
      } else if ((sand <= 45) && (silt <= 40) && (clay >= 40)) {
        /* clay */
        soil_indx = 11;
      } else {
        fprintf(stderr, "Unknown texture type: sand = %f, silt = %f,", sand,
                silt);
        fprintf(stderr, " clay = %f\n", clay);
        exit(1);
      }

      *bulkden = bulk_density[soil_indx];
      *fieldcap = field_capacity[soil_indx];
      ksat = satcond[soil_indx]*100.0;    /* Convert m/sec to cm/sec */

/*      printf("Soil classification = %1d\n", soil_indx);
      printf("standard_bulkden = %10.6lf\n", *bulkden);
      printf("standard_fieldcap = %10.6lf\n", *fieldcap);
      printf("standard_ksat = %10.6lf\n", ksat); */

      return;
    }
