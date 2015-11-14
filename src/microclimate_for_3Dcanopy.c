#include <stdio.h>
#include <math.h>
#include "CanA_3D_Structure.h"

void microclimate_for_3Dcanopy(double **canopy3Dstructure, double *canHeight,int nrows, int ncols, double LeafN_canopytop,double RH_canopytop,double windspeed_canopytop, double kpLN)
{
     /********************************************************************
      * Purpose
      * Purpose of this function is to calculate microclimate of the 3D
      * space in which canopy processes are simulated.
      * Currently we will simple simulate leafN, wind speed, and RH as a 
      * function of cumulative leaf area. This is same strategy adopted 
      * in the sunlit-shaded model.
      * Ideally we will like to have a detailed description of leafN, which
      * is not only a function of cumulative LAI but also leaf position
      * Co2 concentration profile, Relative humidity, and Wind speed Profile 
      * can be calculated from a FEM/FVM based numerical
      * solution scheme. 
      * Currently Qingfensong uses COMSOL commerical package to calculate 
      * canopy microclimate. Accoridng to him, photosynthesis is not very 
      * sensitive to canopy microclimate so I am using simple calculations
      * from older version of BioCro but It could play an important role in 
      * simulating energy balance
      * *****************************************************************/
  int i;
  double kwind,kRH,canopyheight;
  
  kwind=0.7;
  kRH=1.0-RH_canopytop;
  canopyheight=0.0;
  
   for (i=0;i<nrows;i++)
   {
     // calculate average height of each element in the 22nd column [index=20]
     canopy3Dstructure[i][21]=(canopy3Dstructure[i][2]+canopy3Dstructure[i][5]+canopy3Dstructure[i][8])*0.33;
     canopyheight = (canopyheight > canopy3Dstructure[i][21])?canopyheight:canopy3Dstructure[i][21];
   }
   *canHeight=canopyheight;
  for (i=0;i<nrows;i++)
   {
     //Updating matrix column for leaf nitrogen concentration based on cumLAI
     canopy3Dstructure[i][17]=LeafN_canopytop*exp(-kpLN*canopy3Dstructure[i][19]);     
     // updating matrix columns for relative humidity
     canopy3Dstructure[i][22]=RH_canopytop*exp(kRH*canopy3Dstructure[i][21]/canopyheight);
     canopy3Dstructure[i][22]=(canopy3Dstructure[i][22]>1.0)?0.99:canopy3Dstructure[i][22];     
     //Updating matrix column for windspeed
     canopy3Dstructure[i][23]=windspeed_canopytop*exp(-kwind*canopy3Dstructure[i][19]);     
   }     
   return;   
}
