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
//#include "CanA_3D_Structure.h"

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
        SEXP IRHIZOME,             /* Ini Rhiz                 12 */
        SEXP IRTL,                /* i rhiz to leaf           13 */
        SEXP SENCOEFS,            /* sene coefs               14 */
        SEXP TIMESTEP,            /* time step                15 */
        SEXP VECSIZE,             /* vector size              16 */
        SEXP SPLEAF,              /* Spec Leaf Area           17 */
        SEXP SPD,                 /* Spec Lefa Area Dec       18 */
        SEXP DBPCOEFS,            /* Dry Bio Coefs            19 */
        SEXP THERMALP,            /* Themal Periods           20 */
		SEXP TBASE,
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
    /* Creating pointers to avoid calling functions REAL and INTEGER so much */
    double lat = REAL(LAT)[0];
    int *doy = INTEGER(DOY);
    int *hour = INTEGER(HR);
    double *solar = REAL(SOLAR);
    double *temp = REAL(TEMP);
    double *rh = REAL(RH);
    double *windspeed = REAL(WINDSPEED);
    double *precip = REAL(PRECIP);
    double kd = REAL(KD)[0];
    double chil = REAL(CHILHF)[0];
    double heightf = REAL(CHILHF)[1];
    int nlayers = INTEGER(NLAYERS)[0];
    double iRhizome = REAL(IRHIZOME)[0];
    double propLeaf = REAL(IRTL)[0]; 
    double *sencoefs = REAL(SENCOEFS);
    int timestep = INTEGER(TIMESTEP)[0];
    int vecsize = INTEGER(VECSIZE)[0];
    double Sp = REAL(SPLEAF)[0]; 
    double SpD = REAL(SPD)[0];
    double *dbpcoefs = REAL(DBPCOEFS);
    double *thermalp = REAL(THERMALP);
	double tbase = REAL(TBASE)[0];
    double vmax1 = REAL(VMAX)[0];
    double alpha1 = REAL(ALPHA)[0];
    double kparm = REAL(KPARM)[0];
    double theta = REAL(THETA)[0];
    double beta = REAL(BETA)[0];
    double Rd = REAL(RD)[0];
    double Catm = REAL(CATM)[0];
    double b0 = REAL(B0)[0];
    double b1 = REAL(B1)[0];
    double *soilcoefs = REAL(SOILCOEFS);
    double ileafn = REAL(ILEAFN)[0];
    // double kLN = REAL(KLN)[0]; unused
    // double vmaxb1 = REAL(VMAXB1)[0];
    // double alphab1 = REAL(ALPHAB1)[0];
    // double *mresp = REAL(MRESP);
    int soilType = INTEGER(SOILTYPE)[0];
    int wsFun = INTEGER(WSFUN)[0];
    int ws = INTEGER(WS)[0];
    // double *centcoefs = REAL(CENTCOEFS);
    // int centTimestep = INTEGER(CENTTIMESTEP)[0];
    // double *centks = REAL(CENTKS);
    int soilLayers = INTEGER(SOILLAYERS)[0];
    double *soilDepths = REAL(SOILDEPTHS);
    double *cws = REAL(CWS);
    double hydrDist = INTEGER(HYDRDIST)[0];
    double *secs = REAL(SECS);
    double kpLN = REAL(KPLN)[0];
    double lnb0 = REAL(LNB0)[0]; 
    double lnb1 = REAL(LNB1)[0];
    // int lnfun = INTEGER(LNFUN)[0]; unused
    // double *sugarcoefs = REAL(SUGARCOEFS);
    double upperT = REAL(UPPERTEMP)[0];
    double lowerT = REAL(LOWERTEMP)[0];
    double LeafNmax = REAL(MAXLN)[0]; /* max N conc of top leaf in mmol/m2*/
    double LeafNmin = REAL(MINLN)[0];  /* min N conc of top leaf in mmol/m2 */
    double dayofMaxLeafN = REAL(DAYMAXLN)[0]; /* day when maxm leaf N occuers */
    double leafrate = REAL(LEAFTURNOVER)[0];
    double rootrate = REAL(ROOTTURNOVER)[0];
    // double leafREMOB = REAL(LEAFREMOBILIZE)[0]; unused
    // int optiontocalculaterootdepth = INTEGER(OPTIONTOCALCULATEROOTDEPTH)[0];
    // double rootfrontvelocity = REAL(ROOTFRONTVELOCITY)[0];

    /*Reading NitroP Variables */
    struct nitroParms nitrop;
    double TEMPdoubletoint;
    nitrop.ileafN = REAL(NNITROP)[0];
    nitrop.kln = REAL(NNITROP)[1];
    nitrop.Vmaxb1 = REAL(NNITROP)[2];
    nitrop.Vmaxb0 = REAL(NNITROP)[3];
    nitrop.alphab1 = REAL(NNITROP)[4];
    nitrop.alphab0 = REAL(NNITROP)[5];
    nitrop.Rdb1 = REAL(NNITROP)[6];
    nitrop.Rdb0 = REAL(NNITROP)[7];
    nitrop.kpLN = REAL(NNITROP)[8];
    nitrop.lnb0 = REAL(NNITROP)[9];
    nitrop.lnb1 = REAL(NNITROP)[10];
    TEMPdoubletoint = REAL(NNITROP)[11];
    nitrop.lnFun = (int)TEMPdoubletoint;
    nitrop.maxln = REAL(NNITROP)[12];
    nitrop.minln = REAL(NNITROP)[13];
    nitrop.daymaxln = REAL(NNITROP)[14];
    //  Rprintf("%f, %f, %f, %f, %i \n",  nitrop.Vmaxb1,nitrop.Vmaxb0,   nitrop.alphab1,  nitrop.alphab0,nitrop.lnFun);
    /////////////////////////////////////////////////////////////////

    struct frostParms frostparms;
    frostparms.leafT0 = REAL(FROSTP)[0];
    frostparms.leafT100 = REAL(FROSTP)[1];
    frostparms.stemT0 = REAL(FROSTP)[2];
    frostparms.stemT100 = REAL(FROSTP)[3];

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
    int dailyvecsize = (int)(vecsize/24)+1;

    PROTECT(lists = allocVector(VECSXP, 44)); /* total of 32 variables are to be sent to R */
    PROTECT(names = allocVector(STRSXP, 44)); /* name of 32 variables */

    PROTECT(DayofYear = allocVector(REALSXP, dailyvecsize));
    PROTECT(Hour = allocVector(REALSXP, vecsize));
    PROTECT(CanopyAssim = allocVector(REALSXP, vecsize));
    PROTECT(CanopyTrans = allocVector(REALSXP, vecsize));
    PROTECT(Leafy = allocVector(REALSXP, dailyvecsize));
    PROTECT(Stemy = allocVector(REALSXP, dailyvecsize));
    PROTECT(Rooty = allocVector(REALSXP, dailyvecsize));
    PROTECT(Sugary = allocVector(REALSXP, dailyvecsize));
    PROTECT(Fibery = allocVector(REALSXP, dailyvecsize));
    PROTECT(Seedcaney = allocVector(REALSXP, dailyvecsize));
    PROTECT(LAIc = allocVector(REALSXP, dailyvecsize));
    PROTECT(TTTc = allocVector(REALSXP, vecsize));
    PROTECT(SoilWatCont = allocVector(REALSXP, vecsize));
    PROTECT(StomatalCondCoefs = allocVector(REALSXP, vecsize));
    PROTECT(LeafReductionCoefs = allocVector(REALSXP, vecsize));
    PROTECT(LeafNitrogen = allocVector(REALSXP, dailyvecsize));
    PROTECT(AboveLitter = allocVector(REALSXP, dailyvecsize));
    PROTECT(BelowLitter = allocVector(REALSXP, dailyvecsize));
    PROTECT(VmaxVec = allocVector(REALSXP, dailyvecsize));
    PROTECT(AlphaVec = allocVector(REALSXP, dailyvecsize));
    PROTECT(SpVec = allocVector(REALSXP, dailyvecsize));
    PROTECT(SoilEvaporation = allocVector(REALSXP, vecsize));
    PROTECT(cwsMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(psimMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(rdMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(LeafPsimVec = allocVector(REALSXP, vecsize));
    PROTECT(LeafLittervec = allocVector(REALSXP, dailyvecsize));
    PROTECT(StemLittervec = allocVector(REALSXP, dailyvecsize));
    PROTECT(RootLittervec = allocVector(REALSXP, dailyvecsize));
    PROTECT(Leafmaintenance = allocVector(REALSXP, vecsize));
    PROTECT(Stemmaintenance = allocVector(REALSXP, vecsize));
    PROTECT(Rootmaintenance = allocVector(REALSXP, vecsize));
    PROTECT(Seedcanemaintenance = allocVector(REALSXP, vecsize));
    PROTECT(kkleaf = allocVector(REALSXP, dailyvecsize));
    PROTECT(kkroot = allocVector(REALSXP, dailyvecsize));
    PROTECT(kkstem = allocVector(REALSXP, dailyvecsize));
    PROTECT(kkseedcane = allocVector(REALSXP, dailyvecsize));
    PROTECT(kkfiber = allocVector(REALSXP, dailyvecsize));
    PROTECT(kksugar = allocVector(REALSXP, dailyvecsize));
    PROTECT(dailyThermalT = allocVector(REALSXP, dailyvecsize));
    PROTECT(moisturecontent = allocVector(REALSXP, dailyvecsize));
    PROTECT(dayafterplanting = allocVector(REALSXP, dailyvecsize));

    /*********************3D Canopy Parameters*********************************/
    // double canparms = 1.0; unused
    double nrows = 2806;
    double ncols = 27;
    double **canopy3Dstructure;
    int ihere,jhere;

    canopy3Dstructure =  malloc(nrows * sizeof(double *));

    for(ihere = 0; ihere < nrows; ihere++) {
        canopy3Dstructure[ihere] =  malloc((ncols+2) * sizeof(double));
    }
    // Initializing the canopy matrix
    for (ihere = 0; ihere < nrows; ihere++) {
        for (jhere = 0; jhere < ncols; jhere++) {
            canopy3Dstructure[ihere][jhere] = 0.0;
        }
    }
    /*********************************************************/

    double *newLeafcol = malloc(vecsize*sizeof(double));
    if(newLeafcol==NULL) { 
        Rprintf("Out of Memory for newLeafcol\n");
        return((SEXP)1);
    }

    double *newStemcol = malloc(vecsize*sizeof(double));
    if(newStemcol==NULL) { 
        Rprintf("Out of Memory for newStemcol\n");
        return((SEXP)1);
    }

    double *newRootcol = malloc(vecsize*sizeof(double));
    if(newRootcol==NULL) { 
        Rprintf("Out of Memory for newRootcol\n");
        return((SEXP)1);
    }

    double *newRhizomecol = malloc(vecsize*sizeof(double));
    if(newRhizomecol==NULL) { 
        Rprintf("Out of Memory for newRhizomecol\n");
        return((SEXP)1);
    }

    ///////////////////////////////////////////////////////////

    int i, i3;

    double Leaf = 0.0, Stem = 0.0, Root = 0.0, Rhizome = 0.0, LAI = 0.0;
    double TTc = 0.0;
    double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kSeedcane = 0.0;
    double newLeaf = 0.0, newStem = 0.0, newRoot = 0.0;

    double currentday;
    double laimax;
    double Sugar = 0.0, newSugar = 0.0;

    /* Variables needed for collecting litter */
    // double LeafLitter_d = 0.0, StemLitter_d = 0.0; unused
    // double RootLitter_d = 0.0, RhizomeLitter_d = 0.0; unused
    double ALitter = 0.0, BLitter = 0.0;

    double *sti, *sti3, *sti4; 
    // double *sti2; unused
    // double Remob; unused
    // int k = 0, q = 0, m = 0, n = 0; unused
    int ri = 0;

    double StomataWS = 1, LeafWS = 1;
    double CanopyA, CanopyT;
    double LeafN = ileafn;
    // double iSp = Sp;
	double vmax = vmax1;
	double alpha = alpha1;
    // double SugarStress = 1;

    /* Maintenance respiration */
    // double mrc1 = mresp[0]; unused
    // double mrc2 = mresp[1];  unused

    const double FieldC = soilcoefs[0];
    const double WiltP = soilcoefs[1];
    const double phi1 = soilcoefs[2];
    const double phi2 = soilcoefs[3];
    const double soilDepth = soilcoefs[4];
    double waterCont = soilcoefs[5];
    double soilEvap, TotEvap;

    const double seneLeaf = sencoefs[0];
    const double seneStem = sencoefs[1];
    const double seneRoot = sencoefs[2];
    //const double seneRhizome = sencoefs[3]; unused

    struct Can_Str Canopy = {0,0,0};
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    struct dbp_sugarcane_str dbpS;
    struct dbp_str dbpS_old;
    // struct cenT_str centS;  unused
    struct soilML_str soilMLS;
    struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
    soTexS = soilTchoose(soilType);

    Rhizome = iRhizome;
    /* It is useful to assume that there is a small amount of
       leaf area at the begining of the growing season. */
    Leaf = Rhizome * propLeaf;
    /* Initial proportion of the rhizome that is turned
       into leaf the first hour */
    Stem = Rhizome * 0.001;
    Root = Rhizome * 0.001;
    LAI = Leaf * Sp;

    /* Creation of pointers outside the loop */
    sti = &newLeafcol[0]; /* This creates sti to be a pointer to the position 0
                             in the newLeafcol vector */
    // sti2 = &newStemcol[0]; unused
    sti3 = &newRootcol[0];
    sti4 = &newRhizomecol[0];

    /* Some soil related empirical coefficients */
    double rfl = secs[0];  /* root factor lambda */
    double rsec = secs[1]; /* radiation soil evaporation coefficient */
    double rsdf = secs[2]; /* root soil depth factor */
    double scsf = soilcoefs[6]; /* stomatal conductance sensitivity factor */ /* Rprintf("scsf %.2f",scsf); */
    double transpRes = soilcoefs[7]; /* Resistance to transpiration from soil to leaf */
    double leafPotTh = soilcoefs[8]; /* Leaf water potential threshold */

	double water_status[soilLayers * vecsize];
	double root_distribution[soilLayers * vecsize];
	double psi[soilLayers * vecsize];
    double cwsVecSum = 0.0;

    /* Parameters for calculating leaf water potential */
    double LeafPsim = 0.0;

    /* Variables for Respiration Calculations. I am currently using hard coded values. I shall write a function that can takes these values as input. I have used optim function and data from Liu to get these values. */
    double gRespCoeff = 0.15;/*This is constant fraction to calculate Growth respiration from DSSAT*/
    double Qleaf = 1.58, Qstem = 1.80, Qroot = 1.80; /* Q factors for Temp dependence on Maintenance Respiration*/
    double mRespleaf = 0.012,mRespstem = 0.00016,mResproot = 0.00036;
    // double mResp; unused

    /* all of these parameters are for Cuadra biomass partitioning */
    // double TT0 = sugarcoefs[0];  unused
    // double  TTseed = sugarcoefs[1];  unused
    // double Tmaturity = sugarcoefs[2];  unused
    // double Rd = sugarcoefs[3];  unused
    // double Alm = sugarcoefs[4];  unused
    // double Arm = sugarcoefs[5];  unused
    // double Clstem = sugarcoefs[6];  unused
    // double Ilstem = sugarcoefs[7];  unused
    // double Cestem = sugarcoefs[8];  unused
    // double Iestem = sugarcoefs[9];  unused
    // double Clsuc = sugarcoefs[10];  unused
    // double Ilsuc = sugarcoefs[11];  unused
    // double Cesuc = sugarcoefs[12];  unused
    // double Iesuc = sugarcoefs[13];  unused
    /* variable declaration for cuadra biomass partitionign ends here */


    // centS.SCs[0] = 0.0; unused
    // centS.SCs[1] = 0.0; unused
    // centS.SCs[2] = 0.0; unused
    // centS.SCs[3] = 0.0; unused
    // centS.SCs[4] = 0.0; unused
    // centS.SCs[5] = 0.0; unused
    // centS.SCs[6] = 0.0; unused
    // centS.SCs[7] = 0.0; unused
    // centS.SCs[8] = 0.0; unused


    double moist,ddap;

    double dailyassim = 0.0;
    double dailytransp = 0.0;
    double dailyLeafResp = 0.0;
    double dailyStemResp = 0.0;
    double totalMResp,Mresplimit,Mresp_reduction;
    double dailyRootResp = 0.0;
    // double dailyRhizomeResp = 0.0; unused
    double LeafResp,StemResp,RootResp;
    // double RhizomeResp; unused
    // double Litter; unused
    double LAIi;

    double rootlitter = 0.0, deadroot = 0.0, leaflitter = 0.0, deadleaf = 0.0;
    double remobilizedleaf;
    double Remobfactorleaf = 0.5;
    double Fiber = 0.0 ,newFiber = 0.0;
    double kSugar,kFiber,SeedcaneResp = 0.0, Seedcane = 0.0, newSeedcane = 0.0, dailySeedcaneResp = 0.0;
    double LAIold, LAInew;
    int dayi = 0; /* this is idnex for days */
    LAIi = Rhizome * propLeaf*Sp;
    LAIold = LAIi; /* Enter simulation with LAIold variable to track precious LAI to implement water stress */

    /************Based on Today, we need to calculate leaf photosynthesis parameters before the main loop begins ******/
    currentday = doy[0];
    LeafN = seasonal(LeafNmax, LeafNmin , currentday, dayofMaxLeafN, 365.0, lat);
    //      Rprintf("%f, %f, %f,%f,%f, %f\n",LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,lat,LeafN);
    if(nitrop.lnFun == 0) {
        vmax = vmax1;
        Rd = Rd;
        alpha = alpha1;
    } else {
        vmax = nitrop.Vmaxb1 * LeafN + nitrop.Vmaxb0;
        if(vmax < 0) vmax = 0.0;
        alpha = nitrop.alphab1 * LeafN + nitrop.alphab0;
        Rd = nitrop.Rdb1 * LeafN + nitrop.Rdb0;
    }

    for(i = 0; i < vecsize; i++)
    {
        /* First calculate the elapsed Thermal Time*/
        if(temp[i] > tbase) {
            TTc += (temp[i]-tbase) / (24/timestep);
        }

        /*  Do the magic! Calculate growth*/

        Canopy = CanAC(LAI, doy[i], hour[i],
                solar[i], temp[i], rh[i], windspeed[i],
                lat, nlayers, vmax, alpha, kparm, beta,
                Rd, Catm, b0, b1, theta, kd, chil,
                heightf, LeafN, kpLN, lnb0, lnb1, nitrop.lnFun, upperT, lowerT,
				nitrop, 0.04, 0, StomataWS, ws);

        // CanopyA = Canopy.Assim * timestep;
        CanopyA = Canopy.GrossAssim * timestep;
        CanopyT = Canopy.Trans * timestep;

        LAIold = Leaf * Sp;

        /* summing up hourly results to obtain daily assimilation */
        dailyassim =  dailyassim + CanopyA;
        dailytransp = dailytransp + CanopyT;
        /* updated value of daily assimilation and transpiration */

        /*Evaluate maintenance Respiration of different plant organs */
        LeafResp = MRespiration(Leaf, Qleaf, mRespleaf, temp[i], timestep);
        REAL(Leafmaintenance)[i] = LeafResp;
        StemResp = MRespiration(Stem, Qstem, mRespstem, temp[i], timestep);
        REAL(Stemmaintenance)[i] = StemResp;
        RootResp = MRespiration(Root, Qroot, mResproot, temp[i], timestep);
        REAL(Rootmaintenance)[i] = RootResp;
        SeedcaneResp = MRespiration(Seedcane, Qstem, mRespstem, temp[i], timestep);
        REAL(Seedcanemaintenance)[i] = SeedcaneResp;

        /*summing up hourly maintenance Respiration to determine daily maintenance respiration of each plant organ */
        dailyLeafResp =  dailyLeafResp + LeafResp;
        dailyStemResp =  dailyStemResp + StemResp;
        dailyRootResp =  dailyRootResp + RootResp;
        dailySeedcaneResp =  dailySeedcaneResp + SeedcaneResp;

        /* Inserting the multilayer model */
        if(soilLayers > 1) {
            soilMLS = soilML(precip[i], CanopyT, cws, soilDepth, soilDepths, FieldC, WiltP,
                    phi1, phi2, soTexS, wsFun, soilLayers, Root, 
                    LAI, 0.68, temp[i], solar[i], windspeed[i], rh[i], 
                    hydrDist, rfl, rsec, rsdf);

            StomataWS = soilMLS.rcoefPhoto;
            LeafWS = soilMLS.rcoefSpleaf;
            soilEvap = soilMLS.SoilEvapo;

            for(i3 = 0; i3 < soilLayers; i3++) {
                cws[i3] = soilMLS.cws[i3];
                cwsVecSum += cws[i3];
				water_status[i3 + i*soilLayers] = soilMLS.cws[i3];
                root_distribution[i3 + i*soilLayers] = soilMLS.rootDist[i3];
				psi[i3 + i * soilLayers] = 0;
            }
            waterCont = cwsVecSum / soilLayers;
            cwsVecSum = 0.0;

        } else {
            soilEvap = SoilEvapo(LAI, 0.68, temp[i], solar[i], waterCont, FieldC, WiltP, windspeed[i], rh[i], rsec);
            TotEvap = soilEvap + CanopyT;
            WaterS = watstr(precip[i],TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
            waterCont = WaterS.awc;
            StomataWS = WaterS.rcoefPhoto ; 
            LeafWS = WaterS.rcoefSpleaf;
            water_status[i] = waterCont;
			psi[i] = WaterS.psim;
			root_distribution[i] = 0;
        }

        /* An alternative way of computing water stress is by doing the leaf
         * water potential. This is done if the wsFun is equal to 4 */

        if(wsFun == 4) {
            /* Calculating the leaf water potential */
            /* From Campbell E = (Psim_s - Psim_l)/R or
             * evaporation is equal to the soil water potential
             * minus the leaf water potential divided by the resistance.
             * This can be rearranged to Psim_l = Psim_s - E x R   */
            /* It is assumed that total resistance is 5e6 m^4 s^-1
             * kg^-1 
             * Transpiration is in Mg ha-2 hour-1
             * Multiply by 1e3 to go from Mg to kg
             * Multiply by 1e-4 to go from ha to m^2 
             * This needs to go from hours to seconds that's
             * why the conversion factor is (1/3600).*/
            LeafPsim = WaterS.psim - (CanopyT * 1e3 * 1e-4 * 1.0/3600.0) * transpRes;

            /* From WIMOVAVC the proposed equation to simulate the effect of water
             * stress on stomatal conductance */
            if(LeafPsim < leafPotTh) {
                /* StomataWS = 1 - ((LeafPsim - leafPotTh)/1000 *
                 * scsf); In WIMOVAC this equation is used but
                 * the absolute values are taken from the
                 * potentials. Since they both should be
                 * negative and leafPotTh is greater than
                 * LeafPsim this can be rearranged to*/ 
                StomataWS = 1 - ((leafPotTh - LeafPsim)/1000 * scsf);
                /* StomataWS = 1; */
                if(StomataWS < 0.1) StomataWS = 0.1;
            } else {
                StomataWS = 1;
            }
        } else {
            LeafPsim = 0;
        }

        /*	dbpS = SUGARCANE_DBP(TTc); */
        /*See Details to check arguments ot CUADRA biomass partitionign function */

        /* A new day starts here */
        if( (i % 24) ==0) {

            /* daily loss of assimilated CO2 via growth respiration */
            /* Here I am calling function to calculate Growth Respiration. Currently Using a fraction of 0.242 is lost via growth respiration. This fraction is taken from DSSAT                 4.5 manual for Sugarcane: parameter RespGcf *****/
            totalMResp = dailyLeafResp + dailyStemResp + dailyRootResp + dailySeedcaneResp;

            Mresplimit = 0.18 * dailyassim;
            if(totalMResp > Mresplimit && totalMResp > 0) {
                Mresp_reduction = Mresplimit / totalMResp;
                dailyLeafResp = dailyLeafResp * Mresp_reduction;
                dailyStemResp = dailyStemResp * Mresp_reduction;
                dailyRootResp = dailyRootResp * Mresp_reduction;
                dailySeedcaneResp = dailySeedcaneResp * Mresp_reduction;
                totalMResp = Mresplimit;
            }


            dailyassim = GrowthRespiration(dailyassim, gRespCoeff); /* Plant growth continues during day and night*/
            dailyassim = dailyassim - totalMResp; // take care of all respirations here to avoid reduction in stem
            // If  daily assim becomes negatice as governed by respiration losses then set net daily assim to zero and also adjust
            // respiration loss of stem and root so that they match mass balance
            // Here is the conditions when daily assimilation may become negative ona cloudy day


            /* daily loss via  maintenance respirations of different plant organs */
            /*		  dailyassim = dailyassim-dailyLeafResp-dailyStemResp- dailyRootResp- dailyRhizomeResp;  *//* This is available photosynthate available for plant growth */


            /* call the biomass partitioning function */

            //		  dbpS = SUGARCANE_DBP_CUADRA(TTc,TT0,TTseed,Tmaturity,Rd,Alm,Arm,Clstem,Ilstem,Cestem,Iestem,Clsuc,Ilsuc,Cesuc,Iesuc,temp[i]);
            dbpS_old = sel_dbp_coef(dbpcoefs, thermalp, TTc); 

            dbpS.kLeaf = dbpS_old.kLeaf;
            dbpS.kStem = dbpS_old.kStem;
            dbpS.kRoot = dbpS_old.kRoot;
            dbpS.kSeedcane = dbpS_old.kRhiz;
            dbpS.kSugar = 0.0;
            dbpS.kFiber = dbpS_old.kStem;
            /*
               dbpS.kLeaf = 0.001;
               dbpS.kStem = 0.7;
               dbpS.kRoot = 0.299;
               dbpS.kSeedcane = dbpS.kSeedcane;
               dbpS.kSugar = 0;
               dbpS.kFiber = 0;
               */

            kLeaf = dbpS.kLeaf;
            kStem = dbpS.kStem;
            kRoot = dbpS.kRoot;
            kSeedcane = dbpS.kSeedcane;
            kSugar = dbpS.kSugar;
            kFiber = dbpS.kFiber;


            REAL(kkleaf)[dayi] = dbpS.kLeaf;
            REAL(kkstem)[dayi] = dbpS.kStem;
            REAL(kksugar)[dayi]= dbpS.kSugar;
            REAL(kkfiber)[dayi]= dbpS.kFiber;
            REAL(kkroot)[dayi] = dbpS.kRoot;
            REAL(kkseedcane)[dayi] = dbpS.kSeedcane;
            REAL(dailyThermalT)[dayi]= TTc;


            if(kLeaf >= 0) {
                newLeaf =  dailyassim * kLeaf;
                *(sti+i) = newLeaf; 
            } else {
                warning("kleaf is negative");
            }

            if(TTc < seneLeaf) {
                Leaf += newLeaf;
                leaflitter = leaflitter+0.0;
                REAL(LeafLittervec)[dayi]= leaflitter;
            } else {
                deadleaf = (leafrate*Leaf)/100.0;  /* biomass of dead leaf */
                remobilizedleaf = deadleaf*Remobfactorleaf;
                leaflitter = leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
                Leaf =Leaf+newLeaf-deadleaf+remobilizedleaf*33;
                Stem = Stem+remobilizedleaf*0.33;
                Root = Root+remobilizedleaf*0.33;
                REAL(LeafLittervec)[dayi]= leaflitter; /* Collecting the leaf litter */ 
                /*	 REAL(LeafLittervec)[dayi] = Litter * 0.4;  */ /* Collecting the leaf litter */       
            }

        /* New Stem*/
            if(kStem >= 0) {
                newStem =  dailyassim* kStem ;
                newSugar= dailyassim *kSugar;
                newFiber = dailyassim *kFiber;
            } else {
                warning("kstem is negative");
            }

            if(TTc < seneStem) {
                Stem += newStem;
                Fiber+=newFiber;
                Sugar+=newSugar;
            } else {
                Stem = Stem+newStem;  
                Fiber = Fiber+newFiber;
                Sugar = Sugar+newSugar;

            }

            if(kRoot >= 0) {           
                newRoot =  dailyassim * kRoot ;
                if(newRoot>=dailyRootResp)
                {
                    newRoot = newRoot;
                    *(sti3+i) = newRoot;
                } else {
                    newRoot = 0.0;
                    newSugar = newRoot-dailyRootResp; //Stem growth can not be negative. Set it zero and rest is taken care by sugar
                }

            } else {
                error("kRoot should be positive");
            }

            if(TTc < seneRoot) {
                Root += newRoot;
                rootlitter = 0.0;
                REAL(RootLittervec)[dayi]= rootlitter;

            } else {
                deadroot = (rootrate*Root)/100.0;  /* biomass of dead root */
                Root =Root+newRoot-deadroot;
                rootlitter = rootlitter+deadroot;
                REAL(RootLittervec)[dayi]= rootlitter; 
            }

            if(kSeedcane > 0) {
                newSeedcane = dailyassim * kSeedcane ;	
                if(newSeedcane>0) {
                    *(sti4+ri) = newSeedcane;
                } else {
                    newSeedcane = 0;
                }
                ri++;
            } else {
                if(Seedcane < 0) {
                    Seedcane = 1e-4;
                    warning("Seedcane became negative");
                }

                newSeedcane = Seedcane * kSeedcane;
                Root += kRoot * -newSeedcane ;
                Stem += kStem * -newSeedcane ;
                Leaf += kLeaf * -newSeedcane ;
                Seedcane+=newSeedcane;	
            }

            /*	 newSugar = newStem*dbpS.kSugar;
                 Sugar = Sugar+newSugar;   */

            ALitter = REAL(LeafLittervec)[i] + REAL(StemLittervec)[i];
            BLitter = REAL(RootLittervec)[i];

            /*daily update of specific leaf area*/
            Sp = Sp - SpD;
            if (Sp < 0.5) Sp = 0.5;

            /* new value of leaf area index */
            LAInew = Leaf * Sp ;

            if(wsFun!=3) {
                LAI = (LAInew-LAIold)*LeafWS+LAIold;
            } else {
                LAI = LAInew;
            }

            /* apply leaf water stress based on rainfed (wsFun = 0,1,2) or irrigated system (wsFun = 3)*/

            //              if(wsFun ==3)
            //		 { 
            //		 LAI = LAInew;
            //		}
            //		else
            //		 { 
            //		 LAI = LAIold+(LAInew-LAIold)*LeafWS;
            //		}
            //		LAIold = LAI;

            // Implementing LeafWS on Leaf Area  Index
            //LAI = LAInew;		  

            /* daily update of leaf-N, vmac, and alpga  based on cyclic seasonal variation */
            currentday = doy[i];
            LeafN = seasonal(LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,365.0,lat);

            /**This calculation is not needed because LeafN is used to calculate photosynthesis paraetrers in canopy module*********/ 
            /*
               if(nitrop.lnFun == 0) {
               vmax = vmax1;
               Rd = Rd;
               alpha = alpha1;
               } else {
               vmax = nitrop.Vmaxb1*LeafN+nitrop.Vmaxb0;
               if(vmax<0) vmax = 0.0;
               alpha = nitrop.alphab1*LeafN+nitrop.alphab0;
               Rd = nitrop.Rdb1*LeafN+nitrop.Rdb0;
               }
             ******************************************************************************/

            // Here, I am checking for maximum LAI based on minimum SLN = 50 is required
            // If LAI > LAI max then additional senescence of leaf take place

            laimax = (-1)*log((40.0/LeafN))/(kpLN);

            if(LAI > laimax) {
                LAI = laimax;
                deadleaf = (LAI-laimax)/Sp;  /* biomass of dead leaf */
                remobilizedleaf = deadleaf*Remobfactorleaf;
                leaflitter = leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
                Leaf =Leaf-deadleaf+remobilizedleaf*33;
                Stem = Stem+remobilizedleaf*0.33;
                Root = Root+remobilizedleaf*0.33;
            }
            // Here I am simulating leaf Frost damage
            if(temp[i]<frostparms.leafT0)
            {
                deadleaf = getFrostdamage(frostparms.leafT0,frostparms.leafT100,temp[i], Leaf);
                remobilizedleaf = deadleaf*Remobfactorleaf;
                leaflitter = leaflitter+deadleaf-remobilizedleaf; /*40% of dead leaf goes to leaf litter */
                Leaf =Leaf-deadleaf+remobilizedleaf*0.0;
                Stem = Stem+remobilizedleaf*0.50;
                Root = Root+remobilizedleaf*0.50;
            }

            /*reset daily assimilation and daily transpiratin to zero for the next day */
            dailyassim = 0.0;
            dailytransp = 0.0;

            /*reset daily maintenance respiration to zero for the next day */
            dailyLeafResp = 0.0;
            dailyStemResp = 0.0;
            dailyRootResp = 0.0;
            // dailyRhizomeResp = 0.0; unused

            REAL(DayofYear)[dayi] =  INTEGER(DOY)[i];         
            REAL(Leafy)[dayi] = Leaf;
            REAL(Stemy)[dayi] = Stem;
            REAL(Rooty)[dayi] =  Root;
            REAL(Seedcaney)[dayi] = Seedcane;
            REAL(LAIc)[dayi] = LAI;
            REAL(LeafNitrogen)[dayi] = LeafN;
            REAL(AboveLitter)[dayi] = ALitter;
            REAL(BelowLitter)[dayi] = BLitter;
            REAL(VmaxVec)[dayi] = vmax;
            REAL(AlphaVec)[dayi] = alpha;
            REAL(SpVec)[dayi] = Sp;
            REAL(Fibery)[dayi] = Fiber;
            REAL(Sugary)[dayi] = Sugar;
            ddap = (double)dayi;
            REAL(dayafterplanting)[dayi] = ddap;
            moist = (5.72*Stem-7.16*Sugar);
            REAL(moisturecontent)[dayi] = moist;
            dayi = dayi+1;
        } /*daily loop ends here */



        /* here I am writing REMAINING variables at hourly time step */
        REAL(Hour)[i] =  INTEGER(HR)[i];
		REAL(TTTc)[i] = TTc;
        REAL(CanopyAssim)[i] =  CanopyA;
        REAL(CanopyTrans)[i] =  CanopyT; 
        REAL(SoilWatCont)[i] = waterCont;
        REAL(StomatalCondCoefs)[i] = StomataWS;
        REAL(LeafReductionCoefs)[i] = LeafWS;
        REAL(SoilEvaporation)[i] = soilEvap;
        REAL(LeafPsimVec)[i] = LeafPsim;

        for(int layer = 0; layer < soilLayers; layer++) {
            REAL(psimMat)[layer + i * soilLayers] = psi[layer + i * soilLayers];
            REAL(cwsMat)[layer + i * soilLayers] = water_status[layer + i * soilLayers];
            REAL(rdMat)[layer + i * soilLayers] = root_distribution[layer + i * soilLayers];
        }

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

