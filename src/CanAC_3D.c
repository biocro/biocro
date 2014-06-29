#include <stdio.h>
#include "CanA_3D_Structure.h"
#include "c4photo.h"
#include "AuxBioCro.h"
#include "BioCro.h"

struct Can_Str CanAC_3D (double canparms, double **canopy3Dstructure, int nrows, int ncols, double LAI,int DOY, int hr,double solarR,double Temp,
                        double RH,double WindSpeed,double lat,double Vmax,
                        double Alpha, double Kparm, double theta, double beta,
                        double Rd, double Catm, double b0, double b1,
                        double StomataWS, int ws,double kpLN, double upperT, 
                        double lowerT,double LeafN,struct nitroParms nitroP)
{
/**************************************************************************************************************
 * Purpose
 * This function CanAC_3D calculates net canopy CO2 assimilation, gross canopy CO2 assimilation, and canopy
 * transpiration based on 3D canopy model developed by Qingfeng Song (Functional Plant Biology 40 (2) 109-124)
 * 
 * Input Arguments
 * struct Can3Dparms canparms: This structure contains all the parameters (-----?), necessary to generate 3D 
 * description of the field as a function of canopy architecture and distance amongst different canopies or 
 * row spacing and row orientation
 * double LAI: Leaf area index, updated in main BioCro function afte partitioning of assimilated CO2
 * integer DOY: day of year
 * integer hr: hour
 * double solarR: total incoming solar radiation in PPFD
 * double Temp: Air temperature (centigrade)
 * double RH: Relative humidity at the top of the canopy
 * double WindSpeed: Wind Speed at the top of the canopy
 * double lat: Latitude
 * double Vmax: maximum value of Vmax as defined in Collatz (1992) model for C4 photosynthesis
 * double Alpha: Quantum Yield of Slope of A-Q curve as defined in Collatz (1992) C4 photosynthesis
 * double Kparm: Slope of ACi curce, as defined in Collatz(1992) C4 photosynthesis model
 * double theta: curvature parameter as defined in collatz (1992)
 * double beta: curvature parameter as defined in collatz (1992)
 * double Rd: dark respiration of leaf as defined in collatz (1992)
 * doubel Catm: ambient CO2 concentration 
 * double b0: intercept of bell-berry model
 * double b1: slope of bell-berry model
 * double upperT: upper temperature limitation on rate of photosynthesis
 * double lowerT: lower temperature limitation on rate of photosynthesis
 * struct nitroparms nitroP: structure containing parameters that govern relationship between leaf nitrogen 
 * and photosynthesis parameters
 * 
 * Output
 * 
 * struct Can_Str CanAC_3D
 * this structure have three elements:
 * [a]net canopy assimilation, [b]Gross canopy assimilation, and [c]canopy Transpiration
 * 
 * 
 * 
 **************************************************************************************************************/
 struct Can_Str ans;
 double Idir, Idiff,cosTh;
 int is_import_from_2DMatrix=1;
 int i;
 
 // follows are extension of the ray tracing area
  double   light_min_x= -110; 
	double   light_max_x= 110;
	double   light_min_y= -20;
	double   light_max_y= 20;
	double   light_min_z= 0;
	double   light_max_z= 300;
  char filename[] = "/home/djaiswal/Research/R-dev/biocro/inst/extdata/CM_SC.txt"; 
//update canopy if this is a new day otherwise use old canopy structure.
 if(hr==0)
   {
   update_3Dcanopy_structure(canopy3Dstructure,canparms,nrows, ncols);
   }
   update_3Dcanopy_structure(canopy3Dstructure,canparms,nrows, ncols);
   Idir=1000;
   Idiff=100;
   runFastTracer (is_import_from_2DMatrix,filename,canopy3Dstructure,  lat,  DOY,  hr,  Idir,  Idiff,  light_min_x,
   light_max_x,  light_min_y,  light_max_y,  light_min_z,  light_max_z);
  
  microclimate_for_3Dcanopy(canopy3Dstructure, nrows, ncols,LeafN,RH,WindSpeed,kpLN);
 /*
 for (i=0;i<NUMBER_OF_TRIANGLE;i++)
  {
    new=calculate_c4photosynthesis_and_Evapotranspiration(PPFD, microcclimate, photparms);
    netco2+=new;
    grossco2+=new;
    transp+==new;
  }
  update canopy structure using netco2,grossco2,and transp;
  */
  return(ans);                      
}
/*
void IdirIdiff()
{  
  return; 
}

void getmicroclimate_for_3Dcanopy()
{
  return;  
}

*/
/*
void run_ray_tracing(double **canopy3Dstructure)
{
  /** 
   * here I need to makre sure that I am able to call C++ function of Song and use it to update canopy3Dstructure 
   * /
   return;
}
*/
