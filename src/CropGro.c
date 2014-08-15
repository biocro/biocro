
/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2011
 *
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxcaneGro.h"
#include "crocent.h"
#include "soilwater.h"
#include "function_prototype.h"
#include "n2o_model.h"



SEXP CropGro(SEXP LAT,          /* Latitude    ~           1 */ 
      SEXP DOY,                 /* Day of the year   -        2 */
      SEXP HR,                  /* Hour of the day     -      3 */
	    SEXP SOLAR,               /* Solar Radiation    -       4 */
	    SEXP TEMP,                /* Temperature        -       5 */
	    SEXP RH,                  /* Relative humidity   -      6 */ 
	    SEXP WINDSPEED,           /* Wind Speed -               7 */ 
	    SEXP PRECIP,              /* Precipitation     -        8 */
	    SEXP KD,                  /* K D (ext coeff diff) -     9 */
	    SEXP CHILHF,              /* Chi and Height factor  -  10 */
	    SEXP NLAYERS,             /* # Lay canop   -           11 */
	    SEXP RHIZOME,             /* Ini Rhiz        -         12 */
      SEXP IRTL,                /* i rhiz to leaf    -       13 */
      SEXP SENCOEFS,            /* sene coefs       -        14 */
      SEXP TIMESTEP,            /* time step       -         15 */
      SEXP VECSIZE,             /* vector size        -      16 */
	    SEXP SPLEAF,              /* Spec Leaf Area     -      17 */
	    SEXP SPD,                 /* Spec Lefa Area Dec  -     18 */
	    SEXP DBPCOEFS,            /* Dry Bio Coefs      -      19 */
	    SEXP THERMALP,            /* Themal Periods   -        20 */
	    SEXP VMAX,                /* Vmax of photo     -       21 */
	    SEXP ALPHA,               /* Quantum yield   -         22 */
	    SEXP KPARM,               /* k parameter (photo) -     23 */
	    SEXP THETA,               /* theta param (photo)   -   24 */
	    SEXP BETA,                /* beta param  (photo) -     25 */
	    SEXP RD,                  /* Dark Resp   (photo) -     26 */
	    SEXP CATM,                /* CO2 atmosph       -       27 */
	    SEXP B0,                  /* Int (Ball-Berry)  -       28 */
	    SEXP B1,                  /* Slope (Ball-Berry)  -     29 */
	    SEXP WS,                  /* Water stress flag   -     30 */
	    SEXP SOILCOEFS,           /* Soil Coefficients   -     31 */
	    SEXP ILEAFN,              /* Ini Leaf Nitrogen -       32 */
	    SEXP KLN,                 /* Decline in Leaf Nitr -    33 */
	    SEXP VMAXB1,              /* Effect of N on Vmax   -   34 */
	    SEXP ALPHAB1,             /* Effect of N on alpha   -  35 */
	    SEXP MRESP,               /* Maintenance resp     -    36 */
	    SEXP SOILTYPE,            /* Soil type       -         37 */
	    SEXP WSFUN,               /* Water Stress Func  -      38 */
	    SEXP CENTCOEFS,           /* Century coefficients   -  39 */
      SEXP CENTTIMESTEP,        /* Century timestep   -      40 */ 
	    SEXP CENTKS,              /* Century decomp rates-     41 */
	    SEXP SOILLAYERS,          /* # soil layers        -    42 */
	    SEXP SOILDEPTHS,          /* Soil Depths      -        43 */
	    SEXP CWS,                 /* Current water status  -   44 */
	    SEXP HYDRDIST,            /* Hydraulic dist flag   -   45 */
	    SEXP SECS,                /* Soil empirical coefs  -   46 */
	    SEXP KPLN,                /* Leaf N decay         -    47 */
	    SEXP LNB0,                /* Leaf N Int        -       48 */
	    SEXP LNB1,                /* Leaf N slope         -    49 */
      SEXP LNFUN,               /* Leaf N func flag      -   50 */
      SEXP UPPERTEMP,           /* Temperature Limitations photoParms- */
	    SEXP LOWERTEMP,         //-
	    SEXP NNITROP, //-
      SEXP SOMPOOLSfromR, // I don't know if I should change this to double * Let me know
      SEXP SOILTEXTUREfromR)           
{
    
  /*** SEXP VARIABLES***/
  
    
    SEXP lists, names;
    SEXP DayofYear;//Y
    SEXP Hour;//Y
  	SEXP CanopyAssim;//Y
  	SEXP CanopyTrans;//Y
  	SEXP Leafy;//Y
  	SEXP Stemy;//Y
  	SEXP Rooty;//Y
  	SEXP Rhizomey;//Y
  	SEXP Grainy;//Y
  	SEXP LAIc;//Y
  	SEXP TTTc;//Y
  	SEXP SoilWatCont;//Y
  	SEXP StomatalCondCoefs;//y
  	SEXP LeafReductionCoefs;//y
  	SEXP LeafNitrogen;//Y
  	SEXP AboveLitter;//y
  	SEXP BelowLitter;//y
  	SEXP VmaxVec;//Y
  	SEXP AlphaVec;//Y
  	SEXP SpVec;//Y
  	SEXP MinNitroVec;//Y
  	SEXP RespVec;//Y
  	SEXP SoilEvaporation;//Y
  	SEXP cwsMat; //Y
  	SEXP psimMat; /* Holds the soil water potential */ //Y
  	SEXP rdMat; //Y
    SEXP waterfluxMat; /* holds water flux in m3/m2 hr */ //Y
  	SEXP SCpools;//Y
  	SEXP SNpools;//Y
  	SEXP LeafPsimVec; //Y
  // From here, we have daily (instead of hourly) output vectors
    SEXP DayafterPlanting;//Y
    SEXP GDD; // daily vector of growing degree day //Y
    SEXP GPP; // Gross Primary Productivity // Y
    SEXP NPP; // Net Primary Productivity // Y
    SEXP autoRESP; // Autotrophic Respiration // Y
    SEXP hetRESP; // Heterotrophic Respiration
    SEXP NER; // Net Ecosystem Respiration UNUSED VARIABLE
    SEXP StemMResp;//Y
    SEXP RootMResp;//Y
    SEXP RhizomeMResp;//Y
    SEXP LeafDarkResp;//Y
    SEXP Stemd;//Y
    SEXP Leafd;//Y
    SEXP Rootd;//Y
    SEXP Rhizomed;//Y
    SEXP Stemlitterd;//Y
    SEXP Leaflitterd;// Y
    SEXP Rootlitterd; //Y
    SEXP Rhizomelitterd; //Y
    SEXP LAId; //Y
    SEXP totalSOC;
    SEXP strucc1;
    SEXP strucc2;
    SEXP metabc1;
    SEXP metabc2;
    SEXP som1c1;         // UNUSED OUTPUT VARIABLES, COMMENTED OUT
    SEXP som1c2;
    SEXP som2c1;
    SEXP som2c2;
    SEXP som3c;
    SEXP minN;
    
    
    
    
    int i, i2, i3;
    int dailyvecsize;   
   /**********************START OF LOCAL INPUT VARIABLES***********************/
    double lat;
    int *pt_doy;
    int *pt_hr;
    double *pt_solar;
    double *pt_temp;
    double *pt_rh;
    double *pt_windspeed;
    double *pt_precip;
    double kd;
    double * chilhf;
    int nlayers;
    double Rhizome;
    double propLeaf;
    double * sencoefs;
    int timestep;
    int vecsize;
    double Sp;
    double * dbpcoefs;
    double * thermalp;
    double vmax1;
    double alpha1;
  	double kparm1;
    double theta;
    double beta;
  	double Rd1, Ca;
    double b01, b11;
    int ws;
    double * soilcoefs;
    double LeafN_0;
    double kLN;
    double * mresp;
    int soilType, wsFun;
    double * centcoefs;
    int centTimestep;
    int soillayers;
    double * soildepths;
    double * cwsVec;
    int hydrdist;
    double * secs;
    double kpLN, lnb0, lnb1;
    int lnfun;
    double upperT;
    double lowerT;
    double * nnitrop;
    double * sompoolsfromr;
    double * soiltexturefromr;
      /**********************END OF LOCAL INPUT VARIABLES************************/
   
   /**************************START OF LOCAL OUTPUT VARIABLES********************/
   double * LAId_c;
   double * RhizomeLitterD;  
   double * RootLitterD;  
   double * LeafLitterD;
   double * StemLitterD;
   double * RhizomeD;
   double * RootD;
   double * StemD;
   double * LeafD;
   double * leafDarkResp;
   double * rhizomeMResp;
   double * rootMResp;
   double * stemMResp;
   double * AutoResp;
   double * npp;
   double * gpp;
   int * dayafterplanting;
   
   double * leafpsimvec;
   double * soilevaporation;
   double * minnitrovec;
   double * spvec;
   double * alphavec;
   double * vmaxvec;
   double * leafreductioncoefs;
   double * stomatalcondcoefs;
   double * abovelitter;
   double * belowlitter;
   double * respvec;
   double * soilwatcont;
   double * leafnitrogen;
   double *GDD_c;
   double * TTTc_c;
   double * LAIc_c;
   double * grainy;
   double * leafy;
   double * stemy; 
   double * rooty; 
   double * rhizomey;
   double * canopytrans;
   double * canopyassim;
   double * dayofyear;
   double * hour;
   double ** cwsmatrix, ** psimmatrix,** rdmatrix, ** waterfluxmatrix;
   
   double * snPools;
   double * scPools;
   /**************************END OF LOCAL OUTPUT VARIABLES**********************/
      

/****************START OF INPUT CASTING**************************/   
   
   vecsize = INTEGER(VECSIZE)[0];
   dailyvecsize = vecsize/24;//locally needed
   
      
   sompoolsfromr = malloc(vecsize*sizeof(sompoolsfromr));
   sompoolsfromr = &REAL(SOMPOOLSfromR)[0];
   upperT=REAL(UPPERTEMP)[0];
   lowerT=REAL(LOWERTEMP)[0];
   soillayers = INTEGER(SOILLAYERS)[0];
          /* Effect of Nitrogen */
  kpLN = REAL(KPLN)[0];
	lnb0 = REAL(LNB0)[0]; 
	lnb1 = REAL(LNB1)[0];
	lnfun = INTEGER(LNFUN)[0];
  centTimestep = INTEGER(CENTTIMESTEP)[0];
    
  vmax1 = REAL(VMAX)[0];
	alpha1 = REAL(ALPHA)[0];
	kparm1 = REAL(KPARM)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];
	Rd1 = REAL(RD)[0];
	Ca = REAL(CATM)[0];
	b01 = REAL(B0)[0];
	b11 = REAL(B1)[0];

  LeafN_0 = REAL(ILEAFN)[0];
  timestep = INTEGER(TIMESTEP)[0];

	Rhizome = REAL(RHIZOME)[0];
	Sp = REAL(SPLEAF)[0]; 
  kLN = REAL(KLN)[0];
  wsFun = INTEGER(WSFUN)[0];
  soilType = INTEGER(SOILTYPE)[0];
  // Modify SOil Type Based on Texture Information entered
 
  if(soilType<0 || soilType > 10)
  { 
    soilType=1;
  } // This is a temporary fix to avoid unknown SoilType 
  
  cwsVec = malloc(soillayers*sizeof(cwsVec));
    for(i2=0;i2<soillayers;i2++){
  	  cwsVec[i2] = REAL(CWS)[i2];
	  }
    
    soiltexturefromr = malloc(3*sizeof(soiltexturefromr));
    soiltexturefromr = REAL(SOILTEXTUREfromR);
    
    nnitrop = malloc(15*sizeof(nnitrop));
    nnitrop = REAL(NNITROP);
    
    secs = malloc(3*sizeof(secs));
    secs = REAL(SECS);
    
    centcoefs = malloc(24*sizeof(centcoefs));
    centcoefs = REAL(CENTCOEFS);

    mresp = malloc(2*sizeof(mresp));
    mresp = REAL(MRESP);

    sencoefs = malloc(4*sizeof(sencoefs));
    sencoefs = REAL(SENCOEFS);
    
    soilcoefs = malloc(9*sizeof(soilcoefs));
    soilcoefs = REAL(SOILCOEFS);

    chilhf = malloc(2*sizeof(chilhf));
    chilhf = REAL(CHILHF);
	  
    
    pt_doy = malloc(vecsize*sizeof(pt_doy));
    pt_doy=INTEGER(DOY);
  
