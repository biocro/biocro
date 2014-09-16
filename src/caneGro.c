/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <time.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxcaneGro.h"
#include "CanA_3D_Structure.h"

SEXP caneGro(SEXP LAT,                 /* Latitude                  1 */ 
	    SEXP DOY,                 /* Day of the year           2 */
	    SEXP HR,                  /* Hour of the day           3 */
	    SEXP SOLAR,               /* Solar Radiation           4 */
	    SEXP TEMP,                /* Temperature               5 */
	    SEXP RH,                  /* Relative humidity         6 */ 
	    SEXP WINDSPEED,           /* Wind Speed                7 */ 
	    SEXP PRECIP,              /* Precipitation             8 */
	    SEXP KD,                  /* K D (ext coeff diff)      9 */
	    SEXP CHILHF,              /* Chi and Height factor    10 */
	    SEXP NLAYERS,             /* # Lay canop              11 */
	    SEXP RHIZOME,             /* Ini Rhiz                 12 */
      SEXP IRTL,                /* i rhiz to leaf           13 */
      SEXP SENCOEFS,            /* sene coefs               14 */
      SEXP TIMESTEP,            /* time step                15 */
      SEXP VECSIZE,             /* vector size              16 */
	    SEXP SPLEAF,              /* Spec Leaf Area           17 */
	    SEXP SPD,                 /* Spec Lefa Area Dec       18 */
	    SEXP DBPCOEFS,            /* Dry Bio Coefs            19 */
	    SEXP THERMALP,            /* Themal Periods           20 */
	    SEXP VMAX,                /* Vmax of photo            21 */
	    SEXP ALPHA,               /* Quantum yield            22 */
	    SEXP KPARM,               /* k parameter (photo)      23 */
	    SEXP THETA,               /* theta param (photo)      24 */
	    SEXP BETA,                /* beta param  (photo)      25 */
	    SEXP RD,                  /* Dark Resp   (photo)      26 */
	    SEXP CATM,                /* CO2 atmosph              27 */
	    SEXP B0,                  /* Int (Ball-Berry)         28 */
	    SEXP B1,                  /* Slope (Ball-Berry)       29 */
	    SEXP WS,                  /* Water stress flag        30 */
	    SEXP SOILCOEFS,           /* Soil Coefficients        31 */
	    SEXP ILEAFN,              /* Ini Leaf Nitrogen        32 */
	    SEXP KLN,                 /* Decline in Leaf Nitr     33 */
	    SEXP VMAXB1,              /* Effect of N on Vmax      34 */
	    SEXP ALPHAB1,             /* Effect of N on alpha     35 */
	    SEXP MRESP,               /* Maintenance resp         36 */
	    SEXP SOILTYPE,            /* Soil type                37 */
	    SEXP WSFUN,               /* Water Stress Func        38 */
	    SEXP CENTCOEFS,           /* Century coefficients     39 */
      SEXP CENTTIMESTEP,        /* Century timestep         40 */ 
	    SEXP CENTKS,              /* Century decomp rates     41 */
	    SEXP SOILLAYERS,          /* # soil layers            42 */
	    SEXP SOILDEPTHS,          /* Soil Depths              43 */
	    SEXP CWS,                 /* Current water status     44 */
	    SEXP HYDRDIST,            /* Hydraulic dist flag      45 */
	    SEXP SECS,                /* Soil empirical coefs     46 */
	    SEXP KPLN,                /* Leaf N decay             47 */
	    SEXP LNB0,                /* Leaf N Int               48 */
	    SEXP LNB1,                /* Leaf N slope             49 */
      SEXP LNFUN,               /* Leaf N func flag         50 */
/************************************************************************************/
	    SEXP SUGARCOEFS,          /* Coefficients for sugarcane biomass partitioning 51*/
	    SEXP UPPERTEMP,           /* Temperature Limitations photoParms */
	    SEXP LOWERTEMP,            /*temperature Limitation photoParms */
	    SEXP MAXLN,                 /*Cyclic Variation in leaf N nitroParms*/
	    SEXP MINLN,               /* Cyclic Variation in leaf N nitroParms*/
	    SEXP DAYMAXLN,              /*Cyclic Variation in leaf N nitroParms*/
	    SEXP LEAFTURNOVER,         /* Leaf Turnover rate in percentage*/
	    SEXP ROOTTURNOVER,        /* Root turnover rate in percentage */
	    SEXP LEAFREMOBILIZE,     /*fraction of leaf remobilzed upon senescence */ 
	    SEXP OPTIONTOCALCULATEROOTDEPTH,
	    SEXP ROOTFRONTVELOCITY,
      SEXP NNITROP,
      SEXP IRRIG,
      SEXP FROSTP
	)
{


/*********************3D Canopy Parameters*********************************/
double canparms=1.0;
double nrows=2806;
double ncols=27;
double **canopy3Dstructure;
int ihere,jhere;

  canopy3Dstructure =  malloc(nrows * sizeof(double *));

  for(ihere = 0; ihere < nrows; ihere++)
	{
		canopy3Dstructure[ihere] =  malloc((ncols+2) * sizeof(double));
	}
  //Initializing the canopy matrix
  for ( ihere=0;ihere<nrows;ihere++)
  {
    for (jhere=0;jhere<ncols;jhere++)
    {
      canopy3Dstructure[ihere][jhere]=0.0;
    }
  }
 /*********************************************************/

	int vecsize = INTEGER(VECSIZE)[0];
	
	double *newLeafcol=malloc(vecsize*sizeof(double));
				  if(newLeafcol==NULL)
				  { 
				    Rprintf("Out of Memory for newLeafcol\n");
				    exit;
				  }
	double *newStemcol=malloc(vecsize*sizeof(double));
				  if(newStemcol==NULL)
				  { 
				    Rprintf("Out of Memory for newStemcol\n");
				    exit;
				  }
	double *newRootcol=malloc(vecsize*sizeof(double));
				  if(newRootcol==NULL)
				  { 
				    Rprintf("Out of Memory for newRootcol\n");
				    exit;
				  }
	double *newRhizomecol=malloc(vecsize*sizeof(double));
				  if(newRhizomecol==NULL)
				  { 
				    Rprintf("Out of Memory for newRhizomecol\n");
				    exit;
				  }

				  double upperT=REAL(UPPERTEMP)[0];
				  double lowerT=REAL(LOWERTEMP)[0];				 
///////////////////////////////////////////////////////////
	struct nitroParms nitroparms;
	double TEMPdoubletoint;
	nitroparms.ileafN=REAL(NNITROP)[0];
        nitroparms.kln=REAL(NNITROP)[1];
	nitroparms.Vmaxb1=REAL(NNITROP)[2];
	nitroparms.Vmaxb0=REAL(NNITROP)[3];
	nitroparms.alphab1=REAL(NNITROP)[4];
	nitroparms.alphab0=REAL(NNITROP)[5];
        nitroparms.Rdb1=REAL(NNITROP)[6];
	nitroparms.Rdb0=REAL(NNITROP)[7];
	nitroparms.kpLN=REAL(NNITROP)[8];
	nitroparms.lnb0=REAL(NNITROP)[9];
	nitroparms.lnb1=REAL(NNITROP)[10];
	TEMPdoubletoint=REAL(NNITROP)[11];
	nitroparms.lnFun=(int)TEMPdoubletoint;
	nitroparms.maxln=REAL(NNITROP)[12];
	nitroparms.minln=REAL(NNITROP)[13];
	nitroparms.daymaxln=REAL(NNITROP)[14];
//  Rprintf("%f, %f, %f, %f, %i \n",  nitroparms.Vmaxb1,nitroparms.Vmaxb0,   nitroparms.alphab1,  nitroparms.alphab0,nitroparms.lnFun);
/////////////////////////////////////////////////////////////////

  struct frostParms frostparms;
  frostparms.leafT0=REAL(FROSTP)[0];
  frostparms.leafT100=REAL(FROSTP)[1];
  frostparms.stemT0=REAL(FROSTP)[2];
  frostparms.stemT100=REAL(FROSTP)[3];
  double leaffrostdamage;
  
 

	/* This creates vectors which will collect the senesced plant
	   material. This is needed to calculate litter and therefore carbon
	   in the soil and then N in the soil. */

	double iSp, Sp , propLeaf;
	int i, i2, i3;
	int dailyvecsize;
	double rootfrontvelocity=REAL(ROOTFRONTVELOCITY)[0];
	int optiontocalculaterootdepth = INTEGER(OPTIONTOCALCULATEROOTDEPTH)[0];


	double vmax1;
	double alpha1;
	double kparm1;
	double theta;
	double beta;
	double Rd1, Ca;
	double b01, b11;

	double Leaf, Stem, Root, LAI, Grain = 0.0;
	double TTc = 0.0,TT12c=0.0;
	double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kSeedcane = 0.0, kGrain = 0.0;
	double newLeaf, newStem = 0.0, newRoot, newRhizome, newGrain = 0.0;

	/* Variables needed for collecting litter */
	double LeafLitter_d = 0.0, StemLitter_d = 0.0;
	double RootLitter_d = 0.0, RhizomeLitter_d = 0.0;
	double ALitter = 0.0, BLitter = 0.0;
	/* Maintenance respiration */

	double mrc1 = REAL(MRESP)[0];
	double mrc2 = REAL(MRESP)[1]; 

	double waterCont;
	double StomWS = 1, LeafWS = 1,SugarStress=1,deepak_temp;
	int timestep;
	double CanopyA, CanopyT;

	double Rhizome;
	double Sugar,newSugar,SeneSeedcane;
	double laimax;

	/* Soil Parameters*/
	double FieldC, WiltP, phi1, phi2, soilDepth;
	int soilType, wsFun;
	double LeafNmax = REAL(MAXLN)[0]; /* max N conc of top leaf in mmol/m2*/
	double LeafNmin = REAL(MINLN)[0];  /* min N conc of top leaf in mmol/m2 */
	double dayofMaxLeafN = REAL(DAYMAXLN)[0]; /* day when maxm leaf N occuers */
	double LeafN, kLN,currentday;
	double soilEvap, TotEvap;
	int soillayers = INTEGER(SOILLAYERS)[0];
	double cwsVec[soillayers];
	for(i2=0;i2<soillayers;i2++){
		cwsVec[i2] = REAL(CWS)[i2];
	}
	double cwsVecSum = 0.0;
	/* Some soil related empirical coefficients */
	double rfl = REAL(SECS)[0];  /* root factor lambda */
	double rsec = REAL(SECS)[1]; /* radiation soil evaporation coefficient */
	double rsdf = REAL(SECS)[2]; /* root soil depth factor */
	double scsf = REAL(SOILCOEFS)[6]; /* stomatal conductance sensitivity factor */ /* Rprintf("scsf %.2f",scsf); */
	double transpRes = REAL(SOILCOEFS)[7]; /* Resistance to transpiration from soil to leaf */
	double leafPotTh = REAL(SOILCOEFS)[8]; /* Leaf water potential threshold */

        /* Parameters for calculating leaf water potential */
	double LeafPsim = 0.0;

        /* Effect of Nitrogen */
	double kpLN = REAL(KPLN)[0];
	double lnb0 = REAL(LNB0)[0]; 
	double lnb1 = REAL(LNB1)[0];
	int lnfun = INTEGER(LNFUN)[0];
	double leafrate = REAL(LEAFTURNOVER)[0];
	double rootrate = REAL(ROOTTURNOVER)[0];

	/* Variables for Respiration Calculations. I am currently using hard coded values. I shall write a function that can takes these values as input. I have used optim function and data from Liu to get these values. */
        double gRespCoeff=0.15;/*This is constant fraction to calculate Growth respiration from DSSAT*/
        double Qleaf=1.58, Qstem=1.80, Qroot=1.80; /* Q factors for Temp dependence on Maintenance Respiration*/
	double mRespleaf=0.012,mRespstem=0.00016,mResproot=0.00036;
	double mResp;

	double SeneLeaf, SeneStem, SeneRoot = 0.0, SeneRhizome = 0.0 ;
	double *sti , *sti2, *sti3, *sti4; 
	double Remob;
	int k = 0, q = 0, m = 0, n = 0;
	int ri = 0;

/* all of these parameters are for Cuadra biomass partitioning */
	double TT0 = REAL(SUGARCOEFS)[0]; 
	double  TTseed = REAL(SUGARCOEFS)[1]; 
	double Tmaturity = REAL(SUGARCOEFS)[2]; 
	double Rd = REAL(SUGARCOEFS)[3]; 
	double Alm = REAL(SUGARCOEFS)[4]; 
	double Arm = REAL(SUGARCOEFS)[5]; 
	double Clstem = REAL(SUGARCOEFS)[6]; 
	double Ilstem = REAL(SUGARCOEFS)[7]; 
	double Cestem = REAL(SUGARCOEFS)[8]; 
	double Iestem = REAL(SUGARCOEFS)[9]; 
	double Clsuc = REAL(SUGARCOEFS)[10]; 
	double Ilsuc = REAL(SUGARCOEFS)[11]; 
	double Cesuc = REAL(SUGARCOEFS)[12]; 
	double Iesuc = REAL(SUGARCOEFS)[13]; 
/* variable declaration for cuadra biomass partitionign ends here */
	

	struct Can_Str Canopy, CanopyRd;
	struct ws_str WaterS;
	struct dbp_sugarcane_str dbpS;
  struct dbp_str dbpS_old;
	struct cenT_str centS; 
	struct soilML_str soilMLS;
	struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
	soTexS = soilTchoose(INTEGER(SOILTYPE)[0]);

	centS.SCs[0] = 0.0;
	centS.SCs[1] = 0.0;
	centS.SCs[2] = 0.0;
	centS.SCs[3] = 0.0;
	centS.SCs[4] = 0.0;
	centS.SCs[5] = 0.0;
	centS.SCs[6] = 0.0;
	centS.SCs[7] = 0.0;
	centS.SCs[8] = 0.0;

	SEXP lists, names;

	SEXP DayofYear;
	SEXP Hour;
	SEXP CanopyAssim;
	SEXP CanopyTrans;
	SEXP Leafy;
	SEXP Stemy;
	SEXP Rooty;
	SEXP Sugary;
	SEXP Fibery;
	SEXP Seedcaney;
	SEXP LAIc;
	SEXP TTTc;
	SEXP SoilWatCont;
	SEXP StomatalCondCoefs;
	SEXP LeafReductionCoefs;
	SEXP LeafNitrogen;
	SEXP AboveLitter;
	SEXP BelowLitter;
	SEXP VmaxVec;
	SEXP AlphaVec;
	SEXP SpVec;
	SEXP SoilEvaporation;
	SEXP cwsMat;
	SEXP psimMat; /* Holds the soil water potential */
	SEXP rdMat;
	SEXP LeafPsimVec;
        SEXP LeafLittervec;
        SEXP StemLittervec;
	SEXP RootLittervec;
	SEXP Leafmaintenance;
        SEXP Stemmaintenance;
        SEXP Rootmaintenance;
        SEXP Seedcanemaintenance;
	SEXP kkleaf;
        SEXP kkroot;
        SEXP kkstem;
	SEXP kkseedcane;
	SEXP kkfiber;
        SEXP kksugar;
        SEXP dailyThermalT;
	SEXP moisturecontent;
	SEXP dayafterplanting;

		


	vecsize = length(DOY);
	dailyvecsize=(int)(vecsize/24)+1;

	PROTECT(lists = allocVector(VECSXP,44)); /* total of 32 variables are to be sent to R */
	PROTECT(names = allocVector(STRSXP,44)); /* name of 32 variables */



        PROTECT(DayofYear = allocVector(REALSXP,dailyvecsize));
	PROTECT(Hour = allocVector(REALSXP,vecsize));
	PROTECT(CanopyAssim = allocVector(REALSXP,vecsize));
	PROTECT(CanopyTrans = allocVector(REALSXP,vecsize));
	PROTECT(Leafy = allocVector(REALSXP,dailyvecsize));
	PROTECT(Stemy = allocVector(REALSXP,dailyvecsize));
	PROTECT(Rooty = allocVector(REALSXP,dailyvecsize));
	PROTECT(Sugary = allocVector(REALSXP,dailyvecsize));
	PROTECT(Fibery = allocVector(REALSXP,dailyvecsize));
	PROTECT(Seedcaney = allocVector(REALSXP,dailyvecsize));
	PROTECT(LAIc = allocVector(REALSXP,dailyvecsize));
	PROTECT(TTTc = allocVector(REALSXP,vecsize));
	PROTECT(SoilWatCont = allocVector(REALSXP,vecsize));
	PROTECT(StomatalCondCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafReductionCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafNitrogen = allocVector(REALSXP,dailyvecsize));
	PROTECT(AboveLitter = allocVector(REALSXP,dailyvecsize));
	PROTECT(BelowLitter = allocVector(REALSXP,dailyvecsize));
	PROTECT(VmaxVec = allocVector(REALSXP,dailyvecsize));
	PROTECT(AlphaVec = allocVector(REALSXP,dailyvecsize));
	PROTECT(SpVec = allocVector(REALSXP,dailyvecsize));
	PROTECT(SoilEvaporation = allocVector(REALSXP,vecsize));
	PROTECT(cwsMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(psimMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(rdMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));
	PROTECT(LeafLittervec = allocVector(REALSXP,dailyvecsize));
	PROTECT(StemLittervec = allocVector(REALSXP,dailyvecsize));
	PROTECT(RootLittervec = allocVector(REALSXP,dailyvecsize));
	PROTECT(Leafmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Stemmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Rootmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Seedcanemaintenance = allocVector(REALSXP,vecsize));
	PROTECT(kkleaf = allocVector(REALSXP,dailyvecsize));
	PROTECT(kkroot = allocVector(REALSXP,dailyvecsize));
	PROTECT(kkstem = allocVector(REALSXP,dailyvecsize));
	PROTECT(kkseedcane = allocVector(REALSXP,dailyvecsize));
	PROTECT(kkfiber = allocVector(REALSXP,dailyvecsize));
	PROTECT(kksugar = allocVector(REALSXP,dailyvecsize));
	PROTECT(dailyThermalT = allocVector(REALSXP,dailyvecsize));
	PROTECT(moisturecontent = allocVector(REALSXP,dailyvecsize));
	PROTECT(dayafterplanting = allocVector(REALSXP,dailyvecsize));
       


	/* Picking vmax, alpha and kparm */
	vmax1 = REAL(VMAX)[0];
	alpha1 = REAL(ALPHA)[0];
	kparm1 = REAL(KPARM)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];
	Rd1 = REAL(RD)[0];
	Ca = REAL(CATM)[0];
	b01 = REAL(B0)[0];
	b11 = REAL(B1)[0];


	LeafN = REAL(ILEAFN)[0]; /* Initial value of N in the leaf */
	kLN = REAL(KLN)[0];
	timestep = INTEGER(TIMESTEP)[0];
	Rhizome = REAL(RHIZOME)[0];
	Sp = REAL(SPLEAF)[0]; 
	SeneLeaf = REAL(SENCOEFS)[0];
	SeneStem = REAL(SENCOEFS)[1];
	SeneRoot = REAL(SENCOEFS)[2];
	SeneRhizome = REAL(SENCOEFS)[3];

	/* Soil Parameters */
	FieldC = REAL(SOILCOEFS)[0];
	WiltP = REAL(SOILCOEFS)[1];
	phi1 = REAL(SOILCOEFS)[2];
	phi2 = REAL(SOILCOEFS)[3];
	soilDepth = REAL(SOILCOEFS)[4];
	waterCont = REAL(SOILCOEFS)[5];
	wsFun = INTEGER(WSFUN)[0];
	soilType = INTEGER(SOILTYPE)[0];

	propLeaf = REAL(IRTL)[0]; 
	Leaf=Rhizome*propLeaf;
	/* It is useful to assume that there is a small amount of
	   leaf area at the begining of the growing season. */

	/* Initial proportion of the rhizome that is turned
	   into leaf the first hour */
	Stem = Rhizome * 0.001;
	Root = Rhizome * 0.001;
	LAI = Leaf * Sp;
	iSp = Sp;

/* Creating pointers to avoid calling functions REAL and INTEGER so much */
	int *pt_doy = INTEGER(DOY);
	int *pt_hr = INTEGER(HR);
	double *pt_solar = REAL(SOLAR);
	double *pt_temp = REAL(TEMP);
	double *pt_rh = REAL(RH);
	double *pt_windspeed = REAL(WINDSPEED);
	double *pt_precip = REAL(PRECIP);
	double lat = REAL(LAT)[0];
	int nlayers = INTEGER(NLAYERS)[0];
	int ws = INTEGER(WS)[0];
	double kd = REAL(KD)[0];
	double chil = REAL(CHILHF)[0];
	double hf = REAL(CHILHF)[1];
	double moist,ddap;

	/* Creation of pointers outside the loop */
	sti = &newLeafcol[0]; /* This creates sti to be a pointer to the position 0
				 in the newLeafcol vector */
	sti2 = &newStemcol[0];
	sti3 = &newRootcol[0];
	sti4 = &newRhizomecol[0];
	
	double dailyassim=0.0;
	double dailytransp=0.0;
	double dailyLeafResp = 0.0;
        double dailyStemResp = 0.0;
        double totalMResp,Mresplimit,Mresp_reduction;
	double dailyRootResp = 0.0;
	double dailyRhizomeResp = 0.0;
	double LeafResp,StemResp,RootResp,RhizomeResp,Litter;
	double LAIi;
	double delTT;
	double dap;

	double rootlitter,deadroot,leaflitter,deadleaf,leafREMOB;
  double remobilizedleaf;
  double Remobfactorleaf=0.5;
	double Fiber,newFiber;
	double kSugar,kFiber,SeedcaneResp,Seedcane,newSeedcane,dailySeedcaneResp;
	double LAIold,LAInew;
	leafREMOB=REAL(LEAFREMOBILIZE)[0];
	int dayi=0; /* this is idnex for days */
            LAIi = Rhizome * propLeaf*Sp;
	    LAIold=LAIi; /* Enter simulation with LAIold variable to track precious LAI to implement water stress */
      
 
 
 /************Based on Today, we need to calculate leaf photosynthesis parameters before the main loop begins ******/
      currentday=*(pt_doy+0);
  	  LeafN=seasonal(LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,365.0,lat);
//      Rprintf("%f, %f, %f,%f,%f, %f\n",LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,lat,LeafN);
      if(nitroparms.lnFun == 0){
					vmax1 = vmax1;
					Rd1=Rd1;
					alpha1=alpha1;
				}else{
					vmax1=nitroparms.Vmaxb1*LeafN+nitroparms.Vmaxb0;
					if(vmax1<0) vmax1=0.0;
					alpha1=nitroparms.alphab1*LeafN+nitroparms.alphab0;
					Rd1=nitroparms.Rdb1*LeafN+nitroparms.Rdb0;
				   }
        
  /**************************************************************************************************************/
  
 //Initialization for 3D canopy 
 update_3Dcanopy_structure(canopy3Dstructure,canparms,nrows, ncols);
      
	for(i=0;i<vecsize;i++)
	{
		/* First calculate the elapsed Thermal Time*/
		/* The idea is that here I need to divide by the time step
		   to calculate the thermal time. For example, a 3 hour time interval
		   would mean that the division would need to by 8 */
		TTc += *(pt_temp+i) / (24/timestep); 
                if((*(pt_temp+i))<9)
                {
			TT12c = TT12c;
			delTT=0.0;
		}
    LAIold = Leaf * Sp;
    
    // Adjusting rain based on irrigation amount
    //   Rprintf("%f\n",REAL(IRRIG)[0]);
       /*
        if (REAL(IRRIG)[0]>0) {
       TotEvap*=0.01; //To convert total ET loss from Mg/ha to mm
       *(pt_precip+i)+=(TotEvap*REAL(IRRIG)[0]);
                            }
        */
     
		if((*(pt_temp+i))>= 9)
                {
			TT12c = TT12c + (*(pt_temp+i)-9) / (24/timestep);
			delTT=(*(pt_temp+i)- 9)/24.0;
		}
                
/* This calculated TTTc based on 0 base Temperature */    
/*		REAL(TTTc)[i] = TTc;

/* Now we are calculating TTc based on 12 base Temperature */
                REAL(TTTc)[i]=TT12c;
	     
	  
    /*  Do the magic! Calculate growth*/
/******************Here we are implementing 3D canopy with ray tracing model***************
 if (canopymodel==1){
  if(newday){
    we are reading files directly
    but we need to use matlab to C code to generate canopty structure using LAI and other canopy parameters
     3Dcanopy=get3DCanopyStructure(3dparameters); 
	   }
     3Dcanopy_light=runraytracing(3Dcanopy, lat, day, hour,Idir, Idiff,xmin,xmax,ymin,ymax,zmin,zmax);


  Canopy = CanAC_3D(, *(pt_doy+i), *(pt_hr+i),
  		       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,LeafN, kpLN, lnb0, lnb1, nitroparms.lnFun,upperT,lowerT,nitroparms);
 }

//else{    
// Testing if I can call C++ ray tracing code from here




 Canopy = CanAC_3D (canparms, canopy3Dstructure,nrows, ncols,LAI,*(pt_doy+i), *(pt_hr+i),
    	       *(pt_solar+i), *(pt_temp+i),*(pt_rh+i), *(pt_windspeed+i),lat,vmax1,alpha1,kparm1,
  		       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws,kpLN,upperT,lowerT,LeafN,nitroparms);
********************/             


    Canopy = CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,LeafN, kpLN, lnb0, lnb1, nitroparms.lnFun,upperT,lowerT,nitroparms);


//}

/* This is an addition, which was omitted earlier */
/* WIMOVAC suggestsevaluating A grodd = Anet + Rd before proceeding with further calculations */

/* A better way to evaluate gross canopy assimilation would be to evaluate for individual layer in CanAC functions using leaf temperature. However, this will require changing CanAC functon, which I do not want to distrub for now.

		/* Collecting the results */
//		CanopyA = Canopy.Assim * timestep;
    CanopyA = Canopy.GrossAssim * timestep;
		CanopyT = Canopy.Trans * timestep;

                /* summing up hourly results to obtain daily assimilation */
	   dailyassim =  dailyassim + CanopyA;
	   dailytransp = dailytransp + CanopyT;
		 /* updated value of daily assimilation and transpiration */

		 /*Evaluate maintenance Respiration of different plant organs */
		   LeafResp=MRespiration(Leaf, Qleaf, mRespleaf, *(pt_temp+i), timestep);
		   REAL(Leafmaintenance)[i]=LeafResp;
		   StemResp=MRespiration(Stem, Qstem, mRespstem, *(pt_temp+i), timestep);
                   REAL(Stemmaintenance)[i]=StemResp;
	           RootResp=MRespiration(Root, Qroot, mResproot, *(pt_temp+i), timestep);
		     REAL(Rootmaintenance)[i]=RootResp;
                   SeedcaneResp=MRespiration(Seedcane, Qstem, mRespstem, *(pt_temp+i), timestep);
		     REAL(Seedcanemaintenance)[i]=SeedcaneResp;

		   /*summing up hourly maintenance Respiration to determine daily maintenance respiration of each plant organ */
		    dailyLeafResp =  dailyLeafResp + LeafResp;
		    dailyStemResp =  dailyStemResp + StemResp;
		    dailyRootResp =  dailyRootResp + RootResp;
		    dailySeedcaneResp =  dailySeedcaneResp + SeedcaneResp;

		/* Inserting the multilayer model */
		if(soillayers > 1){
			dap=(double)dayi; // converting dayi to double before passing to soilML to avoid rounding error
		             	soilMLS = soilML(*(pt_precip+i), CanopyT, &cwsVec[0], soilDepth, REAL(SOILDEPTHS), FieldC, WiltP,
					 phi1, phi2, soTexS, wsFun, INTEGER(SOILLAYERS)[0], Root, 
					 LAI, 0.68, *(pt_temp+i), *(pt_solar), *(pt_windspeed+i), *(pt_rh+i), 
							   INTEGER(HYDRDIST)[0], rfl, rsec, rsdf);//,optiontocalculaterootdepth,rootfrontvelocity, dap);


			StomWS = soilMLS.rcoefPhoto;
			LeafWS = soilMLS.rcoefSpleaf;
			soilEvap = soilMLS.SoilEvapo;
			for(i3=0;i3<soillayers;i3++){
				cwsVec[i3] = soilMLS.cws[i3];
				cwsVecSum += cwsVec[i3];
				REAL(cwsMat)[i3 + i*soillayers] = soilMLS.cws[i3];
				REAL(rdMat)[i3 + i*soillayers] = soilMLS.rootDist[i3];
			}

			waterCont = cwsVecSum / soillayers;
			cwsVecSum = 0.0;

		}else{

			soilEvap = SoilEvapo(LAI, 0.68, *(pt_temp+i), *(pt_solar+i), waterCont, FieldC, WiltP, 
                                             *(pt_windspeed+i), *(pt_rh+i), rsec);
			TotEvap = soilEvap + CanopyT;
			WaterS = watstr(*(pt_precip+i),TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
			waterCont = WaterS.awc;
			StomWS = WaterS.rcoefPhoto ; 
			LeafWS = WaterS.rcoefSpleaf;
			REAL(cwsMat)[i] = waterCont;
			REAL(psimMat)[i] = WaterS.psim;
		}

/* An alternative way of computing water stress is by doing the leaf
 * water potential. This is done if the wsFun is equal to 4 */

                if(wsFun == 4){
			/* Calculating the leaf water potential */
			/* From Campbell E = (Psim_s - Psim_l)/R or
			 * evaporation is equal to the soil water potential
			 * minus the leaf water potential divided by the resistance.
			 * This can be rearranged to Psim_l = Psim_s - E x R   */
			/* It is assumed that total resistance is 5e6 m^4 s^-1
			 * kg^-1 
			 * Transpiration is in Mg ha-2 hr-1
			 * Multiply by 1e3 to go from Mg to kg
			 * Multiply by 1e-4 to go from ha to m^2 
			 * This needs to go from hours to seconds that's
			 * why the conversion factor is (1/3600).*/
			LeafPsim = WaterS.psim - (CanopyT * 1e3 * 1e-4 * 1.0/3600.0) * transpRes;

			/* From WIMOVAVC the proposed equation to simulate the effect of water
			 * stress on stomatal conductance */
			if(LeafPsim < leafPotTh){
				/* StomWS = 1 - ((LeafPsim - leafPotTh)/1000 *
				 * scsf); In WIMOVAC this equation is used but
				 * the absolute values are taken from the
				 * potentials. Since they both should be
				 * negative and leafPotTh is greater than
				 * LeafPsim this can be rearranged to*/ 
				StomWS = 1 - ((leafPotTh - LeafPsim)/1000 * scsf);
				/* StomWS = 1; */
				if(StomWS < 0.1) StomWS = 0.1;
			}else{
				StomWS = 1;
			}
		}else{
			LeafPsim = 0;
		}

		/*	dbpS=SUGARCANE_DBP(TT12c); */
		/*See Details to check arguments ot CUADRA biomass partitionign function */

		/* A new day starts here */
		if( (i % 24) ==0) 
		{

		/* daily loss of assimilated CO2 via growth respiration */
	       	/* Here I am calling function to calculate Growth Respiration. Currently Using a fraction of 0.242 is lost via growth respiration. This fraction is taken from DSSAT                 4.5 manual for Sugarcane: parameter RespGcf *****/
		  totalMResp=dailyLeafResp+dailyStemResp+dailyRootResp+dailySeedcaneResp;
      
             Mresplimit=0.18*dailyassim;
             if(totalMResp>Mresplimit && totalMResp >0)
             {
               Mresp_reduction=Mresplimit/totalMResp;
               dailyLeafResp=dailyLeafResp*Mresp_reduction;
               dailyStemResp=dailyStemResp*Mresp_reduction;
               dailyRootResp=dailyRootResp*Mresp_reduction;
               dailySeedcaneResp=dailySeedcaneResp*Mresp_reduction;
               totalMResp=Mresplimit;
             }
             
  
      dailyassim=GrowthRespiration(dailyassim, gRespCoeff); /* Plant growth continues during day and night*/
      dailyassim = dailyassim-totalMResp; // take care of all respirations here to avoid reduction in stem
      // If  daily assim becomes negatice as governed by respiration losses then set net daily assim to zero and also adjust
      // respiration loss of stem and root so that they match mass balance
      // Here is the conditions when daily assimilation may become negative ona cloudy day
      
      
		/* daily loss via  maintenance respirations of different plant organs */
/*		  dailyassim=dailyassim-dailyLeafResp-dailyStemResp- dailyRootResp- dailyRhizomeResp;  *//* This is available photosynthate available for plant growth */

		  
		/* call the biomass partitioning function */

//		  dbpS = SUGARCANE_DBP_CUADRA(TT12c,TT0,TTseed,Tmaturity,Rd,Alm,Arm,Clstem,Ilstem,Cestem,Iestem,Clsuc,Ilsuc,Cesuc,Iesuc,*(pt_temp+i));
      	dbpS_old = sel_dbp_coef(REAL(DBPCOEFS), REAL(THERMALP), TT12c); 
          
          dbpS.kLeaf=dbpS_old.kLeaf;
          dbpS.kStem=dbpS_old.kStem;
  	      dbpS.kRoot=dbpS_old.kRoot;
		      dbpS.kSeedcane=dbpS_old.kRhiz;
		      dbpS.kSugar=0.0;
		      dbpS.kFiber=dbpS_old.kStem;
		  /*
		   dbpS.kLeaf=0.001;
		   dbpS.kStem=0.7;
		   dbpS.kRoot=0.299;
		   dbpS.kSeedcane=dbpS.kSeedcane;
		   dbpS.kSugar=0;
		   dbpS.kFiber=0;
		  */
			  
		   kLeaf = dbpS.kLeaf;
		   kStem = dbpS.kStem;
		   kRoot = dbpS.kRoot;
		   kSeedcane = dbpS.kSeedcane;
		   kSugar = dbpS.kSugar;
		   kFiber=dbpS.kFiber;
		   

		   REAL(kkleaf)[dayi] = dbpS.kLeaf;
		   REAL(kkstem)[dayi] = dbpS.kStem;
		   REAL(kksugar)[dayi]= dbpS.kSugar;
		   REAL(kkfiber)[dayi]= dbpS.kFiber;
		   REAL(kkroot)[dayi] = dbpS.kRoot;
		   REAL(kkseedcane)[dayi] = dbpS.kSeedcane;
		   REAL(dailyThermalT)[dayi]= TT12c;
		

		        	if(kLeaf >= 0)
				{
					newLeaf =  dailyassim * kLeaf;
			    *(sti+i) = newLeaf; 
                                         
			 }
				  else
				  {
					  warning("kleaf is negative");
          }
                
		
		     if(TT12c < SeneLeaf)
				 {
				   Leaf += newLeaf;
				   leaflitter=leaflitter+0.0;
				   REAL(LeafLittervec)[dayi]= leaflitter;
                                 }
				 else
				 {
				 deadleaf=(leafrate*Leaf)/100.0;  /* biomass of dead leaf */
         remobilizedleaf<-deadleaf*Remobfactorleaf;
				 leaflitter=leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
				 Leaf =Leaf+newLeaf-deadleaf+remobilizedleaf*33;
         Stem=Stem+remobilizedleaf*0.33;
         Root=Root+remobilizedleaf*0.33;
				 REAL(LeafLittervec)[dayi]= leaflitter; /* Collecting the leaf litter */ 
				 /*	 REAL(LeafLittervec)[dayi]=Litter * 0.4;  */ /* Collecting the leaf litter */       
				 }
				  if(kStem >= 0)
				  {
				  newStem =  dailyassim* kStem ;
					newSugar= dailyassim *kSugar;
					newFiber=dailyassim *kFiber;
				  }
				  else
				  {
					  warning("kstem is negative");
				  }
                
				  if(TT12c < SeneStem)
				  {
				   Stem += newStem;
				   Fiber+=newFiber;
				   Sugar+=newSugar;
				  }
				  else
				  {
				       Stem=Stem+newStem;  
				       Fiber=Fiber+newFiber;
				       Sugar=Sugar+newSugar;
					  
				  }
           if(kRoot >= 0)
		      {           
					 newRoot =  dailyassim * kRoot ;
      					 if(newRoot>=dailyRootResp)
      					  {
      					   newRoot=newRoot;
                    *(sti3+i) = newRoot;
      					  }
      					else
      					{
      					  newRoot=0.0;
      					  newSugar=newRoot-dailyRootResp; //Stem growth can not be negative. Set it zero and rest is taken care by sugar
                }
			 
		      }
				 else
				 {
					  error("kRoot should be positive");
				 }

	
				 if(TT12c < SeneRoot)
				 {
				   Root += newRoot;
           rootlitter=0.0;
				   REAL(RootLittervec)[dayi]= rootlitter;
				  
				 }
				 else
				 {
				    deadroot=(rootrate*Root)/100.0;  /* biomass of dead root */
				    Root =Root+newRoot-deadroot;
				    rootlitter=rootlitter+deadroot;
				    REAL(RootLittervec)[dayi]= rootlitter; 
				 }
	
				 if(kSeedcane > 0)
		                    {
			            newSeedcane = dailyassim * kSeedcane ;	
				           if(newSeedcane>0)
				             {
					      *(sti4+ri) = newSeedcane;
				             }
					   else
					   {
						   newSeedcane=0;
					   }
			            ri++;
				  }
				 else
				 {
				  if(Seedcane < 0){
					             Seedcane = 1e-4;
					             warning("Seedcane became negative");
				                  }

			        newSeedcane = Seedcane * kSeedcane;
			        Root += kRoot * -newSeedcane ;
			        Stem += kStem * -newSeedcane ;
				       Leaf += kLeaf * -newSeedcane ;
	                       Seedcane+=newSeedcane;	
				 }
                
				 /*	 newSugar=newStem*dbpS.kSugar;
					 Sugar=Sugar+newSugar;   */
				 	
				 ALitter = REAL(LeafLittervec)[i] + REAL(StemLittervec)[i];
				 BLitter = REAL(RootLittervec)[i];


		
		/*daily update of specific leaf area*/
	        Sp = Sp - REAL(SPD)[0];
                if (Sp < 0.5)Sp=0.5;

		/* new value of leaf area index */
		LAInew = Leaf * Sp ;
    
    if(wsFun!=3){
      LAI=(LAInew-LAIold)*LeafWS+LAIold;
    }
    else {
      LAI=LAInew;
    }
 
    
    
		/* apply leaf water stress based on rainfed (wsFun=0,1,2) or irrigated system (wsFun=3)*/

		//              if(wsFun ==3)
		//		{ 
		//		 LAI=LAInew;
		//		}
		//		else
		//		{ 
		//		 LAI=LAIold+(LAInew-LAIold)*LeafWS;
		//		}
		//		LAIold=LAI;

		  // Implementing LeafWS on Leaf Area  Index
     //LAI=LAInew;		  

		/* daily update of leaf-N, vmac, and alpga  based on cyclic seasonal variation */
		currentday=*(pt_doy+i);
		LeafN=seasonal(LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,365.0,lat);
      
      /**This calculation is not needed because LeafN is used to calculate photosynthesis paraetrers in canopy module*********/ 
      /*
      if(nitroparms.lnFun == 0){
					vmax1 = vmax1;
					Rd1=Rd1;
					alpha1=alpha1;
				}else{
					vmax1=nitroparms.Vmaxb1*LeafN+nitroparms.Vmaxb0;
					if(vmax1<0) vmax1=0.0;
					alpha1=nitroparms.alphab1*LeafN+nitroparms.alphab0;
					Rd1=nitroparms.Rdb1*LeafN+nitroparms.Rdb0;
				   }
      ******************************************************************************/

                    // Here, I am checking for maximum LAI based on minimum SLN = 50 is required
		    // If LAI > LAI max then additional senescence of leaf take place
		    laimax=(-1)*log((40.0/LeafN))/(kpLN);
				 if(LAI>laimax)
				 {
				LAI=laimax;
				deadleaf=(LAI-laimax)/Sp;  /* biomass of dead leaf */
        remobilizedleaf<-deadleaf*Remobfactorleaf;
  			leaflitter=leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
				Leaf =Leaf-deadleaf+remobilizedleaf*33;
        Stem=Stem+remobilizedleaf*0.33;
        Root=Root+remobilizedleaf*0.33;
				 }
         // Here I am simulating leaf Frost damage
         if((*pt_temp+i)<frostparms.leafT0)
         {
         deadleaf=getFrostdamage(frostparms.leafT0,frostparms.leafT100,*(pt_temp+i), Leaf);
         remobilizedleaf<-deadleaf*Remobfactorleaf;
    		 leaflitter=leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
				 Leaf =Leaf-deadleaf+remobilizedleaf*0.0;
         Stem=Stem+remobilizedleaf*0.50;
         Root=Root+remobilizedleaf*0.50;
         }

		/*reset daily assimilation and daily transpiratin to zero for the next day */
               	dailyassim=0.0;
	        dailytransp=0.0;

		/*reset daily maintenance respiration to zero for the next day */
		 dailyLeafResp = 0.0;
		 dailyStemResp = 0.0;
		 dailyRootResp = 0.0;
		 dailyRhizomeResp = 0.0;
            
		REAL(DayofYear)[dayi] =  INTEGER(DOY)[i];         
	        REAL(Leafy)[dayi] = Leaf;
		REAL(Stemy)[dayi] = Stem;
		REAL(Rooty)[dayi] =  Root;
		REAL(Seedcaney)[dayi] = Seedcane;
		REAL(LAIc)[dayi] = LAI;
		REAL(LeafNitrogen)[dayi] = LeafN;
		REAL(AboveLitter)[dayi] = ALitter;
		REAL(BelowLitter)[dayi] = BLitter;
		REAL(VmaxVec)[dayi] = vmax1;
		REAL(AlphaVec)[dayi] = alpha1;
		REAL(SpVec)[dayi] = Sp;
		REAL(Fibery)[dayi]=Fiber;
		REAL(Sugary)[dayi]=Sugar;
		ddap=(double)dayi;
		REAL(dayafterplanting)[dayi]=ddap;
		moist=(5.72*Stem-7.16*Sugar);
		REAL(moisturecontent)[dayi]=moist;
        	dayi=dayi+1;
		} /*daily loop ends here */
		
		

		/* here I am writing REMAINING variables at hourly time step */
		REAL(Hour)[i] =  INTEGER(HR)[i];
		REAL(CanopyAssim)[i] =  CanopyA;
		REAL(CanopyTrans)[i] =  CanopyT; 
		REAL(SoilWatCont)[i] = waterCont;
		REAL(StomatalCondCoefs)[i] = StomWS;
		REAL(LeafReductionCoefs)[i] = LeafWS;
		REAL(SoilEvaporation)[i] = soilEvap;
		REAL(LeafPsimVec)[i] = LeafPsim;
      

	}



	SET_VECTOR_ELT(lists,0,DayofYear);
	SET_VECTOR_ELT(lists,1,Hour);
	SET_VECTOR_ELT(lists,2,CanopyAssim);
	SET_VECTOR_ELT(lists,3,CanopyTrans);
	SET_VECTOR_ELT(lists,4,Leafy);
	SET_VECTOR_ELT(lists,5,Stemy);
	SET_VECTOR_ELT(lists,6,Rooty);
	SET_VECTOR_ELT(lists,7,Sugary);
	SET_VECTOR_ELT(lists,8,Fibery);
	SET_VECTOR_ELT(lists,9,Seedcaney);
	SET_VECTOR_ELT(lists,10,LAIc);
	SET_VECTOR_ELT(lists,11,TTTc);
	SET_VECTOR_ELT(lists,12,SoilWatCont);
	SET_VECTOR_ELT(lists,13,StomatalCondCoefs);
	SET_VECTOR_ELT(lists,14,LeafReductionCoefs);
	SET_VECTOR_ELT(lists,15,LeafNitrogen);
	SET_VECTOR_ELT(lists,16,AboveLitter);
	SET_VECTOR_ELT(lists,17,BelowLitter);
	SET_VECTOR_ELT(lists,18,VmaxVec);
	SET_VECTOR_ELT(lists,19,AlphaVec);
	SET_VECTOR_ELT(lists,20,SpVec);
	SET_VECTOR_ELT(lists,21,SoilEvaporation);
	SET_VECTOR_ELT(lists,22,cwsMat);
	SET_VECTOR_ELT(lists,23,psimMat);
	SET_VECTOR_ELT(lists,24,rdMat);
	SET_VECTOR_ELT(lists,25,LeafPsimVec);
	SET_VECTOR_ELT(lists,26,LeafLittervec);
	SET_VECTOR_ELT(lists,27,StemLittervec);
	SET_VECTOR_ELT(lists,28,RootLittervec);
	SET_VECTOR_ELT(lists,29,Leafmaintenance);
	SET_VECTOR_ELT(lists,30,Stemmaintenance);
	SET_VECTOR_ELT(lists,31,Rootmaintenance);
	SET_VECTOR_ELT(lists,32,Seedcanemaintenance);
	SET_VECTOR_ELT(lists,33,kkleaf);
	SET_VECTOR_ELT(lists,34,kkroot);
	SET_VECTOR_ELT(lists,35,kkstem);
	SET_VECTOR_ELT(lists,36,kkseedcane);
	SET_VECTOR_ELT(lists,37,kkfiber);
        SET_VECTOR_ELT(lists,38,kksugar);
	SET_VECTOR_ELT(lists,39,dailyThermalT);
        SET_VECTOR_ELT(lists,40,moisturecontent);
        SET_VECTOR_ELT(lists,41,dayafterplanting);


       

	SET_STRING_ELT(names,0,mkChar("DayofYear"));
	SET_STRING_ELT(names,1,mkChar("Hour"));
	SET_STRING_ELT(names,2,mkChar("CanopyAssim"));
	SET_STRING_ELT(names,3,mkChar("CanopyTrans"));
	SET_STRING_ELT(names,4,mkChar("Leaf"));
	SET_STRING_ELT(names,5,mkChar("Stem"));
	SET_STRING_ELT(names,6,mkChar("Root"));
	SET_STRING_ELT(names,7,mkChar("Sugar"));
	SET_STRING_ELT(names,8,mkChar("Fiber"));
	SET_STRING_ELT(names,9,mkChar("Seedcane"));
	SET_STRING_ELT(names,10,mkChar("LAI"));
	SET_STRING_ELT(names,11,mkChar("ThermalT"));
	SET_STRING_ELT(names,12,mkChar("SoilWatCont"));
	SET_STRING_ELT(names,13,mkChar("StomatalCondCoefs"));
	SET_STRING_ELT(names,14,mkChar("LeafReductionCoefs"));
	SET_STRING_ELT(names,15,mkChar("LeafNitrogen"));
	SET_STRING_ELT(names,16,mkChar("AboveLitter"));
	SET_STRING_ELT(names,17,mkChar("BelowLitter"));
	SET_STRING_ELT(names,18,mkChar("VmaxVec"));
	SET_STRING_ELT(names,19,mkChar("AlphaVec"));
	SET_STRING_ELT(names,20,mkChar("SpVec"));
	SET_STRING_ELT(names,21,mkChar("SoilEvaporation"));
	SET_STRING_ELT(names,22,mkChar("cwsMat"));
	SET_STRING_ELT(names,23,mkChar("psimMat"));
	SET_STRING_ELT(names,24,mkChar("rdMat"));
	SET_STRING_ELT(names,25,mkChar("LeafPsimVec"));
	SET_STRING_ELT(names,26,mkChar("LeafLittervec"));
	SET_STRING_ELT(names,27,mkChar("StemLittervec"));
	SET_STRING_ELT(names,28,mkChar("RootLittervec"));
	SET_STRING_ELT(names,29,mkChar("Leafmaintenance"));
	SET_STRING_ELT(names,30,mkChar("Stemmaintenance"));
	SET_STRING_ELT(names,31,mkChar("Rootmaintenance"));
	SET_STRING_ELT(names,32,mkChar("Seedcanemaintenance"));
	SET_STRING_ELT(names,33,mkChar("kkleaf"));
	SET_STRING_ELT(names,34,mkChar("kkroot"));
	SET_STRING_ELT(names,35,mkChar("kkstem"));
	SET_STRING_ELT(names,36,mkChar("kkseedcane"));
	SET_STRING_ELT(names,37,mkChar("kkfiber"));
	SET_STRING_ELT(names,38,mkChar("kksugar"));
	SET_STRING_ELT(names,39,mkChar("dailyThermalT"));
  SET_STRING_ELT(names,40,mkChar("moisturecontent"));
  SET_STRING_ELT(names,41,mkChar("dayafterplanting"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(44);  /* 34= 32+2, 2 comes from the very first two PROTECT statement for variable list and name */

	free(newLeafcol);
	free(newStemcol);
	free(newRootcol);
	free(newRhizomecol);
	return(lists);
}


	
