#ifndef FUNCTION_PROTOTYPE_H
#define FUNCTION_PROTOTYPE_H

#include "AuxBioCro.h"
#include "crocent.h"
#include "soilwater.h"

void Copy_SoilWater_BioCro_To_CropCent(struct soilML_str *soilMLS, struct cropcentlayer *CROPCENT);
void CalculateBiogeochem(struct miscanthus *miscanthus, struct cropcentlayer *CROPCENT,struct dailyclimate *dailyclimate);
void Filling_BioCro_SoilStructure (struct soilML_str *soilMLS, struct soilText_str *soTexS, int soillayers,double *depths);
void Calculate_Soil_Layer_Temperature(double soiltavg[],int num_layers,struct dailyclimate *dailyclimate);  
void Copy_CropCent_To_DayCent_Structures(struct cropcentlayer *CROPCENT, SITEPAR_SPT sitepar,LAYERPAR_SPT layers,SOIL_SPT soil);
void CropGro_c(double lat,          /* Latitude    ~           1 */ 
      int *pt_doy,                 /* Day of the year   -        2 */
      int *pt_hr,                  /* Hour of the day     -      3 */
      double *pt_solar,               /* Solar Radiation    -       4 */
      double *pt_temp,                /* Temperature        -       5 */
      double *pt_rh,                  /* Relative humidity   -      6 */ 
	    double *pt_windspeed,           /* Wind Speed -               7 */ 
	    double *pt_precip,              /* Precipitation     -        8 */
	    double kd,                  /* K D (ext coeff diff) -     9 */
	    double * chilhf,              /* NEED TO REVERT TO DOUBLE * Chi and Height factor  -  10 */
	    int nlayers,             /* # Lay canop   -           11 */
	    double Rhizome,             /* Ini Rhiz        -         12 */
      double propLeaf,                /* i rhiz to leaf    -       13 */
      double * sencoefs,            /* sene coefs   !!!!!!!MADE NEW VARIABLE ADJUST IN OTHER FILE    -        14 */
      int timestep,            /* time step       -         15 */
      int vecsize,             /* vector size        -      16 */
	    double Sp,              /* Spec Leaf Area     -      17 */
	   // SEXP SPD, UNUSED VARIABLE OMITTED FROM ARGS                 /* Spec Lefa Area Dec  -     18 */
	    double * dbpcoefs,            /* Dry Bio Coefs      -      19 */
	    double * thermalp,            /* Themal Periods   -        20 */
	    double vmax1,                /* Vmax of photo     -       21 */
	    double alpha1,               /* Quantum yield   -         22 */
	    double kparm1,               /* k parameter (photo) -     23 */
	    double theta,               /* theta param (photo)   -   24 */
	    double beta,                /* beta param  (photo) -     25 */
	    double Rd1,                  /* Dark Resp   (photo) -     26 */
	    double Ca,                /* CO2 atmosph       -       27 */
	    double b01,                  /* Int (Ball-Berry)  -       28 */
	    double b11,                  /* Slope (Ball-Berry)  -     29 */
	    int ws,                  /* Water stress flag   -     30 */
	    double * soilcoefs,           /* Soil Coefficients !!!!! MADE NEW VARIABLE ADJUST IN OTHER FILE  -     31 */
	    double LeafN_0,              /* Ini Leaf Nitrogen -       32 */
	    double kLN,                 /* Decline in Leaf Nitr -    33 */
	   // SEXP VMAXB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on Vmax   -   34 */
	   // SEXP ALPHAB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on alpha   -  35 */
	    double * mresp,               /* Maintenance resp !!!!!CHANGED VAR ADJUST IN OTHER FILE    -    36 */
	    int soilType,            /* Soil type       -         37 */
	    int wsFun,               /* Water Stress Func  -      38 */
	    double * centcoefs,           /* Century coefficients !!!!!!!!! MADE NeW VARiABLE ADJUST IN OTHER FILE  -  39 */
      int centTimestep,        /* Century timestep   -      40 */ 
	   // SEXP CENTKS, UNUSED VAR OMMITTED FROM ARGS             /* Century decomp rates-     41 */
	    int soillayers,          /* # soil layers        -    42 */
	    double * soildepths,          /* Soil Depths      -        43 */
	    double * cwsVec,                 /* Current water status !!!!! CHANGED var ADJST IN NEW  -   44 */
	    int hydrdist,            /* Hydraulic dist flag   -   45 */
	    double * secs,                /* Soil empirical coefs !!!MADE NEW VAR CHANGE IN OTHER  -   46 */
	    double kpLN,                /* Leaf N decay         -    47 */
	    double lnb0,                /* Leaf N Int        -       48 */
	    double lnb1,                /* Leaf N slope         -    49 */
      int lnfun,               /* Leaf N func flag      -   50 */
      double upperT,           /* Temperature Limitations photoParms- */
	    double lowerT,         //-
	    double * nnitrop, //-        MADE NEW VAR CHANGE IN OTHER
      double * sompoolsfromr, // I don't know if I should change this to double * Let me know
      double * soiltexturefromr,// MADE NEW VAR CHANGE IN OTHER)    
      double * LAId_c, /********Start of output vars******/
      double * RhizomeLitterD,  
      double * RootLitterD,  
      double * LeadLitterD,
      double * StemLitterD,
      double * RhizomeD,
      double * RootD,
      double * StemD,
      double * LeafD,
      double * leafDarkResp,
      double * rhizomeMResp,
      double * rootMResp,
      double * stemMResp,
      double * AutoResp,
      double * npp,
      double * gpp,
      int * dayafterplanting,
     
      double * leafpsimvec,
      double * soilevaporation,
      double * minnitrovec,
      double * spvec,
      double * alphavec,
      double * vmaxvec,
      double * leafreductioncoefs,
      double * stomatalcondcoefs,
      double * abovelitter,
      double * belowlitter,
      double * respvec,
      double * soilwatcont,
      double * leafnitrogen,
      double * GDD_c,
      double * TTTc_c,
      double * LAIc_c,
      double * grainy,
      double * leafy,
      double * stemy, 
      double * rooty, 
      double * rhizomey,
      double * canopytrans,
      double * canopyassim,
      double * dayofyear,
      double * hour,
      double ** cwsmatrix,
      double ** psimmatrix,
      double ** rdmatrix,
      double ** waterfluxmatrix,
     
      double * snPools,
      double * scPools);
      
#endif