// 	int *pt_hr = INTEGER(HR);
    
    pt_hr = malloc(vecsize*sizeof(pt_hr));
    pt_hr=INTEGER(HR);
    
//	double *pt_solar = REAL(SOLAR);
   
    pt_solar = malloc(vecsize*sizeof(pt_solar));
    pt_solar=REAL(SOLAR);
  
//	double *pt_temp = REAL(TEMP);
    
    pt_temp = malloc(vecsize*sizeof(pt_temp));
    pt_temp=REAL(TEMP);
    
//	double *pt_rh = REAL(RH);
    
    pt_rh = malloc(vecsize*sizeof(pt_rh));
    pt_rh=REAL(RH);
//	double *pt_windspeed = REAL(WINDSPEED);
  
    pt_windspeed = malloc(vecsize*sizeof(pt_windspeed));
    pt_windspeed=REAL(WINDSPEED);
//	double *pt_precip = REAL(PRECIP);
   
    pt_precip = malloc(vecsize*sizeof(pt_precip));
    pt_precip=REAL(PRECIP);
    
    
    dbpcoefs = malloc(vecsize*sizeof(dbpcoefs));
    dbpcoefs = REAL(DBPCOEFS);
    
    
    thermalp = malloc(vecsize*sizeof(thermalp));
    thermalp = REAL(THERMALP);
    
    
    soildepths = malloc(vecsize*sizeof(soildepths));
    soildepths = REAL(SOILDEPTHS);
    
    lat = REAL(LAT)[0];
    nlayers = INTEGER(NLAYERS)[0];
  	ws = INTEGER(WS)[0];
  	kd = REAL(KD)[0];
    

    hydrdist = INTEGER(HYDRDIST)[0];
    propLeaf = REAL(IRTL)[0]; 
   
   /*********************END***********************/

