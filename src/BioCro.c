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
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"

SEXP MisGro(SEXP LAT,                 /* Latitude                  1 */ 
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
            SEXP ROOTFRONTVELOCITY
	)
{

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
        double gRespCoeff=0.242;/*This is constant fraction to calculate Growth respiration from DSSAT*/
        double Qleaf=1.58, Qstem=1.80, Qroot=1.80; /* Q factors for Temp dependence on Maintenance Respiration*/
	double mRespleaf=0.012,mRespstem=0.004,mResproot=0.0088;
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
	double dailyRootResp = 0.0;
	double dailyRhizomeResp = 0.0;
	double LeafResp,StemResp,RootResp,RhizomeResp,Litter;
	double LAIi;
	double delTT;
	double dap;

	double rootlitter,deadroot,leaflitter,deadleaf,leafREMOB;
	double Fiber,newFiber;
	double kSugar,kFiber,SeedcaneResp,Seedcane,newSeedcane,dailySeedcaneResp;
	double LAIold,LAInew;
	leafREMOB=REAL(LEAFREMOBILIZE)[0];
	int dayi=0; /* this is idnex for days */
            LAIi = Rhizome * propLeaf*Sp;
	    LAIold=LAIi; /* Enter simulation with LAIold variable to track precious LAI to implement water stress */
	for(i=0;i<vecsize;i++)
	{
		/* First calculate the elapsed Thermal Time*/
		/* The idea is that here I need to divide by the time step
		   to calculate the thermal time. For example, a 3 hour time interval
		   would mean that the division would need to by 8 */
		TTc += *(pt_temp+i) / (24/timestep); 
                if((*(pt_temp+i))<12)
                {
			TT12c = TT12c;
			delTT=0.0;
		}
               
		if((*(pt_temp+i))>= 12)
                {
			TT12c = TT12c + (*(pt_temp+i)-12) / (24/timestep);
			delTT=(*(pt_temp+i)- 12)/24.0;
		}
                
/* This calculated TTTc based on 0 base Temperature */    
/*		REAL(TTTc)[i] = TTc;

/* Now we are calculating TTc based on 12 base Temperature */
                REAL(TTTc)[i]=TT12c;
	     
	        /*  Do the magic! Calculate growth*/
		Canopy = CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,
                               LeafN, kpLN, lnb0, lnb1, lnfun,upperT,lowerT);

/* This is an addition, which was omitted earlier */
/* WIMOVAC suggestsevaluating A grodd = Anet + Rd before proceeding with further calculations */

/* Here, I am calling CanAC again with zero PAR. This, theoretically should simulate dark respiration  (due to Rd term) and give us a negative CanAC. I will add this value to net assimilation rate of canopy to obtain gross canopy assimilation. */

           	CanopyRd = CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       0.0, *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,
                               LeafN, kpLN, lnb0, lnb1, lnfun,upperT,lowerT);

		Canopy.Assim = Canopy.Assim-CanopyRd.Assim; // I am subtracting because CanopyRd.Assim is less than zero

/* A better way to evaluate gross canopy assimilation would be to evaluate for individual layer in CanAC functions using leaf temperature. However, this will require changing CanAC functon, which I do not want to distrub for now.

		/* Collecting the results */
		CanopyA = Canopy.Assim * timestep;
		CanopyT = Canopy.Trans * timestep;

                /* summing up hourly results to obtain daily assimilation */
		 dailyassim =  dailyassim + CanopyA;
	         dailytransp = dailytransp + CanopyT;
		 /* updated value of daily assimilation and transpiration */

		 /*Evaluate maintenance Respiration of different plant organs */
		   LeafResp=MRespiration(Leaf, Qleaf, mRespleaf, *(pt_temp+i), timestep);
		   REAL(Leafmaintenance)[i]=LeafResp;
		   StemResp=MRespiration(Fiber, Qstem, mRespstem, *(pt_temp+i), timestep);
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
					 INTEGER(HYDRDIST)[0], rfl, rsec, rsdf,optiontocalculaterootdepth,rootfrontvelocity,dap);

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

		/* Picking the dry biomass partitioning coefficients */
