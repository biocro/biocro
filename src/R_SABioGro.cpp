/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <R.h>
#include <Rmath.h>
#include "BioCro.h"

double sel_phen(int phen)
{
    double index = 0;

    if (phen == 6) {
        index = runif(20, 25);
    } else if (phen == 5) {
        index = runif(16, 20);
    } else if (phen == 4) {
        index = runif(12, 16);
    } else if (phen == 3) {
        index = runif(8, 12);
    } else if (phen == 2) {
        index = runif(4, 8);
    } else if (phen == 1) {
        index = runif(0, 4);
    }
    return(index);
}

extern "C" {
/* Here I will include a function which calculates the RSS */

double RSS_BG(double oStem[], double oLeaf[],
	      double oRoot[], double oRhizome[],
	      double oGrain[], double oLAI[],
	      double sStem[], double sLeaf[],
	      double sRoot[], double sRhizome[],
	      double sGrain[], double sLAI[],
	      int N1Dat)
{

	double diff1=0.0, RSS1=0.0, SST1=0.0;
	double diff2=0.0, RSS2=0.0, SST2=0.0;
	double diff3=0.0, RSS3=0.0, SST3=0.0;
	double diff4=0.0, RSS4=0.0, SST4=0.0; 
	double diff5=0.0, RSS5=0.0, SST5=0.0;
	double diff6=0.0, RSS6=0.0, SST6=0.0;
	double RSS=0.0;

	for(int i = 0 ; i < N1Dat; i++){
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
#include <Rinternals.h>

/* First pass the observed data and then the simulated */

SEXP SABioGro(SEXP oTHERMAL, SEXP oSTEM, SEXP oLEAF, 
	      SEXP oROOT, SEXP oRHIZOME, 
	      SEXP oGRAIN, SEXP oLAI,
	      SEXP DOY, SEXP HR, SEXP SOLAR, 
              SEXP TEMP, SEXP RH, SEXP WINDSPEED, 
	      SEXP PRECIP, SEXP DBPCOEF, SEXP VECSIZE, 
	      SEXP LAT, SEXP NLAYERS, SEXP INITIAL_BIOMASS, 
	      SEXP SENESCTIME, SEXP TIMESTEP, 
	      SEXP VMAX, SEXP ALPHA, SEXP KPARM, 
	      SEXP THETA, SEXP BETA, SEXP RD, 
	      SEXP CATM, SEXP B0, SEXP B1, 
	      SEXP SOILCOEFS, SEXP WSFUN, SEXP WATER_STRESS_APPROACH, 
	      SEXP ILEAFN, SEXP KLN, SEXP VMAXB1, 
	      SEXP ALPHAB1, SEXP MRESP, SEXP SOILTYPE, 
	      SEXP CENTCOEFS, SEXP CENTKS, SEXP CENTTIMESTEP, 
	      SEXP KD, SEXP CHILHF, SEXP SP, 
	      SEXP SPD, SEXP THERMAL_BASE_TEMP, SEXP THERMALP, SEXP INDEX, 
	      SEXP NDATA, SEXP N1DAT, SEXP NITER, 
	      SEXP NITER2, SEXP SATEMP, SEXP COOLSAMP, 
	      SEXP SCALE, SEXP SD, SEXP PHEN, 
	      SEXP SOILLAYERS, SEXP SOILDEPTHS, 
	      SEXP CWS, SEXP HYDRDIST, SEXP SECS, 
	      SEXP NCOEFS, SEXP LNFUN,SEXP UPPERTEMP, SEXP LOWERTEMP,SEXP NNITROP, SEXP STOMWS)
{
    // Optimizes the partitioning coefficients using observed data.
    //
    // Two optimizations are done in sequence. Each optimization has the following stages:
    //      1: Randomly select a single partitioning coeffient from any stage of growth and assign it a random value.
    //      2: Normalize the coefficients so that for each stage, they sum to 1.
    //      3: Run BioCro with the set of coefficients.
    //      4: Compare the sums of squares between the new set of coefficients and the set with the lowest observed sum of squares
    //         to decide whether to keep the new set. The two optimization routines have different criteria to determine
    //         whether the new parameters are kept.
       
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

	struct BioGro_results_str *results = (struct BioGro_results_str*)malloc(sizeof(struct BioGro_results_str));
    if (results) {
        initialize_biogro_results(results, INTEGER(SOILLAYERS)[0], INTEGER(VECSIZE)[0]);
    } else {
        Rprintf("Out of memory in R_SABioGro.cpp.\n");
        return R_NilValue;
    }

	/* Index variables */
	int accept = 0;
	int n1 = 0, n2 = 0;
    double upperT=REAL(UPPERTEMP)[0];
	double lowerT=REAL(LOWERTEMP)[0];
	double StomWS = REAL(STOMWS)[0];
	double *initial_biomass = REAL(INITIAL_BIOMASS);

	/* The all important vector size */
	int vecsize = INTEGER(VECSIZE)[0];
	int n1dat = 0;
	size_t Ndat = INTEGER(NDATA)[0];
	int niter = INTEGER(NITER)[0];
	int niter2 = INTEGER(NITER2)[0];
	double saTemp;
	int coolSamp;
	double scale;
	int phen;


	saTemp = REAL(SATEMP)[0];
	coolSamp = INTEGER(COOLSAMP)[0];
	scale = REAL(SCALE)[0];
	phen = INTEGER(PHEN)[0];

	// int timestep; unused
	double b0, b1;
	double vmaxb1, alphab1;
	double LeafN, kLN, Ca;

	b0 = REAL(B0)[0];
	b1 = REAL(B1)[0];
	// timestep = INTEGER(TIMESTEP)[0]; unused

	alphab1 = REAL(ALPHAB1)[0];
	vmaxb1 = REAL(VMAXB1)[0];

	LeafN = REAL(ILEAFN)[0];
	kLN = REAL(KLN)[0];

	/* Needed variables */
	double lati;
	/* Creating vectors */
	double dbpcoef[25];
	/* Picking the simulation */
	int ind;
	// double sCanopyAssim[Ndat]; unused
    double sStemy[Ndat], sLeafy[Ndat];
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
	int nlayers = INTEGER(NLAYERS)[0];
	Rd = REAL(RD)[0];
	Ca = REAL(CATM)[0];
	vmax = REAL(VMAX)[0];
	alpha = REAL(ALPHA)[0];
	kparm = REAL(KPARM)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];

	/* Pick the observed */
	for(size_t m = 0; m < Ndat; ++m) {
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

	for(int iters = 0; iters < niter; ++iters){

        /* Selecting the index to sample */

		index = sel_phen(phen);
	
		/* First phenological stage */
	
		kLeaf_1 = fabs(oldkLeaf_1);
		kStem_1 = fabs(oldkStem_1);
		kRoot_1 = fabs(oldkRoot_1);


		if(index < 1){
			kLeaf_1 = fabs(rnorm(kLeaf_1,sd1));
		}else if(index < 2){
			kStem_1 = fabs(rnorm(kStem_1,sd1));
		}else if(index < 3){
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
		       REAL(WINDSPEED),REAL(PRECIP), REAL(KD)[0], REAL(CHILHF)[0], REAL(CHILHF)[2], REAL(CHILHF)[3], 
		       REAL(CHILHF)[1],nlayers, initial_biomass,
		       REAL(SENESCTIME),INTEGER(TIMESTEP)[0],vecsize,
		       REAL(SP)[0], REAL(SPD)[0], dbpcoef, REAL(THERMALP), REAL(THERMAL_BASE_TEMP)[0],
		       vmax,alpha,kparm,theta,beta,Rd,Ca,b0,b1, REAL(SOILCOEFS), LeafN, kLN,
		       vmaxb1, alphab1, REAL(MRESP), INTEGER(SOILTYPE)[0], INTEGER(WSFUN)[0],
		       INTEGER(WATER_STRESS_APPROACH)[0], REAL(CENTCOEFS), INTEGER(CENTTIMESTEP)[0], REAL(CENTKS),
		       INTEGER(SOILLAYERS)[0], REAL(SOILDEPTHS), REAL(CWS), INTEGER(HYDRDIST)[0], 
		       REAL(SECS), REAL(NCOEFS)[0], REAL(NCOEFS)[1], REAL(NCOEFS)[2], INTEGER(LNFUN)[0],upperT,lowerT,nitroparms, StomWS, thermal_leaf_nitrogen_limitation, results);

		/* pick the needed elements for the SSE */
		for(size_t k = 0; k < Ndat; ++k) {
			ind = INTEGER(INDEX)[k];
			// sCanopyAssim[k] = results->CanopyAssim[ind];
			sLeafy[k] = results->Leafy[ind];
			sStemy[k] = results->Stemy[ind];
			sRooty[k] = results->Rooty[ind];
			sRhizomey[k] = results->Rhizomey[ind];
			sGrainy[k] = results->Grainy[ind];
			sLAIy[k] = results->LAIc[ind];
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

	for(int iters2 = 0; iters2 < niter2; ++iters2) {
		index = sel_phen(phen);

		/* First phenological stage */
		kLeaf_1 = fabs(oldkLeaf_1);
		kStem_1 = fabs(oldkStem_1);
		kRoot_1 = fabs(oldkRoot_1);
        
        if(index < 1) {
            kLeaf_1 = fabs(rnorm(kLeaf_1,sd1));
        } else if(index < 2) {
            kStem_1 = fabs(rnorm(kStem_1,sd1));
        } else if(index < 3) {
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
		       REAL(WINDSPEED),REAL(PRECIP), REAL(KD)[0], REAL(CHILHF)[0], REAL(CHILHF)[2], REAL(CHILHF)[3], 
		       REAL(CHILHF)[1],nlayers, initial_biomass,
		       REAL(SENESCTIME),INTEGER(TIMESTEP)[0],vecsize,
		       REAL(SP)[0], REAL(SPD)[0], dbpcoef, REAL(THERMALP), REAL(THERMAL_BASE_TEMP)[0],
		       vmax,alpha,kparm,theta,beta,Rd,Ca,b0,b1, REAL(SOILCOEFS), LeafN, kLN,
		       vmaxb1, alphab1, REAL(MRESP), INTEGER(SOILTYPE)[0], INTEGER(WSFUN)[0],
		       INTEGER(WATER_STRESS_APPROACH)[0], REAL(CENTCOEFS), INTEGER(CENTTIMESTEP)[0], REAL(CENTKS),
		       INTEGER(SOILLAYERS)[0], REAL(SOILDEPTHS), REAL(CWS), INTEGER(HYDRDIST)[0],
		       REAL(SECS), REAL(NCOEFS)[0], REAL(NCOEFS)[1], REAL(NCOEFS)[2], INTEGER(LNFUN)[0],upperT,lowerT,nitroparms, StomWS, thermal_leaf_nitrogen_limitation, results);

		/* pick the needed elements for the SSE */
		for(size_t k = 0; k < Ndat; ++k){
			ind = INTEGER(INDEX)[k];
			// sCanopyAssim[k] = results->CanopyAssim[ind]; unused
			sLeafy[k] = results->Leafy[ind];
			sStemy[k] = results->Stemy[ind];
			sRooty[k] = results->Rooty[ind];
			sRhizomey[k] = results->Rhizomey[ind];
			sLAIy[k] = results->LAIc[ind];
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

	for(size_t j = 0; j < Ndat; ++j){
		ind = INTEGER(INDEX)[j];
		REAL(simStem)[j] = results->Stemy[ind];
		REAL(simLeaf)[j] = results->Leafy[ind];
		REAL(simRhiz)[j] = results->Rhizomey[ind];
		REAL(simRoot)[j] = results->Rooty[ind];
		REAL(simGrain)[j] = results->Grainy[ind];
		REAL(simLAI)[j] = results->LAIc[ind];
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
	free_biogro_results(results);
	return(lists);
}
}  // extern "C"