//	vecsize = length(DOY);
	PROTECT(lists = allocVector(VECSXP,61));
	PROTECT(names = allocVector(STRSXP,61));

	PROTECT(DayofYear = allocVector(REALSXP,vecsize));
	PROTECT(Hour = allocVector(REALSXP,vecsize));
	PROTECT(CanopyAssim = allocVector(REALSXP,vecsize));
	PROTECT(CanopyTrans = allocVector(REALSXP,vecsize));
	PROTECT(Leafy = allocVector(REALSXP,vecsize));
	PROTECT(Stemy = allocVector(REALSXP,vecsize));
	PROTECT(Rooty = allocVector(REALSXP,vecsize));
	PROTECT(Rhizomey = allocVector(REALSXP,vecsize));
	PROTECT(Grainy = allocVector(REALSXP,vecsize));
	PROTECT(LAIc = allocVector(REALSXP,vecsize));
	PROTECT(TTTc = allocVector(REALSXP,vecsize));
	PROTECT(SoilWatCont = allocVector(REALSXP,vecsize));
	PROTECT(StomatalCondCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafReductionCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafNitrogen = allocVector(REALSXP,vecsize));
	PROTECT(AboveLitter = allocVector(REALSXP,vecsize));
	PROTECT(BelowLitter = allocVector(REALSXP,vecsize));
	PROTECT(VmaxVec = allocVector(REALSXP,vecsize));
	PROTECT(AlphaVec = allocVector(REALSXP,vecsize));
	PROTECT(SpVec = allocVector(REALSXP,vecsize));
	PROTECT(MinNitroVec = allocVector(REALSXP,vecsize));
	PROTECT(RespVec = allocVector(REALSXP,vecsize));
	PROTECT(SoilEvaporation = allocVector(REALSXP,vecsize));
	PROTECT(cwsMat = allocMatrix(REALSXP,soillayers,vecsize));
  PROTECT(waterfluxMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(psimMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(rdMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(SCpools = allocVector(REALSXP,9));
	PROTECT(SNpools = allocVector(REALSXP,9));
	PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));
  PROTECT(DayafterPlanting = allocVector(REALSXP,dailyvecsize));
  PROTECT(GDD = allocVector(REALSXP,dailyvecsize));
  PROTECT(GPP = allocVector(REALSXP,dailyvecsize));
  PROTECT(NPP = allocVector(REALSXP,dailyvecsize));
  PROTECT(autoRESP = allocVector(REALSXP,dailyvecsize));
  PROTECT(hetRESP = allocVector(REALSXP,dailyvecsize));
  PROTECT(NER = allocVector(REALSXP,dailyvecsize));
  PROTECT(StemMResp= allocVector(REALSXP,dailyvecsize));
  PROTECT(RootMResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(RhizomeMResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(LeafDarkResp = allocVector(REALSXP,dailyvecsize));
  PROTECT(Stemd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Leafd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rootd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rhizomed = allocVector(REALSXP,dailyvecsize));
  PROTECT(Stemlitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Leaflitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rootlitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(Rhizomelitterd = allocVector(REALSXP,dailyvecsize));
  PROTECT(LAId = allocVector(REALSXP,dailyvecsize));
  PROTECT(totalSOC = allocVector(REALSXP,dailyvecsize));
  PROTECT(strucc1 = allocVector(REALSXP,dailyvecsize));
   PROTECT(strucc2 = allocVector(REALSXP,dailyvecsize));
  PROTECT(metabc1 = allocVector(REALSXP,dailyvecsize));
  PROTECT(metabc2 = allocVector(REALSXP,dailyvecsize));
   PROTECT(som1c1 = allocVector(REALSXP,dailyvecsize));
  PROTECT(som1c2 = allocVector(REALSXP,dailyvecsize));
  PROTECT(som2c1 = allocVector(REALSXP,dailyvecsize));
   PROTECT(som2c2 = allocVector(REALSXP,dailyvecsize));
  PROTECT(som3c = allocVector(REALSXP,dailyvecsize));
  PROTECT(minN = allocVector(REALSXP,dailyvecsize));

    
 /****************** START OF OUTPUT ALLOCATION******************/


   
    snPools = malloc(9*sizeof(snPools));
    scPools = malloc(9*sizeof(scPools));
    
    
    GDD_c = malloc(dailyvecsize*sizeof(GDD_c));
    LAId_c = malloc(dailyvecsize*sizeof(LAId_c));
    RhizomeLitterD = malloc(dailyvecsize*sizeof(RhizomeLitterD));
    RootLitterD = malloc(dailyvecsize*sizeof(RootLitterD));
    LeafLitterD = malloc(dailyvecsize*sizeof(LeafLitterD));
    StemLitterD = malloc(dailyvecsize*sizeof(StemLitterD));
    RhizomeD = malloc(dailyvecsize*sizeof(RhizomeD));
    RootD = malloc(dailyvecsize*sizeof(RootD));
    StemD = malloc(dailyvecsize*sizeof(StemD));
    LeafD = malloc(dailyvecsize*sizeof(LeafD));
    leafDarkResp = malloc(dailyvecsize*sizeof(leafDarkResp));
    rhizomeMResp = malloc(dailyvecsize*sizeof(rhizomeMResp));
    rootMResp = malloc(dailyvecsize*sizeof(rootMResp));
    stemMResp = malloc(dailyvecsize*sizeof(stemMResp));
    AutoResp = malloc(dailyvecsize*sizeof(AutoResp));
    npp = malloc(dailyvecsize*sizeof(npp));
    gpp = malloc(dailyvecsize*sizeof(gpp));
    dayafterplanting = malloc(dailyvecsize*sizeof(dayafterplanting));
    
    leafpsimvec = malloc(vecsize*sizeof(leafpsimvec));
    soilevaporation = malloc(vecsize*sizeof(soilevaporation));
    respvec = malloc(vecsize*sizeof(respvec));
    minnitrovec = malloc(vecsize*sizeof(minnitrovec));
    spvec = malloc(vecsize*sizeof(spvec));
    vmaxvec = malloc(vecsize*sizeof(vmaxvec));
    alphavec = malloc(vecsize*sizeof(alphavec));
    abovelitter = malloc(vecsize*sizeof(abovelitter));
    belowlitter = malloc(vecsize*sizeof(belowlitter));
    leafnitrogen = malloc(vecsize*sizeof(leafnitrogen));
    leafreductioncoefs = malloc(vecsize*sizeof(leafreductioncoefs));
    stomatalcondcoefs = malloc(vecsize*sizeof(stomatalcondcoefs));
    soilwatcont = malloc(vecsize*sizeof(soilwatcont));
    TTTc_c = malloc(vecsize*sizeof(TTTc_c));
    LAIc_c = malloc(vecsize*sizeof(LAIc_c));
    grainy = malloc(vecsize*sizeof(grainy));
    stemy = malloc(vecsize*sizeof(stemy));
    rooty = malloc(vecsize*sizeof(rooty));
    leafy = malloc(vecsize*sizeof(leafy));
    rhizomey = malloc(vecsize*sizeof(rhizomey));
    canopytrans = malloc(vecsize*sizeof(canopytrans));
    canopyassim = malloc(vecsize*sizeof(canopyassim));
    dayofyear = malloc(vecsize*sizeof(dayofyear));
    hour = malloc(vecsize*sizeof(hour));
    
    
  
    
    cwsmatrix = (double **) malloc(vecsize*sizeof(double *));
    rdmatrix = (double **) malloc(vecsize*sizeof(double *));
    waterfluxmatrix = (double **) malloc(vecsize*sizeof(double *));
    psimmatrix = (double **) malloc(vecsize*sizeof(double *));
    for(i = 0; i < vecsize; i++)
    {
      cwsmatrix[i] = (double *) malloc(soillayers*sizeof(double));
      rdmatrix[i] = (double *) malloc(soillayers*sizeof(double));
      waterfluxmatrix[i] = (double *) malloc(soillayers*sizeof(double));
      psimmatrix[i] = (double *) malloc(soillayers*sizeof(double));
    }

  /*************************END*****************************/
   
	

 /*************CALL TO CROPGRO_C WILL GO HERE*********************/
 
   CropGro_c(lat,          /* Latitude    ~           1 */ 
      pt_doy,                 /* Day of the year   -        2 */
      pt_hr,                  /* Hour of the day     -      3 */
      pt_solar,               /* Solar Radiation    -       4 */
      pt_temp,                /* Temperature        -       5 */
	    pt_rh,                  /* Relative humidity   -      6 */ 
	    pt_windspeed,           /* Wind Speed -               7 */ 
	    pt_precip,              /* Precipitation     -        8 */
	    kd,                  /* K D (ext coeff diff) -     9 */
	    chilhf,              /* NEED TO REVERT TO DOUBLE * Chi and Height factor  -  10 */
	    nlayers,             /* # Lay canop   -           11 */
	    Rhizome,             /* Ini Rhiz        -         12 */
      propLeaf,                /* i rhiz to leaf    -       13 */
      sencoefs,            /* sene coefs   !!!!!!!MADE NEW VARIABLE ADJUST IN OTHER FILE    -        14 */
      timestep,            /* time step       -         15 */
      vecsize,             /* vector size        -      16 */
	    Sp,              /* Spec Leaf Area     -      17 */
	   // SEXP SPD, UNUSED VARIABLE OMITTED FROM ARGS                 /* Spec Lefa Area Dec  -     18 */
	    dbpcoefs,            /* Dry Bio Coefs      -      19 */
	    thermalp,            /* Themal Periods   -        20 */
	    vmax1,                /* Vmax of photo     -       21 */
	    alpha1,               /* Quantum yield   -         22 */
	    kparm1,               /* k parameter (photo) -     23 */
	    theta,               /* theta param (photo)   -   24 */
	    beta,                /* beta param  (photo) -     25 */
	    Rd1,                  /* Dark Resp   (photo) -     26 */
	    Ca,                /* CO2 atmosph       -       27 */
	    b01,                  /* Int (Ball-Berry)  -       28 */
	    b11,                  /* Slope (Ball-Berry)  -     29 */
	    ws,                  /* Water stress flag   -     30 */
	    soilcoefs,           /* Soil Coefficients !!!!! MADE NEW VARIABLE ADJUST IN OTHER FILE  -     31 */
	    LeafN_0,              /* Ini Leaf Nitrogen -       32 */
	    kLN,                 /* Decline in Leaf Nitr -    33 */
	   // SEXP VMAXB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on Vmax   -   34 */
	   // SEXP ALPHAB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on alpha   -  35 */
	    mresp,               /* Maintenance resp !!!!!CHANGED VAR ADJUST IN OTHER FILE    -    36 */
	    soilType,            /* Soil type       -         37 */
	    wsFun,               /* Water Stress Func  -      38 */
	    centcoefs,           /* Century coefficients !!!!!!!!! MADE NeW VARiABLE ADJUST IN OTHER FILE  -  39 */
      centTimestep,        /* Century timestep   -      40 */ 
	   // SEXP CENTKS, UNUSED VAR OMMITTED FROM ARGS             /* Century decomp rates-     41 */
	    soillayers,          /* # soil layers        -    42 */
	    soildepths,          /* Soil Depths      -        43 */
	    cwsVec,                 /* Current water status !!!!! CHANGED var ADJST IN NEW  -   44 */
	    hydrdist,            /* Hydraulic dist flag   -   45 */
	    secs,                /* Soil empirical coefs !!!MADE NEW VAR CHANGE IN OTHER  -   46 */
	    kpLN,                /* Leaf N decay         -    47 */
	    lnb0,                /* Leaf N Int        -       48 */
	    lnb1,                /* Leaf N slope         -    49 */
      lnfun,               /* Leaf N func flag      -   50 */
      upperT,           /* Temperature Limitations photoParms- */
	    lowerT,         //-
	    nnitrop, //-        MADE NEW VAR CHANGE IN OTHER
      sompoolsfromr, // I don't know if I should change this to double * Let me know
      soiltexturefromr,// MADE NEW VAR CHANGE IN OTHER)    
      LAId_c, /********Start of output vars******/
      RhizomeLitterD,  
      RootLitterD,  
      LeafLitterD,
      StemLitterD,
      RhizomeD,
      RootD,
      StemD,
      LeafD,
      leafDarkResp,
      rhizomeMResp,
      rootMResp,
      stemMResp,
      AutoResp,
      npp,
      gpp,
      dayafterplanting,    
      leafpsimvec,
      soilevaporation,
      minnitrovec,
      spvec,
      alphavec,
      vmaxvec,
      leafreductioncoefs,
      stomatalcondcoefs,
      abovelitter,
      belowlitter,
      respvec,
      soilwatcont,
      leafnitrogen,
      GDD_c,
      TTTc_c,
      LAIc_c,
      grainy,
      leafy,
      stemy, 
      rooty, 
      rhizomey,
      canopytrans,
      canopyassim,
      dayofyear,
      hour,
      cwsmatrix,
      psimmatrix,
      rdmatrix,
      waterfluxmatrix,
      snPools,
      scPools
      );
  /*************CALL TO CROPGRO_C WILL GO HERE*********************/   
    /************************************START OF OUTPUT RECASTING TO SEXP**************/
    REAL(SCpools)[0] = scPools[0];
  	REAL(SCpools)[1] = scPools[1];
		REAL(SCpools)[2] = scPools[2];
		REAL(SCpools)[3] = scPools[3];
		REAL(SCpools)[4] = scPools[4];
		REAL(SCpools)[5] = scPools[5];
		REAL(SCpools)[6] = scPools[6];
		REAL(SCpools)[7] = scPools[7];
		REAL(SCpools)[8] = scPools[8];

		REAL(SNpools)[0] = snPools[0];
		REAL(SNpools)[1] = snPools[1];
		REAL(SNpools)[2] = snPools[2];
		REAL(SNpools)[3] = snPools[3];
		REAL(SNpools)[4] = snPools[4];
		REAL(SNpools)[5] = snPools[5];
		REAL(SNpools)[6] = snPools[6];
		REAL(SNpools)[7] = snPools[7];
		REAL(SNpools)[8] = snPools[8];
    
    i2 = 0;
    
    for(i = 0; i < vecsize; i++)
    {
      
      REAL(DayofYear)[i] =  dayofyear[i];
  		REAL(Hour)[i] =  hour[i];
  		REAL(CanopyAssim)[i] =  canopyassim[i];
  		REAL(CanopyTrans)[i] =  canopytrans[i]; 
  		REAL(Leafy)[i] = leafy[i]; //Leaf;
  		REAL(Stemy)[i] = stemy[i]; // Stem;
  		REAL(Rooty)[i] =  rooty[i]; //Root;
  		REAL(Rhizomey)[i] = rhizomey[i]; //Rhizome;
  		REAL(Grainy)[i] = grainy[i];
  		REAL(LAIc)[i] = LAIc_c[i];
  		REAL(SoilWatCont)[i] = soilwatcont[i];
  		REAL(StomatalCondCoefs)[i] = stomatalcondcoefs[i];
  		REAL(LeafReductionCoefs)[i] = leafreductioncoefs[i];
  		REAL(LeafNitrogen)[i] = leafnitrogen[i];
  		REAL(AboveLitter)[i] = abovelitter[i];
  		REAL(BelowLitter)[i] = belowlitter[i];
  		REAL(VmaxVec)[i] = vmaxvec[i];
  		REAL(AlphaVec)[i] = alphavec[i];
  		REAL(SpVec)[i] = spvec[i];
  		REAL(MinNitroVec)[i] = minnitrovec[i];
  		REAL(RespVec)[i] = respvec[i];
  		REAL(SoilEvaporation)[i] = soilevaporation[i];
  		REAL(LeafPsimVec)[i] = leafpsimvec[i];
      
      if(soillayers>1)
      {
          for(i3=0;i<soillayers;i++)
          {
              REAL(cwsMat)[i3+i*soillayers] = cwsmatrix[i][i3];
              REAL(rdMat)[i3 + i*soillayers] = rdmatrix[i][i3];
              REAL(waterfluxMat)[i3 + i*soillayers] = waterfluxmatrix[i][i3];
            
          }
        
      }
      else
      {
          i3 = 0;
          REAL(cwsMat)[i] = cwsmatrix[i][i3];
          REAL(psimMat)[i] = psimmatrix[i][i3];
      }
      
      
      if(i%24==0)
      {
           REAL(GPP)[i2]=gpp[i2];
           REAL(LeafDarkResp)[i2]=leafDarkResp[i2];
           REAL(StemMResp)[i2]=stemMResp[i2];
           REAL(RootMResp)[i2]=rootMResp[i2];
           REAL(RhizomeMResp)[i2]=rhizomeMResp[i2];
           REAL(autoRESP)[i2]= AutoResp[i2];
           REAL(NPP)[i2]=npp[i2];
           REAL(Stemd)[i2]=StemD[i2];
           REAL(Leafd)[i2]=LeafD[i2];
           REAL(Rootd)[i2]=RootD[i2];
           REAL(Rhizomed)[i2]=RhizomeD[i2];
           REAL(Stemlitterd)[i2]=StemLitterD[i2];
           REAL(Leaflitterd)[i2]=LeafLitterD[i2];
           REAL(Rootlitterd)[i2]=RootLitterD[i2];
           REAL(Rhizomelitterd)[i2]=RhizomeLitterD[i2];
           REAL(DayafterPlanting)[i2]=dayafterplanting[i2];  
           REAL(GDD)[i2]=GDD_c[i2];
           REAL(LAId)[i2]=LAId_c[i2];
           i2++;
           
      }
    }
    
    /****************************END***********************/
    

	SET_VECTOR_ELT(lists,0,DayofYear);
	SET_VECTOR_ELT(lists,1,Hour);
	SET_VECTOR_ELT(lists,2,CanopyAssim);
	SET_VECTOR_ELT(lists,3,CanopyTrans);
	SET_VECTOR_ELT(lists,4,Leafy);
	SET_VECTOR_ELT(lists,5,Stemy);
	SET_VECTOR_ELT(lists,6,Rooty);
	SET_VECTOR_ELT(lists,7,Rhizomey);
	SET_VECTOR_ELT(lists,8,Grainy);
	SET_VECTOR_ELT(lists,9,LAIc);
	SET_VECTOR_ELT(lists,10,TTTc);
	SET_VECTOR_ELT(lists,11,SoilWatCont);
	SET_VECTOR_ELT(lists,12,StomatalCondCoefs);
	SET_VECTOR_ELT(lists,13,LeafReductionCoefs);
	SET_VECTOR_ELT(lists,14,LeafNitrogen);
	SET_VECTOR_ELT(lists,15,AboveLitter);
	SET_VECTOR_ELT(lists,16,BelowLitter);
	SET_VECTOR_ELT(lists,17,VmaxVec);
	SET_VECTOR_ELT(lists,18,AlphaVec);
	SET_VECTOR_ELT(lists,19,SpVec);
	SET_VECTOR_ELT(lists,20,MinNitroVec);
	SET_VECTOR_ELT(lists,21,RespVec);
	SET_VECTOR_ELT(lists,22,SoilEvaporation);
	SET_VECTOR_ELT(lists,23,cwsMat);
	SET_VECTOR_ELT(lists,24,psimMat);
	SET_VECTOR_ELT(lists,25,rdMat);
	SET_VECTOR_ELT(lists,26,SCpools);
	SET_VECTOR_ELT(lists,27,SNpools);
	SET_VECTOR_ELT(lists,28,LeafPsimVec);
  SET_VECTOR_ELT(lists,29,DayafterPlanting);
  SET_VECTOR_ELT(lists,30,GDD);
  SET_VECTOR_ELT(lists,31,GPP);
  SET_VECTOR_ELT(lists,32,NPP);
  SET_VECTOR_ELT(lists,33,autoRESP);  
  SET_VECTOR_ELT(lists,34,hetRESP);
  SET_VECTOR_ELT(lists,35,NER);
  SET_VECTOR_ELT(lists,36,StemMResp);
  SET_VECTOR_ELT(lists,37,RootMResp);
  SET_VECTOR_ELT(lists,38,RhizomeMResp);
  SET_VECTOR_ELT(lists,39,LeafDarkResp);
  SET_VECTOR_ELT(lists,40,Stemd);
  SET_VECTOR_ELT(lists,41,Leafd);
  SET_VECTOR_ELT(lists,42,Rootd);
  SET_VECTOR_ELT(lists,43,Rhizomed);
  SET_VECTOR_ELT(lists,44,Stemlitterd);
  SET_VECTOR_ELT(lists,45,Leaflitterd);
  SET_VECTOR_ELT(lists,46,Rootlitterd);
  SET_VECTOR_ELT(lists,47,Rhizomelitterd);
  SET_VECTOR_ELT(lists,48,LAId);
  SET_VECTOR_ELT(lists,49,totalSOC);
  SET_VECTOR_ELT(lists,50,strucc1);
  SET_VECTOR_ELT(lists,51,strucc2);
  SET_VECTOR_ELT(lists,52,metabc1);
  SET_VECTOR_ELT(lists,53,metabc1);
  SET_VECTOR_ELT(lists,54,som1c1);
  SET_VECTOR_ELT(lists,55,som1c2);
  SET_VECTOR_ELT(lists,56,som2c1);
  SET_VECTOR_ELT(lists,57,som2c2);
  SET_VECTOR_ELT(lists,58,som3c);
  SET_VECTOR_ELT(lists,59,minN);
  SET_VECTOR_ELT(lists,60,waterfluxMat);


	SET_STRING_ELT(names,0,mkChar("DayofYear"));
	SET_STRING_ELT(names,1,mkChar("Hour"));
	SET_STRING_ELT(names,2,mkChar("CanopyAssim"));
	SET_STRING_ELT(names,3,mkChar("CanopyTrans"));
	SET_STRING_ELT(names,4,mkChar("Leaf"));
	SET_STRING_ELT(names,5,mkChar("Stem"));
	SET_STRING_ELT(names,6,mkChar("Root"));
	SET_STRING_ELT(names,7,mkChar("Rhizome"));
	SET_STRING_ELT(names,8,mkChar("Grain"));
	SET_STRING_ELT(names,9,mkChar("LAI"));
	SET_STRING_ELT(names,10,mkChar("ThermalT"));
	SET_STRING_ELT(names,11,mkChar("SoilWatCont"));
	SET_STRING_ELT(names,12,mkChar("StomatalCondCoefs"));
	SET_STRING_ELT(names,13,mkChar("LeafReductionCoefs"));
	SET_STRING_ELT(names,14,mkChar("LeafNitrogen"));
	SET_STRING_ELT(names,15,mkChar("AboveLitter"));
	SET_STRING_ELT(names,16,mkChar("BelowLitter"));
	SET_STRING_ELT(names,17,mkChar("VmaxVec"));
	SET_STRING_ELT(names,18,mkChar("AlphaVec"));
	SET_STRING_ELT(names,19,mkChar("SpVec"));
	SET_STRING_ELT(names,20,mkChar("MinNitroVec"));
	SET_STRING_ELT(names,21,mkChar("RespVec"));
	SET_STRING_ELT(names,22,mkChar("SoilEvaporation"));
	SET_STRING_ELT(names,23,mkChar("cwsMat"));
	SET_STRING_ELT(names,24,mkChar("psimMat"));
	SET_STRING_ELT(names,25,mkChar("rdMat"));
	SET_STRING_ELT(names,26,mkChar("SCpools"));
	SET_STRING_ELT(names,27,mkChar("SNpools"));
	SET_STRING_ELT(names,28,mkChar("LeafPsimVec"));
  SET_STRING_ELT(names,29,mkChar("DayafterPlanting"));
  SET_STRING_ELT(names,30,mkChar("GDD"));
  SET_STRING_ELT(names,31,mkChar("GPP"));
  SET_STRING_ELT(names,32,mkChar("NPP"));
  SET_STRING_ELT(names,33,mkChar("autoRESP"));
  SET_STRING_ELT(names,34,mkChar("hetRESP"));
  SET_STRING_ELT(names,35,mkChar("NER"));
  SET_STRING_ELT(names,36,mkChar("StemMResp"));
  SET_STRING_ELT(names,37,mkChar("RootMResp"));
  SET_STRING_ELT(names,38,mkChar("RhizomeMResp"));
  SET_STRING_ELT(names,39,mkChar("LeafDarkResp"));
  SET_STRING_ELT(names,40,mkChar("Stemd"));
  SET_STRING_ELT(names,41,mkChar("Leafd"));
  SET_STRING_ELT(names,42,mkChar("Rootd"));
  SET_STRING_ELT(names,43,mkChar("Rhizomed"));
  SET_STRING_ELT(names,44,mkChar("Stemlitterd"));
  SET_STRING_ELT(names,45,mkChar("Leaflitterd"));
  SET_STRING_ELT(names,46,mkChar("Rootlitterd"));
  SET_STRING_ELT(names,47,mkChar("Rhizomelitterd"));
  SET_STRING_ELT(names,48,mkChar("LAId"));
  SET_STRING_ELT(names,49,mkChar("totalSOC"));
  SET_STRING_ELT(names,50,mkChar("strucc1"));
  SET_STRING_ELT(names,51,mkChar("strucc2"));
  SET_STRING_ELT(names,52,mkChar("metabc1"));
  SET_STRING_ELT(names,53,mkChar("metabc1"));
  SET_STRING_ELT(names,54,mkChar("som1c1"));
  SET_STRING_ELT(names,55,mkChar("som1c2"));
  SET_STRING_ELT(names,56,mkChar("som2c1"));
  SET_STRING_ELT(names,57,mkChar("som2c2"));
  SET_STRING_ELT(names,58,mkChar("som3c"));
   SET_STRING_ELT(names,59,mkChar("minN"));
   SET_STRING_ELT(names,60,mkChar("waterfluxMat"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(63);
	return(lists);
}