/*		dbpS = sel_dbp_coef(REAL(DBPCOEFS), REAL(THERMALP), TT12c); */
		/*	dbpS=SUGARCANE_DBP(TT12c); */
		/*See Details to check arguments ot CUADRA biomass partitionign function */

		/* A new day starts here */
		if( (i % 24) ==0) 
		{

		/* daily loss of assimilated CO2 via growth respiration */
	       	/* Here I am calling function to calculate Growth Respiration. Currently Using a fraction of 0.242 is lost via growth respiration. This fraction is taken from DSSAT                 4.5 manual for Sugarcane: parameter RespGcf *****/
		  dailyassim=GrowthRespiration(dailyassim, gRespCoeff); /* Plant growth continues during day and night*/

		/* daily loss via  maintenance respirations of different plant organs */
/*		  dailyassim=dailyassim-dailyLeafResp-dailyStemResp- dailyRootResp- dailyRhizomeResp;  *//* This is available photosynthate available for plant growth */

		  
		/* call the biomass partitioning function */

		  dbpS = SUGARCANE_DBP_CUADRA(TT12c,TT0,TTseed,Tmaturity,Rd,Alm,Arm,Clstem,Ilstem,Cestem,Iestem,Clsuc,Ilsuc,Cesuc,Iesuc,*(pt_temp+i));
	
		  
		  /*         dbpS.kLeaf=0.001;
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
					if(newLeaf>=dailyLeafResp)
					{
					 newLeaf=newLeaf-dailyLeafResp;
					}
					else
					{
					  newLeaf=0.0;
                                        }

					
                                       /*                      newLeaf = CanopyA * kLeaf * LeafWS ;         

				      /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
			              Plant. Phys. 46, 233-235. For this the water stress coefficient is different
			              for leaf and vmax. */
			              /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
			                /* The 0.02 and 0.03 are constants here but vary depending on species
			                 as pointed out in that reference. */

                                        /*			newLeaf = resp(newLeaf, mrc1, *(pt_temp+i));  */
                       
			                       if(newLeaf>0) 
					        {
		      	                       *(sti+i) = newLeaf; 
                                                } 
					                                                     
		                  }
				  else
				  {

					  newLeaf = Leaf * kLeaf ; /* This is not suppposed to happen for sugarcane */
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
				 leaflitter=leaflitter+deadleaf; /*40% of dead leaf goes to leaf litter */
				 Leaf =Leaf+newLeaf-deadleaf;
				 REAL(LeafLittervec)[dayi]= leaflitter; /* Collecting the leaf litter */ 
				 /*	 REAL(LeafLittervec)[dayi]=Litter * 0.4;  */ /* Collecting the leaf litter */       
				 }

				

				  if(kStem >= 0)
				  {
				        newStem =  dailyassim * kStem ;
					newSugar= dailyassim *kSugar;
					newFiber=dailyassim *kFiber;
					if(newStem>=dailyStemResp)
					{
					 newStem=newStem-dailyStemResp;
					}
					else
					{
					  newStem=0.0;
					  	newSugar= 0;
						newFiber=0;
					  
					  //  newSugar=newStem-dailyStemResp; //Stem growth can not be negative. Set it zero and rest is taken care by sugar
                                        }

					  /***
					  else
					  {
						  newStem=0; 
					  }
					  ***/
				  }
				  else
				  {
					  error("kStem should be positive");
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
					 newRoot=newRoot-dailyRootResp;
					}
					else
					{
					  newRoot=0.0;
					  newSugar=newRoot-dailyRootResp; //Stem growth can not be negative. Set it zero and rest is taken care by sugar
                                        }
			                   if(newRoot>0)
			                   {
		                            *(sti3+i) = newRoot;
			                   }
					   /***
					   else
					   {
						   newRoot=0;
				            }
					   ***/
		                 }
				 else
				 {
					 newRoot = Root * kRoot ;
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
	                       newSeedcane=newSeedcane-dailySeedcaneResp;	
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
		/* apply leaf water stress based on rainfed (wsFun=0,1,2) or irrigated system (wsFun=3)*/

                                if(wsFun ==3)
				{ 
				 LAI=LAInew;
				}
				else
				{ 
				 LAI=LAIold+(LAInew-LAIold)*LeafWS;
				}
				LAIold=LAI;


		  // Implementing LeafWS on Leaf Area  Index
		  

		/* daily update of leaf-N, vmac, and alpga  based on cyclic seasonal variation */
		currentday=*(pt_doy+i);
		LeafN=seasonal(LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,365.0,lat);
		vmax1=0.971*LeafN-39;
		if(vmax1<0) vmax1=0.0;
		alpha1=0.000488*LeafN+0.02368;  

                    // Here, I am checking for maximum LAI based on minimum SLN = 50 is required
		    // If LAI > LAI max then additional senescence of leaf take place
		laimax=(-1)*log((30.0/LeafN))/(kpLN);
				 if(LAI>laimax)
				 {
					 LAI=laimax;
					 deadleaf=(LAI-laimax)/Sp;  /* biomass of dead leaf */
				         Leaf=Leaf-deadleaf;
					 leaflitter=leaflitter+deadleaf;
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
        SET_VECTOR_ELT(lists,40,mkChar("moisturecontent"));
        SET_VECTOR_ELT(lists,41,mkChar("dayafterplanting"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(44);  /* 34= 32+2, 2 comes from the very first two PROTECT statement for variable list and name */

	free(newLeafcol);
	free(newStemcol);
	free(newRootcol);
	free(newRhizomecol);

	return(lists);
}


/* HERE I AM ADDING THE SIMULATED ANNEALING FUNCTION */
/* First pass the observed data and then the simulated */

SEXP SABioGro(SEXP oTHERMAL, SEXP oSTEM, SEXP oLEAF, 
	      SEXP oROOT, SEXP oRHIZOME, 
	      SEXP oGRAIN, SEXP oLAI,
	      SEXP DOY, SEXP HR, SEXP SOLAR, 
              SEXP TEMP, SEXP RH, SEXP WINDSPEED, 
	      SEXP PRECIP, SEXP DBPCOEF, SEXP VECSIZE, 
	      SEXP LAT, SEXP NLAYERS, SEXP RHIZOMEIRTL, 
	      SEXP SENESCTIME, SEXP TIMESTEP, 
	      SEXP VMAX, SEXP ALPHA, SEXP KPARM, 
	      SEXP THETA, SEXP BETA, SEXP RD, 
	      SEXP CATM, SEXP B0, SEXP B1, 
	      SEXP SOILCOEFS, SEXP WSFUN, SEXP WS, 
	      SEXP ILEAFN, SEXP KLN, SEXP VMAXB1, 
	      SEXP ALPHAB1, SEXP MRESP, SEXP SOILTYPE, 
	      SEXP CENTCOEFS, SEXP CENTKS, SEXP CENTTIMESTEP, 
	      SEXP KD, SEXP CHILHF, SEXP SP, 
	      SEXP SPD, SEXP THERMALP, SEXP INDEX, 
	      SEXP NDATA, SEXP N1DAT, SEXP NITER, 
	      SEXP NITER2, SEXP SATEMP, SEXP COOLSAMP, 
	      SEXP SCALE, SEXP SD, SEXP PHEN, 
	      SEXP SOILLAYERS, SEXP SOILDEPTHS, 
	      SEXP CWS, SEXP HYDRDIST, SEXP SECS, 
	      SEXP NCOEFS, SEXP LNFUN, SEXP UPPERTEMP, SEXP LOWERTEMP)
{


	/* External variables */
	extern double CanopyAssim[8760] ;
	extern double Leafy[8760] ;
	extern double Stemy[8760] ;
	extern double Rooty[8760] ;
	extern double Rhizomey[8760] ;
	extern double Grainy[8760] ;
	extern double LAIc[8760] ;
	/* Index variables */
	int j,k,m;
	int niter, niter2, iters = 0, iters2 = 0;
	int accept = 0;
	int n1 = 0, n2 = 0;
	double upperT=REAL(UPPERTEMP)[0];
	double lowerT=REAL(LOWERTEMP)[0];


	/* The all important vector size */
	int vecsize;
	vecsize = INTEGER(VECSIZE)[0];
	int Ndat;
	int n1dat = 0;
	Ndat = INTEGER(NDATA)[0];
	niter = INTEGER(NITER)[0];
	niter2 = INTEGER(NITER2)[0];
	double saTemp;
	int coolSamp;
	double scale;
	int phen;


	saTemp = REAL(SATEMP)[0];
	coolSamp = INTEGER(COOLSAMP)[0];
	scale = REAL(SCALE)[0];
	phen = INTEGER(PHEN)[0];

	int timestep;
	double b0, b1;
	double vmaxb1, alphab1;
	double LeafN, kLN, Ca;

	b0 = REAL(B0)[0];
	b1 = REAL(B1)[0];
	timestep = INTEGER(TIMESTEP)[0];

	alphab1 = REAL(ALPHAB1)[0];
	vmaxb1 = REAL(VMAXB1)[0];

	LeafN = REAL(ILEAFN)[0];
	kLN = REAL(KLN)[0];

	/* Needed variables */
	double lati, rhizome, irtl;
	int nlayers;
	/* Creating vectors */
	double dbpcoef[25];
	/* Picking the simulation */
	int ind;
	double sCanopyAssim[Ndat], sStemy[Ndat], sLeafy[Ndat];
	double sRhizomey[Ndat], sRooty[Ndat], sGrainy[Ndat],  sLAIy[Ndat];
	double oStemy[Ndat], oLeafy[Ndat];
	double oRhizomey[Ndat], oRooty[Ndat], oGrainy[Ndat], oLAIy[Ndat];
	/* more vairbles */
	double rss = 0.0, oldRss;
	double U , mr;

	/* Yet more */
	double index = 0;
	double sd1 = REAL(SD)[0];
	double sd2 = REAL(SD)[1];
	double sd3 = REAL(SD)[2];
	double sd4 = REAL(SD)[3];

	double kLeaf_1, kStem_1, kRoot_1, kRhizome_1;
	double kLeaf_2, kStem_2, kRoot_2, kRhizome_2;
	double kLeaf_3, kStem_3, kRoot_3, kRhizome_3;
	double kLeaf_4, kStem_4, kRoot_4, kRhizome_4;
	double kLeaf_5, kStem_5, kRoot_5, kRhizome_5;
	double kLeaf_6, kStem_6, kRoot_6, kRhizome_6, kGrain_6;
	double k1, k2, k3, k4, k5, k6;
	double vmax, alpha, kparm, theta, beta, Rd;
	double oldkLeaf_1, oldkStem_1, oldkRhizome_1, oldkRoot_1;
	double oldkLeaf_2, oldkStem_2, oldkRhizome_2, oldkRoot_2;
	double oldkLeaf_3, oldkStem_3, oldkRhizome_3, oldkRoot_3;
	double oldkLeaf_4, oldkStem_4, oldkRhizome_4, oldkRoot_4;
	double oldkLeaf_5, oldkStem_5, oldkRhizome_5, oldkRoot_5;
	double oldkLeaf_6, oldkStem_6, oldkRhizome_6, oldkRoot_6, oldkGrain_6;
	/* Handling answers to pass to R*/

	SEXP lists;
	SEXP names;

	SEXP ans1;

	SEXP RSS;
	SEXP ACCPT;
	SEXP RssVec;
	SEXP RssVec2;

	SEXP simStem;
	SEXP obsStem;

	SEXP simLeaf;
	SEXP obsLeaf;

	SEXP simRhiz;
	SEXP obsRhiz;

	SEXP simRoot;
	SEXP obsRoot;

	SEXP simGrain;
	SEXP obsGrain;

	SEXP simLAI;
	SEXP obsLAI;

	SEXP TTime;
	SEXP inde;

	SEXP mat1;
	SEXP accept2;
	SEXP accept3;
	SEXP SAtemp;

	PROTECT(lists = allocVector(VECSXP,23)); /* 1 */
	PROTECT(names = allocVector(STRSXP,23)); /* 2 */

	PROTECT(ans1 = allocVector(REALSXP,25)); /* 3 */
	PROTECT(RSS = allocVector(REALSXP,1)); /* 4 */
	PROTECT(ACCPT = allocVector(REALSXP,1)); /* 5 */
	PROTECT(RssVec = allocVector(REALSXP,niter)); /* 6 */
	PROTECT(RssVec2 = allocVector(REALSXP,niter2)); /* 7 */

	PROTECT(simStem = allocVector(REALSXP,Ndat)); /* 8 */
	PROTECT(obsStem = allocVector(REALSXP,Ndat)); /* 9 */

	PROTECT(simLeaf = allocVector(REALSXP,Ndat)); /* 10 */
	PROTECT(obsLeaf = allocVector(REALSXP,Ndat)); /* 11 */

	PROTECT(simRhiz = allocVector(REALSXP,Ndat)); /* 12 */
	PROTECT(obsRhiz = allocVector(REALSXP,Ndat)); /* 13 */

	PROTECT(simRoot = allocVector(REALSXP,Ndat)); /* 14 */
	PROTECT(obsRoot = allocVector(REALSXP,Ndat)); /* 15 */

	PROTECT(simGrain = allocVector(REALSXP,Ndat)); /* 16 */
	PROTECT(obsGrain = allocVector(REALSXP,Ndat)); /* 17 */

	PROTECT(simLAI = allocVector(REALSXP,Ndat)); /* 18 */
	PROTECT(obsLAI = allocVector(REALSXP,Ndat)); /* 19 */

	PROTECT(TTime = allocVector(REALSXP,Ndat)); /* 20 */
	PROTECT(inde = allocVector(REALSXP,Ndat)); /* 21 */

	PROTECT(mat1 = allocMatrix(REALSXP,25,niter2)); /* 22 */
	PROTECT(accept2 = allocVector(REALSXP,1)); /* 23 */
	PROTECT(accept3 = allocVector(REALSXP,1)); /* 24 */
	PROTECT(SAtemp = allocVector(REALSXP,1)); /* 25 */

	/* Applying the scale to the sd */
	sd1 *= scale;
	sd2 *= scale;
	sd3 *= scale;
	sd4 *= scale;

	/* Picking them */
	lati = REAL(LAT)[0];
	rhizome = REAL(RHIZOMEIRTL)[0];
	irtl = REAL(RHIZOMEIRTL)[1];
	nlayers = INTEGER(NLAYERS)[0];
	Rd = REAL(RD)[0];
	Ca = REAL(CATM)[0];
	vmax = REAL(VMAX)[0];
	alpha = REAL(ALPHA)[0];
	kparm = REAL(KPARM)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];

	/* Pick the observed */
	for(m=0;m<Ndat;m++){
		oStemy[m] = REAL(oSTEM)[m];
		REAL(obsStem)[m] = REAL(oSTEM)[m];
		oLeafy[m] = REAL(oLEAF)[m];
		REAL(obsLeaf)[m] = REAL(oLEAF)[m];
		oRhizomey[m] = REAL(oRHIZOME)[m];
		REAL(obsRhiz)[m] = REAL(oRHIZOME)[m];
		oRooty[m] = REAL(oROOT)[m];
		REAL(obsRoot)[m] = REAL(oROOT)[m];
		oGrainy[m] = REAL(oGRAIN)[m];
		REAL(obsGrain)[m] = REAL(oGRAIN)[m];
		oLAIy[m] = REAL(oLAI)[m];
		REAL(obsLAI)[m] = REAL(oLAI)[m];
	}

	oldkLeaf_1 = REAL(DBPCOEF)[0];
	oldkStem_1 = REAL(DBPCOEF)[1];
	oldkRoot_1 = REAL(DBPCOEF)[2];
	oldkRhizome_1 = REAL(DBPCOEF)[3];

	oldkLeaf_2 = REAL(DBPCOEF)[4];
	oldkStem_2 = REAL(DBPCOEF)[5];
	oldkRoot_2 = REAL(DBPCOEF)[6];
	oldkRhizome_2 = REAL(DBPCOEF)[7];

	oldkLeaf_3 = REAL(DBPCOEF)[8];
	oldkStem_3 = REAL(DBPCOEF)[9];
	oldkRoot_3 = REAL(DBPCOEF)[10];
	oldkRhizome_3 = REAL(DBPCOEF)[11];

	oldkLeaf_4 = REAL(DBPCOEF)[12];
	oldkStem_4 = REAL(DBPCOEF)[13];
	oldkRoot_4 = REAL(DBPCOEF)[14];
	oldkRhizome_4 = REAL(DBPCOEF)[15];

	oldkLeaf_5 = REAL(DBPCOEF)[16];
	oldkStem_5 = REAL(DBPCOEF)[17];
	oldkRoot_5 = REAL(DBPCOEF)[18];
	oldkRhizome_5 = REAL(DBPCOEF)[19];

	oldkLeaf_6 = REAL(DBPCOEF)[20];
	oldkStem_6 = REAL(DBPCOEF)[21];
	oldkRoot_6 = REAL(DBPCOEF)[22];
	oldkRhizome_6 = REAL(DBPCOEF)[23];
	oldkGrain_6 = REAL(DBPCOEF)[24];

	GetRNGstate();

	oldRss = 1e6;
	/* This is a good part to start the optimization */

        /* determining the n1dat */
	n1dat = INTEGER(N1DAT)[phen-1];

	for(iters=0;iters<niter;iters++){

        /* Selecting the index to sample */

		index = sel_phen(phen);
	
	/* First phenological stage */
	
	        kLeaf_1 = fabs(oldkLeaf_1);
		kStem_1 = fabs(oldkStem_1);
		kRoot_1 = fabs(oldkRoot_1);


		if(index < 1){
			kLeaf_1 = fabs(rnorm(kLeaf_1,sd1));
		}else
			
			if(index < 2){
				kStem_1 = fabs(rnorm(kStem_1,sd1));
			}else

				if(index < 3){
					kRoot_1 = fabs(rnorm(kRoot_1,sd1));
				}
    
		k1 = kLeaf_1 + kStem_1 + kRoot_1;
		kLeaf_1 = kLeaf_1/k1;
		kStem_1 = kStem_1/k1;
		kRoot_1 = kRoot_1/k1;
    
		if(index > 3 && index < 4){
			kRhizome_1 = -rnorm(fabs(oldkRhizome_1),sd2);
		}else{
			kRhizome_1 = -fabs(oldkRhizome_1);
		}

		/* Second phenological stage */

		kLeaf_2 = fabs(oldkLeaf_2);
		kStem_2 = fabs(oldkStem_2);
		kRoot_2 = fabs(oldkRoot_2);

		if(index > 4 && index < 5){
			kLeaf_2 = fabs(rnorm(kLeaf_2,sd1));
		}

		if(index > 5 && index < 6){
			kStem_2 = fabs(rnorm(kStem_2,sd1));
		}

		if(index > 6 && index < 7){
			kRoot_2 = fabs(rnorm(kRoot_2,sd1));
		}

		k2 = kLeaf_2 + kStem_2 + kRoot_2 ;
		kLeaf_2 =  kLeaf_2/k2 ;
		kStem_2 = kStem_2/k2 ;
		kRoot_2 = kRoot_2/k2 ;
    
		if(index > 7 && index < 8){
			kRhizome_2 =-rnorm(fabs(oldkRhizome_2),sd2);
		}else{
			kRhizome_2 =-fabs(oldkRhizome_2);
		}

		/* Third phenological stage */

		kLeaf_3 = fabs(oldkLeaf_3);
		kStem_3 = fabs(oldkStem_3);
		kRoot_3 = fabs(oldkRoot_3);
		kRhizome_3 = fabs(oldkRhizome_3);

		if(index > 8 && index < 9){
			kLeaf_3 = fabs(rnorm(kLeaf_3,sd1));
		}

		if(index > 9 && index < 10){
			kStem_3 = fabs(rnorm(kStem_3,sd1));
		}

		if(index > 10 && index < 11){
			kRoot_3 = fabs(rnorm(kRoot_3,sd1));
		}

		if(index > 11 && index < 12){
			kRhizome_3 = fabs(rnorm(kRhizome_3,sd1));
		}

		k3 = kLeaf_3 + kStem_3 + kRoot_3 + kRhizome_3;
		kLeaf_3 =  kLeaf_3/k3 ;
		kStem_3 = kStem_3/k3 ;
		kRoot_3 = kRoot_3/k3 ;
		kRhizome_3 = kRhizome_3/k3 ;
    
		/* Fourth phenological stage */

		kLeaf_4 = fabs(oldkLeaf_4);
		kStem_4 = fabs(oldkStem_4);
		kRoot_4 = fabs(oldkRoot_4);
		kRhizome_4 = fabs(oldkRhizome_4);

		if(index > 12 && index < 13){
			kLeaf_4 = fabs(rnorm(kLeaf_4,sd1));
		}

		if(index > 13 && index < 14){
			kStem_4 = fabs(rnorm(kStem_4,sd1));
		}

		if(index > 14 && index < 15){
			kRoot_4 = fabs(rnorm(kRoot_4,sd1));
		}

		if(index > 15 && index < 16){
			kRhizome_4 = fabs(rnorm(kRhizome_4,sd1));
		}

		k4 = kLeaf_4 + kStem_4 + kRoot_4 + kRhizome_4;
		kLeaf_4 =  kLeaf_4/k4 ;
		kStem_4 = kStem_4/k4 ;
		kRoot_4 = kRoot_4/k4 ;
		kRhizome_4 = kRhizome_4/k4 ;
 
		/* Fifth phenological stage */

		kLeaf_5 = fabs(oldkLeaf_5);
		kStem_5 = fabs(oldkStem_5);
		kRoot_5 = fabs(oldkRoot_5);
		kRhizome_5 = fabs(oldkRhizome_5);

		if(index > 16 && index < 17){
			kLeaf_5 = fabs(rnorm(kLeaf_5,sd1));
		}

		if(index > 17 && index < 18){
			kStem_5 = fabs(rnorm(kStem_5,sd1));
		}

		if(index > 18 && index < 19){
			kRoot_5 = fabs(rnorm(kRoot_5,sd1));
		}

		if(index > 19 && index < 20){
			kRhizome_5 = fabs(rnorm(kRhizome_5,sd1));
		}

		k5 = kLeaf_5 + kStem_5 + kRoot_5 + kRhizome_5;
		kLeaf_5 =  kLeaf_5/k5;
		kStem_5 = kStem_5/k5;
		kRoot_5 = kRoot_5/k5;
		kRhizome_5 = kRhizome_5/k5;

		/* Sixth phenological stage */

		kLeaf_6 = fabs(oldkLeaf_6);
		kStem_6 = fabs(oldkStem_6);
		kRoot_6 = fabs(oldkRoot_6);
		kRhizome_6 = fabs(oldkRhizome_6);
		kGrain_6 = fabs(oldkGrain_6);

		if(index > 20 && index < 21){
			kLeaf_6 = fabs(rnorm(kLeaf_6,sd1));
		}

		if(index > 21 && index < 22){
			kStem_6 = fabs(rnorm(kStem_6,sd1));
		}

		if(index > 22 && index < 23){
			kRoot_6 = fabs(rnorm(kRoot_6,sd1));
		}

		if(index > 23 && index < 24){
			kRhizome_6 = fabs(rnorm(kRhizome_6,sd1));
		}

		if(index > 24 && index < 25){
			kGrain_6 = fabs(rnorm(kGrain_6,sd1));
		}

		k6 = kLeaf_6 + kStem_6 + kRoot_6 + kRhizome_6 + kGrain_6;
		kLeaf_6 =  kLeaf_6 / k6;
		kStem_6 = kStem_6 / k6;
		kRoot_6 = kRoot_6 / k6;
		kRhizome_6 = kRhizome_6 / k6;
		kGrain_6 = kGrain_6 / k6;

/* Populating the dbp coefficients */
		dbpcoef[0] = kLeaf_1;
		dbpcoef[1] = kStem_1;
		dbpcoef[2] = kRoot_1;
		dbpcoef[3] = kRhizome_1;
		dbpcoef[4] = kLeaf_2;
		dbpcoef[5] = kStem_2;
		dbpcoef[6] = kRoot_2;
		dbpcoef[7] = kRhizome_2;
		dbpcoef[8] = kLeaf_3;
		dbpcoef[9] = kStem_3;
		dbpcoef[10] = kRoot_3;
		dbpcoef[11] = kRhizome_3;
		dbpcoef[12] = kLeaf_4;
		dbpcoef[13] = kStem_4;
		dbpcoef[14] = kRoot_4;
		dbpcoef[15] = kRhizome_4;
		dbpcoef[16] = kLeaf_5;
		dbpcoef[17] = kStem_5;
		dbpcoef[18] = kRoot_5;
		dbpcoef[19] = kRhizome_5;
		dbpcoef[20] = kLeaf_6;
		dbpcoef[21] = kStem_6;
		dbpcoef[22] = kRoot_6;
		dbpcoef[23] = kRhizome_6;
		dbpcoef[24] = kGrain_6;

		/* int p; */
		/* for(p = 0;p<25;p++){ */
		/* 	Rprintf("dbpcoef %.i %.3f \n",p,dbpcoef[p]); */
		/* } */

		BioGro(lati,INTEGER(DOY),INTEGER(HR),REAL(SOLAR),REAL(TEMP),REAL(RH),
		       REAL(WINDSPEED),REAL(PRECIP), REAL(KD)[0], REAL(CHILHF)[0], 
		       REAL(CHILHF)[1],nlayers,rhizome,
		       irtl,REAL(SENESCTIME),INTEGER(TIMESTEP)[0],vecsize,
		       REAL(SP)[0], REAL(SPD)[0], dbpcoef, REAL(THERMALP),
		       vmax,alpha,kparm,theta,beta,Rd,Ca,b0,b1, REAL(SOILCOEFS), LeafN, kLN,
		       vmaxb1, alphab1, REAL(MRESP), INTEGER(SOILTYPE)[0], INTEGER(WSFUN)[0],
		       INTEGER(WS)[0], REAL(CENTCOEFS),REAL(CENTKS), INTEGER(CENTTIMESTEP)[0],
		       INTEGER(SOILLAYERS)[0], REAL(SOILDEPTHS), REAL(CWS), INTEGER(HYDRDIST)[0], 
		       REAL(SECS), REAL(NCOEFS)[0], REAL(NCOEFS)[1], REAL(NCOEFS)[2], INTEGER(LNFUN)[0],upperT,lowerT);

		/* pick the needed elements for the SSE */
		for(k=0;k<Ndat;k++){
			ind = INTEGER(INDEX)[k];
			sCanopyAssim[k] = CanopyAssim[ind]; /* Rprintf("CanopyAssim %.i %.1f \n",ind,CanopyAssim[ind]); */
			sLeafy[k] = Leafy[ind]; /* Rprintf("Leafy %.i %.1f \n",ind,Leafy[ind]); */
			sStemy[k] = Stemy[ind]; /* Rprintf("Stemy %.i %.1f \n",ind,Stemy[ind]); */
			sRooty[k] = Rooty[ind]; /* Rprintf("Rooty %.i %.1f \n",ind,Rooty[ind]); */
			sRhizomey[k] = Rhizomey[ind]; /* Rprintf("Rhizomey %.i %.1f \n",ind,Rhizomey[ind]); */
			sGrainy[k] = Grainy[ind]; /* Rprintf("Grainy %.i %.1f \n",ind,Grainy[ind]); */
			sLAIy[k] = LAIc[ind]; /* Rprintf("LAIc %.i %.1f \n",ind,LAIc[ind]); */
		}


		rss = RSS_BG(oStemy,oLeafy,oRooty,oRhizomey,oGrainy,oLAIy,
			     sStemy,sLeafy,sRooty,sRhizomey,sGrainy,sLAIy,
			     n1dat);


		REAL(RssVec)[iters] = rss;
		U = runif(0,1);
		if(rss < (oldRss - saTemp *log(U))){
			accept++;
			oldRss = rss;
			if((accept%coolSamp)==0){
				saTemp *= 0.9 ;
			}
			/* accept the coefficients */
			oldkLeaf_1 = dbpcoef[0];
			oldkStem_1 = dbpcoef[1];
			oldkRoot_1 = dbpcoef[2];
			oldkRhizome_1 = dbpcoef[3];

			oldkLeaf_2 = dbpcoef[4];
			oldkStem_2 = dbpcoef[5];
			oldkRoot_2 = dbpcoef[6];
			oldkRhizome_2 = dbpcoef[7];

			oldkLeaf_3 = dbpcoef[8];
			oldkStem_3 = dbpcoef[9];
			oldkRoot_3 = dbpcoef[10];
			oldkRhizome_3 = dbpcoef[11];

			oldkLeaf_4 = dbpcoef[12];
			oldkStem_4 = dbpcoef[13];
			oldkRoot_4 = dbpcoef[14];
			oldkRhizome_4 = dbpcoef[15];

			oldkLeaf_5 = dbpcoef[16];
			oldkStem_5 = dbpcoef[17];
			oldkRoot_5 = dbpcoef[18];
			oldkRhizome_5 = dbpcoef[19];

			oldkLeaf_6 = dbpcoef[20];
			oldkStem_6 = dbpcoef[21];
			oldkRoot_6 = dbpcoef[22];
			oldkRhizome_6 = dbpcoef[23];
			oldkGrain_6 = dbpcoef[24];

		}
		/* It would be nice to put them in a matrix */

	}

	for(iters2=0;iters2<niter2;iters2++){

		index = sel_phen(phen);

		/* First phenological stage */

		kLeaf_1 = fabs(oldkLeaf_1);
		kStem_1 = fabs(oldkStem_1);
		kRoot_1 = fabs(oldkRoot_1);
        
		if(index < 1){
			kLeaf_1 = fabs(rnorm(kLeaf_1,sd1));
		}else
    
			if(index < 2){
				kStem_1 = fabs(rnorm(kStem_1,sd1));
			}else

				if(index < 3){
					kRoot_1 = fabs(rnorm(kRoot_1,sd1));
				}
    
		k1 = kLeaf_1 + kStem_1 + kRoot_1;
		kLeaf_1 = kLeaf_1/k1;
		kStem_1 = kStem_1/k1;
		kRoot_1 = kRoot_1/k1;
    
		if(index > 3 && index < 4){
			kRhizome_1 = -rnorm(fabs(oldkRhizome_1),sd2);
		}else{
			kRhizome_1 = -fabs(oldkRhizome_1);
		}

		/* Second phenological stage */

		kLeaf_2 = fabs(oldkLeaf_2);
		kStem_2 = fabs(oldkStem_2);
		kRoot_2 = fabs(oldkRoot_2);

		if(index > 4 && index < 5){
			kLeaf_2 = fabs(rnorm(kLeaf_2,sd1));
		}

		if(index > 5 && index < 6){
			kStem_2 = fabs(rnorm(kStem_2,sd1));
		}

		if(index > 6 && index < 7){
			kRoot_2 = fabs(rnorm(kRoot_2,sd1));
		}

		k2 = kLeaf_2 + kStem_2 + kRoot_2 ;
		kLeaf_2 =  kLeaf_2/k2 ;
		kStem_2 = kStem_2/k2 ;
		kRoot_2 = kRoot_2/k2 ;
    
		if(index > 7 && index < 8){
			kRhizome_2 =-rnorm(fabs(oldkRhizome_2),sd2);
		}else{
			kRhizome_2 =-fabs(oldkRhizome_2);
		}

		/* Third phenological stage */

		kLeaf_3 = fabs(oldkLeaf_3);
		kStem_3 = fabs(oldkStem_3);
		kRoot_3 = fabs(oldkRoot_3);
		kRhizome_3 = fabs(oldkRhizome_3);

		if(index > 8 && index < 9){
			kLeaf_3 = fabs(rnorm(kLeaf_3,sd1));
		}

		if(index > 9 && index < 10){
			kStem_3 = fabs(rnorm(kStem_3,sd1));
		}

		if(index > 10 && index < 11){
			kRoot_3 = fabs(rnorm(kRoot_3,sd1));
		}

		if(index > 11 && index < 12){
			kRhizome_3 = fabs(rnorm(kRhizome_3,sd1));
		}

		k3 = kLeaf_3 + kStem_3 + kRoot_3 + kRhizome_3;
		kLeaf_3 =  kLeaf_3/k3 ;
		kStem_3 = kStem_3/k3 ;
		kRoot_3 = kRoot_3/k3 ;
		kRhizome_3 = kRhizome_3/k3 ;
    
		/* Fourth phenological stage */

		kLeaf_4 = fabs(oldkLeaf_4);
		kStem_4 = fabs(oldkStem_4);
		kRoot_4 = fabs(oldkRoot_4);
		kRhizome_4 = fabs(oldkRhizome_4);

		if(index > 12 && index < 13){
			kLeaf_4 = fabs(rnorm(kLeaf_4,sd1));
		}

		if(index > 13 && index < 14){
			kStem_4 = fabs(rnorm(kStem_4,sd1));
		}

		if(index > 14 && index < 15){
			kRoot_4 = fabs(rnorm(kRoot_4,sd1));
		}

		if(index > 15 && index < 16){
			kRhizome_4 = fabs(rnorm(kRhizome_4,sd1));
		}

		k4 = kLeaf_4 + kStem_4 + kRoot_4 + kRhizome_4;
		kLeaf_4 =  kLeaf_4/k4 ;
		kStem_4 = kStem_4/k4 ;
		kRoot_4 = kRoot_4/k4 ;
		kRhizome_4 = kRhizome_4/k4 ;
 
		/* Fifth phenological stage */

		kLeaf_5 = fabs(oldkLeaf_5);
		kStem_5 = fabs(oldkStem_5);
		kRoot_5 = fabs(oldkRoot_5);
		kRhizome_5 = fabs(oldkRhizome_5);

		if(index > 16 && index < 17){
			kLeaf_5 = fabs(rnorm(kLeaf_5,sd1));
		}

		if(index > 17 && index < 18){
			kStem_5 = fabs(rnorm(kStem_5,sd1));
		}

		if(index > 18 && index < 19){
			kRoot_5 = fabs(rnorm(kRoot_5,sd1));
		}

		if(index > 19 && index < 20){
			kRhizome_5 = fabs(rnorm(kRhizome_5,sd1));
		}

		k5 = kLeaf_5 + kStem_5 + kRoot_5 + kRhizome_5;
		kLeaf_5 =  kLeaf_5/k5 ;
		kStem_5 = kStem_5/k5 ;
		kRoot_5 = kRoot_5/k5 ;
		kRhizome_5 = kRhizome_5/k5 ;

		/* Sixth phenological stage */

		kLeaf_6 = fabs(oldkLeaf_6);
		kStem_6 = fabs(oldkStem_6);
		kRoot_6 = fabs(oldkRoot_6);
		kRhizome_6 = fabs(oldkRhizome_6);
		kGrain_6 = fabs(oldkGrain_6);

		if(index > 20 && index < 21){
			kLeaf_6 = fabs(rnorm(kLeaf_6,sd1));
		}

		if(index > 21 && index < 22){
			kStem_6 = fabs(rnorm(kStem_6,sd1));
		}

		if(index > 22 && index < 23){
			kRoot_6 = fabs(rnorm(kRoot_6,sd1));
		}

		if(index > 23 && index < 24){
			kRhizome_6 = fabs(rnorm(kRhizome_6,sd1));
		}

		if(index > 24 && index < 25){
			kGrain_6 = fabs(rnorm(kGrain_6,sd1));
		}

		k6 = kLeaf_6 + kStem_6 + kRoot_6 + kRhizome_6 + kGrain_6;
		kLeaf_6 =  kLeaf_6/k6 ;
		kStem_6 = kStem_6/k6 ;
		kRoot_6 = kRoot_6/k6 ;
		kRhizome_6 = kRhizome_6/k6 ;
		kGrain_6 = kGrain_6/k6 ;

/* Populating the dbp coefficients */

		dbpcoef[0] = kLeaf_1;
		dbpcoef[1] = kStem_1;
		dbpcoef[2] = kRoot_1;
		dbpcoef[3] = kRhizome_1;
		dbpcoef[4] = kLeaf_2;
		dbpcoef[5] = kStem_2;
		dbpcoef[6] = kRoot_2;
		dbpcoef[7] = kRhizome_2;
		dbpcoef[8] = kLeaf_3;
		dbpcoef[9] = kStem_3;
		dbpcoef[10] = kRoot_3;
		dbpcoef[11] = kRhizome_3;
		dbpcoef[12] = kLeaf_4;
		dbpcoef[13] = kStem_4;
		dbpcoef[14] = kRoot_4;
		dbpcoef[15] = kRhizome_4;
		dbpcoef[16] = kLeaf_5;
		dbpcoef[17] = kStem_5;
		dbpcoef[18] = kRoot_5;
		dbpcoef[19] = kRhizome_5;
		dbpcoef[20] = kLeaf_6;
		dbpcoef[21] = kStem_6;
		dbpcoef[22] = kRoot_6;
		dbpcoef[23] = kRhizome_6;
		dbpcoef[24] = kGrain_6;
 
		BioGro(lati,INTEGER(DOY),INTEGER(HR),REAL(SOLAR),REAL(TEMP),REAL(RH),
		       REAL(WINDSPEED),REAL(PRECIP), REAL(KD)[0], REAL(CHILHF)[0], 
		       REAL(CHILHF)[1],nlayers,rhizome,
		       irtl,REAL(SENESCTIME),INTEGER(TIMESTEP)[0],vecsize,
		       REAL(SP)[0], REAL(SPD)[0], dbpcoef, REAL(THERMALP),
		       vmax,alpha,kparm,theta,beta,Rd,Ca,b0,b1, REAL(SOILCOEFS), LeafN, kLN,
		       vmaxb1, alphab1, REAL(MRESP), INTEGER(SOILTYPE)[0], INTEGER(WSFUN)[0],
		       INTEGER(WS)[0], REAL(CENTCOEFS),REAL(CENTKS), INTEGER(CENTTIMESTEP)[0],
		       INTEGER(SOILLAYERS)[0], REAL(SOILDEPTHS), REAL(CWS), INTEGER(HYDRDIST)[0],
		       REAL(SECS), REAL(NCOEFS)[0], REAL(NCOEFS)[1], REAL(NCOEFS)[2], INTEGER(LNFUN)[0],upperT,lowerT);

		/* pick the needed elements for the SSE */
		for(k=0;k<Ndat;k++){
			ind = INTEGER(INDEX)[k];
			sCanopyAssim[k] = CanopyAssim[ind];
			sLeafy[k] = Leafy[ind];
			sStemy[k] = Stemy[ind];
			sRooty[k] = Rooty[ind];
			sRhizomey[k] = Rhizomey[ind];
			sLAIy[k] = LAIc[ind];
		}

		rss = RSS_BG(oStemy,oLeafy,oRooty,oRhizomey,oGrainy,oLAIy,
			     sStemy,sLeafy,sRooty,sRhizomey,sGrainy,sLAIy,
			     n1dat);

		REAL(RssVec2)[iters2] = rss;

		mr = exp(-rss) / exp(-oldRss);

		U = runif(0,1); 
		if(mr > U){ 
			n2++;
			/* accept the coefficients */
			oldkLeaf_1 = dbpcoef[0];
			oldkStem_1 = dbpcoef[1];
			oldkRoot_1 = dbpcoef[2];
			oldkRhizome_1 = dbpcoef[3];
	    
			oldkLeaf_2 = dbpcoef[4];
			oldkStem_2 = dbpcoef[5];
			oldkRoot_2 = dbpcoef[6];
			oldkRhizome_2 = dbpcoef[7];
	    
			oldkLeaf_3 = dbpcoef[8];
			oldkStem_3 = dbpcoef[9];
			oldkRoot_3 = dbpcoef[10];
			oldkRhizome_3 = dbpcoef[11];
	    
			oldkLeaf_4 = dbpcoef[12];
			oldkStem_4 = dbpcoef[13];
			oldkRoot_4 = dbpcoef[14];
			oldkRhizome_4 = dbpcoef[15];

			oldkLeaf_5 = dbpcoef[16];
			oldkStem_5 = dbpcoef[17];
			oldkRoot_5 = dbpcoef[18];
			oldkRhizome_5 = dbpcoef[19];

			oldkLeaf_6 = dbpcoef[20];
			oldkStem_6 = dbpcoef[21];
			oldkRoot_6 = dbpcoef[22];
			oldkRhizome_6 = dbpcoef[23];
			oldkGrain_6 = dbpcoef[24];

			oldRss = rss;
		}	  
		/* put them in a matrix */
		REAL(mat1)[n1*25] = dbpcoef[0];
		REAL(mat1)[1 + n1*25] = dbpcoef[1];
		REAL(mat1)[2 + n1*25] = dbpcoef[2];
		REAL(mat1)[3 + n1*25] = dbpcoef[3];

		REAL(mat1)[4 + n1*25] = dbpcoef[4];
		REAL(mat1)[5 + n1*25] = dbpcoef[5];
		REAL(mat1)[6 + n1*25] = dbpcoef[6];
		REAL(mat1)[7 + n1*25] = dbpcoef[7];
	    
		REAL(mat1)[8 + n1*25] = dbpcoef[8];
		REAL(mat1)[9 + n1*25] = dbpcoef[9];
		REAL(mat1)[10 + n1*25] = dbpcoef[10];
		REAL(mat1)[11 + n1*25] = dbpcoef[11];

		REAL(mat1)[12 + n1*25] = dbpcoef[12];
		REAL(mat1)[13 + n1*25] = dbpcoef[13];
		REAL(mat1)[14 + n1*25] = dbpcoef[14];
		REAL(mat1)[15 + n1*25] = dbpcoef[15];

		REAL(mat1)[16 + n1*25] = dbpcoef[16];
		REAL(mat1)[17 + n1*25] = dbpcoef[17];
		REAL(mat1)[18 + n1*25] = dbpcoef[18];
		REAL(mat1)[19 + n1*25] = dbpcoef[19];

		REAL(mat1)[20 + n1*25] = dbpcoef[20];
		REAL(mat1)[21 + n1*25] = dbpcoef[21];
		REAL(mat1)[22 + n1*25] = dbpcoef[22];
		REAL(mat1)[23 + n1*25] = dbpcoef[23];
		REAL(mat1)[24 + n1*25] = dbpcoef[24];

		n1++;
	}
	PutRNGstate();

	REAL(ans1)[0] = oldkLeaf_1;
	REAL(ans1)[1] = oldkStem_1;
	REAL(ans1)[2] = oldkRoot_1;
	REAL(ans1)[3] = oldkRhizome_1;

	REAL(ans1)[4] = oldkLeaf_2;
	REAL(ans1)[5] = oldkStem_2;
	REAL(ans1)[6] = oldkRoot_2;
	REAL(ans1)[7] = oldkRhizome_2;

	REAL(ans1)[8] = oldkLeaf_3;
	REAL(ans1)[9] = oldkStem_3;
	REAL(ans1)[10] = oldkRoot_3;
	REAL(ans1)[11] = oldkRhizome_3;

	REAL(ans1)[12] = oldkLeaf_4;
	REAL(ans1)[13] = oldkStem_4;
	REAL(ans1)[14] = oldkRoot_4;
	REAL(ans1)[15] = oldkRhizome_4;

	REAL(ans1)[16] = oldkLeaf_5;
	REAL(ans1)[17] = oldkStem_5;
	REAL(ans1)[18] = oldkRoot_5;
	REAL(ans1)[19] = oldkRhizome_5;

	REAL(ans1)[20] = oldkLeaf_6;
	REAL(ans1)[21] = oldkStem_6;
	REAL(ans1)[22] = oldkRoot_6;
	REAL(ans1)[23] = oldkRhizome_6;
	REAL(ans1)[24] = oldkGrain_6;

	REAL(RSS)[0] = oldRss;

	REAL(ACCPT)[0] = accept;
	REAL(accept2)[0] = n1;
	REAL(accept3)[0] = n2;
	REAL(SAtemp)[0] = saTemp;

	for(j=0;j<Ndat;j++){
		ind = INTEGER(INDEX)[j];
		REAL(simStem)[j] = Stemy[ind];
		REAL(simLeaf)[j] = Leafy[ind];
		REAL(simRhiz)[j] = Rhizomey[ind];
		REAL(simRoot)[j] = Rooty[ind];
		REAL(simGrain)[j] = Grainy[ind];
		REAL(simLAI)[j] = LAIc[ind];
		REAL(TTime)[j] = REAL(oTHERMAL)[j];
		REAL(inde)[j] = INTEGER(INDEX)[j];
	}

	SET_VECTOR_ELT(lists,0,ans1);
	SET_VECTOR_ELT(lists,1,RSS);
	SET_VECTOR_ELT(lists,2,ACCPT);
	SET_VECTOR_ELT(lists,3,RssVec);
	SET_VECTOR_ELT(lists,4,simStem);
	SET_VECTOR_ELT(lists,5,TTime);
	SET_VECTOR_ELT(lists,6,inde);
	SET_VECTOR_ELT(lists,7,obsStem);

	SET_VECTOR_ELT(lists,8,simLeaf);
	SET_VECTOR_ELT(lists,9,obsLeaf);

	SET_VECTOR_ELT(lists,10,simRhiz);
	SET_VECTOR_ELT(lists,11,obsRhiz);

	SET_VECTOR_ELT(lists,12,simLAI);
	SET_VECTOR_ELT(lists,13,obsLAI);

	SET_VECTOR_ELT(lists,14,simRoot);
	SET_VECTOR_ELT(lists,15,obsRoot);

	SET_VECTOR_ELT(lists,16,simGrain);
	SET_VECTOR_ELT(lists,17,obsGrain);

	SET_VECTOR_ELT(lists,18,RssVec2);

	SET_VECTOR_ELT(lists,19,mat1);
	SET_VECTOR_ELT(lists,20,accept2);
	SET_VECTOR_ELT(lists,21,accept3);
	SET_VECTOR_ELT(lists,22,SAtemp);

	SET_STRING_ELT(names,0,mkChar("coefs"));
	SET_STRING_ELT(names,1,mkChar("rss"));
	SET_STRING_ELT(names,2,mkChar("accept"));
	SET_STRING_ELT(names,3,mkChar("RssVec"));
	SET_STRING_ELT(names,4,mkChar("simStem"));
	SET_STRING_ELT(names,5,mkChar("TTime"));
	SET_STRING_ELT(names,6,mkChar("index"));
	SET_STRING_ELT(names,7,mkChar("obsStem"));

	SET_STRING_ELT(names,8,mkChar("simLeaf"));
	SET_STRING_ELT(names,9,mkChar("obsLeaf"));
	SET_STRING_ELT(names,10,mkChar("simRhiz"));
	SET_STRING_ELT(names,11,mkChar("obsRhiz"));
	SET_STRING_ELT(names,12,mkChar("simLAI"));
	SET_STRING_ELT(names,13,mkChar("obsLAI"));
	SET_STRING_ELT(names,14,mkChar("simRoot"));
	SET_STRING_ELT(names,15,mkChar("obsRoot"));
	SET_STRING_ELT(names,16,mkChar("simGrain"));
	SET_STRING_ELT(names,17,mkChar("obsGrain"));

	SET_STRING_ELT(names,18,mkChar("RssVec2"));
	SET_STRING_ELT(names,19,mkChar("resMC"));
	SET_STRING_ELT(names,20,mkChar("accept2"));
	SET_STRING_ELT(names,21,mkChar("accept3"));
	SET_STRING_ELT(names,22,mkChar("saTemp"));

	setAttrib(lists,R_NamesSymbol,names);

	UNPROTECT(25);
	return(lists);
}

/* Here I will include a function which calculates the RSS */

double RSS_BG(double oStem[], double oLeaf[],
	      double oRoot[], double oRhizome[],
	      double oGrain[], double oLAI[],
	      double sStem[], double sLeaf[],
	      double sRoot[], double sRhizome[],
	      double sGrain[], double sLAI[],
	      int N1Dat){

	int i;
	double diff1=0.0, RSS1=0.0, SST1=0.0;
	double diff2=0.0, RSS2=0.0, SST2=0.0;
	double diff3=0.0, RSS3=0.0, SST3=0.0;
	double diff4=0.0, RSS4=0.0, SST4=0.0; 
	double diff5=0.0, RSS5=0.0, SST5=0.0;
	double diff6=0.0, RSS6=0.0, SST6=0.0;
	double RSS=0.0;

	for(i = 0 ; i < N1Dat; i++){
		/* Stem */
		if(oStem[i] >= 0){
			SST1 += pow(oStem[i],2);
			diff1 = oStem[i] - sStem[i];
			RSS1 += pow(diff1,2);
		}
		/* Leaf */
		if(oLeaf[i] >= 0){
			SST2 += pow(oLeaf[i],2);
			diff2 = oLeaf[i] - sLeaf[i];
			RSS2 += pow(diff2,2);
		}
		/* Rhizome */
		if(oRhizome[i] >= 0){
			SST3 += pow(oRhizome[i],2);
			diff3 = oRhizome[i] - sRhizome[i];
			RSS3 += pow(diff3,2);
		}
		/* Root */
		if(oRoot[i] >= 0){
			SST4 += pow(oRoot[i],2);
			diff4 = oRoot[i] - sRoot[i];
			RSS4 += pow(diff4,2);
		}
		/* LAI */
		if(oLAI[i] >= 0){
			SST5 += pow(oLAI[i],2);
			diff5 = oLAI[i] - sLAI[i];
			RSS5 += pow(diff5,2);
		}
		if(oGrain[i] >= 0){
			SST6 += pow(oGrain[i],2);
			diff6 = oGrain[i] - sGrain[i];
			RSS6 += pow(diff6,2);
		}
	}

	RSS = RSS1 + RSS2 + RSS3 + RSS4 + RSS5 + RSS6;
	return(RSS);
}
  

/* Main BioGro function */
void BioGro(double lat, int doy[],int hr[],double solar[],double temp[],double rh[],
	    double windspeed[],double precip[], double kd, double chil, 
	    double heightf, int nlayers,
            double iRhizome, double irtl, double sencoefs[], int timestep, int vecsize,
            double Sp, double SpD, double dbpcoefs[25], double thermalp[], double vmax1, 
	    double alpha1, double kparm, double theta, double beta, double Rd, double Catm, double b0, double b1, 
	    double soilcoefs[], double ileafn, double kLN, double vmaxb1,
	    double alphab1, double mresp[], int soilType, int wsFun, int ws, double centcoefs[],
	    double centks[], int centTimestep, int soilLayers, double soilDepths[],
	    double cws[], int hydrDist, double secs[], double kpLN, double lnb0, double lnb1, int lnfun, double upperT, double lowerT )
{

	extern double CanopyAssim[8760] ;
	extern double Leafy[8760] ;
	extern double Stemy[8760] ;
	extern double Rooty[8760] ;
	extern double Rhizomey[8760] ;
	extern double Grainy[8760] ;
	extern double LAIc[8760] ;

	double newLeafcol[8760];
	double newStemcol[8760];
	double newRootcol[8760];
	double newRhizomecol[8760];

	int i, i3;

	double Leaf, Stem, Root, Rhizome, LAI, Grain = 0.0;
	double TTc = 0.0;
	double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kRhizome = 0.0, kGrain = 0.0;
	double newLeaf, newStem = 0.0, newRoot, newRhizome = 0.0, newGrain = 0.0;

	double litter[4];
	litter[0] = centcoefs[19];
	litter[1] = centcoefs[20];
	litter[2] = centcoefs[21];
	litter[3] = centcoefs[22];

	/* Variables needed for collecting litter */
	double LeafLitter = litter[0], StemLitter = litter[1];
	double RootLitter = litter[2], RhizomeLitter = litter[3];
	double LeafLitter_d = 0.0, StemLitter_d = 0.0;
	double RootLitter_d = 0.0, RhizomeLitter_d = 0.0;
	double ALitter = 0.0, BLitter = 0.0;

	double *sti , *sti2, *sti3, *sti4; 
	double Remob;
	int k = 0, q = 0, m = 0, n = 0;
	int ri = 0;

	double StomWS = 1, LeafWS = 1;
	double CanopyA, CanopyT;
	double vmax, alpha;
	double LeafN_0 = ileafn;
	double LeafN = ileafn; /* Need to set it because it is used by CanA before it is computed */
	double iSp = Sp;
	vmax = vmax1;
	alpha = alpha1;

	/* Century */
	double MinNitro = centcoefs[18];
	double SCCs[9];
	double Resp;

	const double mrc1 = mresp[0];
	const double mrc2 = mresp[1];

	struct Can_Str Canopy;
	struct ws_str WaterS;
	struct dbp_str dbpS;
	struct cenT_str centS;
	struct soilML_str soilMLS;
	struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
	soTexS = soilTchoose(soilType);

	Rhizome = iRhizome;
	Leaf = Rhizome * irtl;
	Stem = Rhizome * 0.001;
	Root = Rhizome * 0.001;
	LAI = Leaf * Sp;

	const double FieldC = soilcoefs[0];
	const double WiltP = soilcoefs[1];
	const double phi1 = soilcoefs[2];
	const double phi2 = soilcoefs[3];
	const double soilDepth = soilcoefs[4];
	double waterCont = soilcoefs[5];
	double soilEvap, TotEvap;

	const double SeneLeaf = sencoefs[0];
	const double SeneStem = sencoefs[1];
	const double SeneRoot = sencoefs[2];
	const double SeneRhizome = sencoefs[3];

	SCCs[0] = centcoefs[0];
	SCCs[1] = centcoefs[1];
	SCCs[2] = centcoefs[2];
	SCCs[3] = centcoefs[3];
	SCCs[4] = centcoefs[4];
	SCCs[5] = centcoefs[5];
	SCCs[6] = centcoefs[6];
	SCCs[7] = centcoefs[7];
	SCCs[8] = centcoefs[8];


	/* Creation of pointers outside the loop */
	sti = &newLeafcol[0]; /* This creates sti to be a pointer to the position 0
				 in the newLeafcol vector */
	sti2 = &newStemcol[0];
	sti3 = &newRootcol[0];
	sti4 = &newRhizomecol[0];

	for(i=0;i<vecsize;i++)
	{
		/* First calculate the elapsed Thermal Time*/
		TTc += (temp[i] / (24/timestep));

		/* Do the magic! Calculate growth*/

		Canopy = CanAC(LAI,doy[i],hr[i],
			       solar[i],temp[i],rh[i],windspeed[i],
			       lat,nlayers,vmax,alpha,kparm,theta,beta,
			       Rd,Catm,b0,b1,StomWS,ws,kd, chil,
			       heightf, LeafN, kpLN, lnb0, lnb1, lnfun,upperT,lowerT);



		CanopyA = Canopy.Assim * timestep;
		CanopyT = Canopy.Trans * timestep;

		if(ISNAN(CanopyA)){
			Rprintf("LAI %.2f \n",LAI); 
			Rprintf("Leaf %.2f \n",Leaf);
			Rprintf("irtl %.2f \n",irtl);
			Rprintf("Rhizome %.2f \n",Rhizome);
			Rprintf("Sp %.2f \n",Sp);   
			Rprintf("doy[i] %.i %.i \n",i,doy[i]); 
			Rprintf("hr[i] %.i %.i \n",i,hr[i]); 
			Rprintf("solar[i] %.i %.2f \n",i,solar[i]); 
			Rprintf("temp[i] %.i %.2f \n",i,temp[i]); 
			Rprintf("rh[i] %.i %.2f \n",i,rh[i]); 
			Rprintf("windspeed[i] %.i %.2f \n",i,windspeed[i]);
			Rprintf("lat %.i %.2f \n",i,lat);
			Rprintf("nlayers %.i %.i \n",i,nlayers);   
		}
		//fake variables to avoid error due to change in soilMLs definiton to include proviison to calculate root depth based on roto front velocity
		double faked=0.5;
		double fakei=1;
		if(soilLayers > 1){
			soilMLS = soilML(precip[i], CanopyT, &cws[0], soilDepth, soilDepths, FieldC, WiltP,
					 phi1, phi2, soTexS, wsFun, soilLayers, Root,
					 LAI, 0.68, temp[i], solar[i], windspeed[i], rh[i], hydrDist,
					 secs[0], secs[1], secs[2],fakei,faked,faked);

			StomWS = soilMLS.rcoefPhoto;
			LeafWS = soilMLS.rcoefSpleaf;
			soilEvap = soilMLS.SoilEvapo;
			for(i3=0;i3<soilLayers;i3++){
				cws[i3] = soilMLS.cws[i3];
			}

		}else{

			soilEvap = SoilEvapo(LAI, 0.68, temp[i], solar[i], waterCont, FieldC, WiltP, windspeed[i], rh[i], secs[1]);
			TotEvap = soilEvap + CanopyT;
			WaterS = watstr(precip[i],TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
			waterCont = WaterS.awc;
			StomWS = WaterS.rcoefPhoto ;
			LeafWS = WaterS.rcoefSpleaf;
		}

		/* Picking the dry biomass partitioning coefficients */
		dbpS = sel_dbp_coef(dbpcoefs, thermalp, TTc);

		kLeaf = dbpS.kLeaf;
		kStem = dbpS.kStem;
		kRoot = dbpS.kRoot;
		kRhizome = dbpS.kRhiz;
		kGrain = dbpS.kGrain;

		if(ISNAN(kRhizome) || ISNAN(kLeaf) || ISNAN(kRoot) || ISNAN(kStem) || ISNAN(kGrain)){
			Rprintf("kLeaf %.2f, kStem %.2f, kRoot %.2f, kRhizome %.2f, kGrain %.2f \n",kLeaf,kStem,kRoot,kRhizome,kGrain);
			Rprintf("iter %i \n",i);
		}

		if(i % 24*centTimestep == 0){
			LeafLitter_d = LeafLitter * ((0.1/30)*centTimestep);
			StemLitter_d = StemLitter * ((0.1/30)*centTimestep);
			RootLitter_d = RootLitter * ((0.1/30)*centTimestep);
			RhizomeLitter_d = RhizomeLitter * ((0.1/30)*centTimestep);
       
			LeafLitter -= LeafLitter_d;
			StemLitter -= StemLitter_d;
			RootLitter -= RootLitter_d;
			RhizomeLitter -= RhizomeLitter_d;
       
			centS = Century(&LeafLitter_d,&StemLitter_d,&RootLitter_d,&RhizomeLitter_d,
					waterCont,temp[i],centTimestep,SCCs,WaterS.runoff,
					centcoefs[17], /* N fertilizer*/
					MinNitro, /* initial Mineral nitrogen */
					precip[i], /* precipitation */
					centcoefs[9], /* Leaf litter lignin */
					centcoefs[10], /* Stem litter lignin */
					centcoefs[11], /* Root litter lignin */
					centcoefs[12], /* Rhizome litter lignin */
					centcoefs[13], /* Leaf litter N */
					centcoefs[14], /* Stem litter N */
					centcoefs[15],  /* Root litter N */
					centcoefs[16],   /* Rhizome litter N */
					soilType, centks);
		}

		/* Here I can insert the code for Nitrogen limitations on photosynthesis
		   parameters. This is taken From Harley et al. (1992) Modelling cotton under
		   elevated CO2. PCE. This is modeled as a simple linear relationship between
		   leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
		   availability and possibly by the Thermal time accumulated.*/

		MinNitro = centS.MinN;
		Resp = centS.Resp;
     
		SCCs[0] = centS.SCs[0];
		SCCs[1] = centS.SCs[1];
		SCCs[2] = centS.SCs[2];
		SCCs[3] = centS.SCs[3];
		SCCs[4] = centS.SCs[4];
		SCCs[5] = centS.SCs[5];
		SCCs[6] = centS.SCs[6];
		SCCs[7] = centS.SCs[7];
		SCCs[8] = centS.SCs[8];

		LeafN = LeafN_0 * exp(-kLN * TTc); 
		vmax = (LeafN_0 - LeafN) * vmaxb1 + vmax1;
		alpha = (LeafN_0 - LeafN) * alphab1 + alpha1;

		if(kLeaf > 0)
		{
			newLeaf = CanopyA * kLeaf * LeafWS ; 
			/*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
			    Plant. Phys. 46, 233-235. For this the water stress coefficient is different
			    for leaf and vmax. */
			/* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
			/* The 0.02 and 0.03 are constants here but vary depending on species
			   as pointed out in that reference. */
			newLeaf = resp(newLeaf, mrc1, temp[i]);

			*(sti+i) = newLeaf; /* This populates the vector newLeafcol. It makes sense
					       to use i because when kLeaf is negative no new leaf is
					       being accumulated and thus would not be subjected to senescence */
		}else{

			newLeaf = Leaf * kLeaf ;
			Rhizome += kRhizome * -newLeaf * 0.9; /* 0.9 is the efficiency of retranslocation */
			Stem += kStem * -newLeaf   * 0.9;
			Root += kRoot * -newLeaf * 0.9;
			Grain += kGrain * -newLeaf * 0.9;
		}

		if(TTc < SeneLeaf){

			Leaf += newLeaf;

		}else{
    
			Leaf += newLeaf - *(sti+k); /* This means that the new value of leaf is
						       the previous value plus the newLeaf
						       (Senescence might start when there is
						       still leaf being produced) minus the leaf
						       produced at the corresponding k.*/
			Remob = *(sti+k) * 0.6 ;
			LeafLitter += *(sti+k) * 0.4; /* Collecting the leaf litter */ 
			Rhizome += kRhizome * Remob;
			Stem += kStem * Remob; 
			Root += kRoot * Remob;
			Grain += kGrain * Remob;
			k++;
		}

		/* The specific leaf area declines with the growing season at least in
		   Miscanthus.  See Danalatos, Nalianis and Kyritsis "Growth and Biomass
		   Productivity of Miscanthus sinensis "Giganteus" under optimum cultural
		   management in north-eastern greece*/

		if(i%24 == 0){
			Sp = iSp - (doy[i] - doy[0]) * SpD;
		}

		/* Rprintf("Sp %.2f \n", Sp); */

		LAI = Leaf * Sp ;

		/* New Stem*/
		if(kStem > 0)
		{
			newStem = CanopyA * kStem ;
			newStem = resp(newStem, mrc1, temp[i]);
			*(sti2+i) = newStem;
		}

		if(TTc < SeneStem){

			Stem += newStem;

		}else{

			Stem += newStem - *(sti2+q);
			StemLitter += *(sti2+q);
			q++;

		}

		if(kRoot > 0)
		{
			newRoot = CanopyA * kRoot ;
			newRoot = resp(newRoot, mrc2, temp[i]);
			*(sti3+i) = newRoot;
		}else{

			newRoot = Root * kRoot ;
			Rhizome += kRhizome * -newRoot * 0.9;
			Stem += kStem * -newRoot       * 0.9;
			Leaf += kLeaf * -newRoot * 0.9;
			Grain += kGrain * -newRoot * 0.9;
		}

		if(TTc < SeneRoot){

			Root += newRoot;

		}else{

			Root += newRoot - *(sti3+m);
			RootLitter += *(sti3+m);
			m++;

		}

		if(kRhizome > 0)
		{
			newRhizome = CanopyA * kRhizome ;
			newRhizome = resp(newRhizome, mrc2, temp[i]);
			*(sti4+ri) = newRhizome;
			/* Here i will not work because the rhizome goes from being a source
			   to a sink. I need its own index. Let's call it rhizome's i or ri.*/
			ri++;
		}else{

			if(Rhizome < 0){
				Rhizome = 1e-4;
				warning("Rhizome became negative");
			}

			newRhizome = Rhizome * kRhizome;
			Root += kRoot * -newRhizome ;
			Stem += kStem * -newRhizome ;
			Leaf += kLeaf * -newRhizome ;
			Grain += kGrain * -newRhizome;
		}

		if(TTc < SeneRhizome){

			Rhizome += newRhizome;

		}else {

			Rhizome += newRhizome - *(sti4+n);
			RhizomeLitter += *(sti4+n);
			n++;

		}

		if((kGrain < 1e-10) || (TTc < thermalp[4])){
			newGrain = 0.0;
			Grain += newGrain;
		}else{
			newGrain = CanopyA * kGrain;
			/* No respiration for grain at the moment */
			/* No senescence either */
			Grain += newGrain;  
		}

		ALitter += LeafLitter + StemLitter;
		BLitter += RootLitter + RhizomeLitter;

		CanopyAssim[i] =  CanopyA;
		Leafy[i] = Leaf;
		Stemy[i] = Stem;
		Rooty[i] =  Root;
		Rhizomey[i] = Rhizome;
		Grainy[i] = Grain;
		LAIc[i] = LAI;
	}
}

double sel_phen(int phen){

	double index = 0;

	if(phen == 6){
		index = runif(20,25);
	}else 
	 if(phen == 5){
		 index = runif(16,20);
	 }else
	  if(phen == 4){
		  index = runif(12,16);
	  }else
	   if(phen == 3){
		   index = runif(8,12);
	   }else
	    if(phen == 2){
		    index = runif(4,8);
	    }else
	     if(phen == 1){
		     index = runif(0,4);
	     }

	return(index);

}
