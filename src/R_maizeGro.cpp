/*
 *  BioCro/src/maizeGro.c by Fernando Ezequiel Miguez  Copyright (C) 2012
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "BioCro.h"
#include "Century.h"
#include "R_maizeGro.h" 
#include "AuxMaizeGro.h"

SEXP maizeGro(

        SEXP DOY,                   /* Day of the year                   1 */
        SEXP HR,                    /* Hour                              2 */
        SEXP SOLAR,                 /* Solar radiation                   3 */
        SEXP TEMP,                  /* Temperature                       4 */
        SEXP RH,                    /* Relative Humidity                 5 */
        SEXP WINDSPEED,             /* Windspeed                         6 */
        SEXP PRECIP,                /* Precipitation                     7 */ 
        SEXP DATES,                 /* Plant, Emerge, and Harvest Dates  8 */
        SEXP PHENOP,                /* Phenology Parameters for Corn     9 */
        SEXP PHOTOP,                /* Photosynthetic parameters        10 */
        SEXP CANOPYP,               /* Canopy parameters                11 */
        SEXP NITROP,                /* Nitro parameters                 12 */
        SEXP LAIP,                  /* LAI   parameters                 13 */
        SEXP MALLOCP,               /* MALLOC parameters                14 */
        SEXP LAT,                   /* Latitude                         15 */ 
        SEXP TIMESTEP,              /* Timestep , default 1 hour        16 */
        SEXP PLANTDENSITY,          /* plant density                    17 */
        SEXP SOILP,                 /* Soil parameters                  18 */
        SEXP SOILP2,                /* Soil parameters 2                19 */
        SEXP SOILDEPTHS,            /* Soil depths                      20 */
        SEXP CWS,                    /* Current water status            21 */
        SEXP SENEP,
        SEXP NNITROP                 /* Maize senescence parameteres    22*/
        )
{

    double lat = REAL(LAT)[0];
    int *doy = INTEGER(DOY);
    int *hr = INTEGER(HR);
    double *solar = REAL(SOLAR);
    double *temp = REAL(TEMP);
    double *rh = REAL(RH);
    double *windspeed = REAL(WINDSPEED);
    double *precip = REAL(PRECIP);
    double kd = REAL(CANOPYP)[3];
    double chil = REAL(CANOPYP)[4];
	

    double heightf = REAL(CANOPYP)[7];
    double nlayers = REAL(CANOPYP)[2];
    int timestep = INTEGER(TIMESTEP)[0];
	int vecsize;
    double Sp = REAL(CANOPYP)[0];
    // double SpD = REAL(CANOPYP)[1]; unused
    double vmax11 = REAL(PHOTOP)[0];
    double vmax12 = REAL(PHOTOP)[1];
    double alpha1 = REAL(PHOTOP)[2];
    double kparm = REAL(PHOTOP)[3];
    double theta = REAL(PHOTOP)[4];
    double beta = REAL(PHOTOP)[5];
    double Rd11 = REAL(PHOTOP)[6];
    double Rd12 = REAL(PHOTOP)[7];
    double Catm = REAL(PHOTOP)[8];
    double b0 = REAL(PHOTOP)[9];
    double b1 = REAL(PHOTOP)[10];
    double *soilcoefs = REAL(SOILP);
    double iLeafN = REAL(NITROP)[0];
    double kLN = REAL(NITROP)[1];
    double vmax_b1 = REAL(NITROP)[2];
    double alpha_b1 = REAL(NITROP)[3];
    int soilType = INTEGER(SOILP2)[0];
    int wsFun = INTEGER(SOILP2)[2];
    double ws = REAL(PHOTOP)[11];
    int soilLayers = INTEGER(SOILP2)[1];
    double *soilDepths = REAL(SOILDEPTHS);
    double *cws = REAL(CWS);
    int hydrDist = INTEGER(SOILP2)[3];
    double kpLN = REAL(NITROP)[4];
    double lnb0 = REAL(NITROP)[5];
    double lnb1 = REAL(NITROP)[6];
    int lnFun = REAL(NITROP)[7];

    double upperT = REAL(PHOTOP)[12];
    double lowerT = REAL(PHOTOP)[13];

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

    SEXP lists, names;

    SEXP DayofYear;
    SEXP Hour;
    SEXP PhenoStage;
    SEXP CanopyAssim;
    SEXP CanopyTrans;
    SEXP LeafVec;
    SEXP StemVec;
    SEXP RootVec;
    SEXP GrainVec;
    SEXP LAIc;
    SEXP LAImat;
    SEXP TTTc;
    SEXP SoilWatCont;
    SEXP StomatalCondCoefs;
    SEXP VmaxVec;
    SEXP LeafNVec;
    SEXP SoilEvaporation;
    SEXP cwsMat;
    SEXP psimMat; /* Holds the soil water potential */
    SEXP rdMat;
    SEXP SCpools;
    SEXP SNpools;
    SEXP LeafPsimVec;

    vecsize = 8760 ; /* 365 days * 24 hours  */
    PROTECT(lists = allocVector(VECSXP,23));
    PROTECT(names = allocVector(STRSXP,23));

    PROTECT(DayofYear = allocVector(REALSXP,vecsize));
    PROTECT(Hour = allocVector(REALSXP,vecsize));
    PROTECT(PhenoStage = allocVector(REALSXP,vecsize));
    PROTECT(CanopyAssim = allocVector(REALSXP,vecsize));
    PROTECT(CanopyTrans = allocVector(REALSXP,vecsize));
    PROTECT(LeafVec = allocVector(REALSXP,vecsize));
    PROTECT(StemVec = allocVector(REALSXP,vecsize));
    PROTECT(RootVec = allocVector(REALSXP,vecsize));
    PROTECT(GrainVec = allocVector(REALSXP,vecsize));
    PROTECT(LAIc = allocVector(REALSXP,vecsize));
    PROTECT(LAImat = allocMatrix(REALSXP,MAXLEAFNUMBER,vecsize));
    PROTECT(TTTc = allocVector(REALSXP,vecsize));
    PROTECT(SoilWatCont = allocVector(REALSXP,vecsize));
    PROTECT(StomatalCondCoefs = allocVector(REALSXP,vecsize));
    PROTECT(VmaxVec = allocVector(REALSXP,vecsize));
    PROTECT(LeafNVec = allocVector(REALSXP,vecsize));
    PROTECT(SoilEvaporation = allocVector(REALSXP,vecsize));
    PROTECT(cwsMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(psimMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(rdMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(SCpools = allocVector(REALSXP,9));
    PROTECT(SNpools = allocVector(REALSXP,9));
    PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));

    double newLeafcol[8760];
    double newStemcol[8760];
    double newRootcol[8760];

    int i, i2, i3, i4;

    double Leaf = 0.0, Stem = 0.0, Root = 0.0, LAI = 0.0, Grain = 0.0;
    double TTc = 0.0, TTc_V10 = 0.0;
    double kLeaf, kStem, kRoot, kGrain;
    double newLeaf, newStem, newRoot, newGrain;

    double *sti, *sti2, *sti3;
    int k = 0, j = 0, q = 0, m = 0;

    double StomWS = 1, LeafWS = 1;
    double CanopyA, CanopyT;
    double LeafN = iLeafN;
    double LeafNR = iLeafN;
    double vmax;
    double alpha = alpha1;

    /* Maintenance respiration */
    const double mrc1 = REAL(CANOPYP)[5];
    // const double mrc2 = REAL(CANOPYP)[6];

    const double FieldC = soilcoefs[0];
    const double WiltP = soilcoefs[1];
    const double phi1 = soilcoefs[2];
    const double phi2 = soilcoefs[3];
    const double soilDepth = soilcoefs[4];
    double waterCont = 0.3;
    double soilEvap, TotEvap;

    const double seneStem = REAL(SENEP)[0];
    const double seneLeaf = REAL(SENEP)[1];
    const double seneRoot = REAL(SENEP)[2];

    struct Can_Str Canopy = {0,0,0};
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    struct maize_dbp_str dbpS;
    struct lai_str tmpLAI;
    struct soilML_str soilMLS;
    struct soilText_str soTexS; 
    soTexS = soilTchoose(soilType);

    /* Creation of pointers outside the loop */
    sti = &newLeafcol[0];
    sti2 = &newStemcol[0];
    sti3 = &newRootcol[0];

    /* Some soil related empirical coefficients */
    double rfl = REAL(SOILP)[8];  /* root factor lambda */
    double rsec = REAL(SOILP)[9]; /* radiation soil evaporation coefficient */
    double rsdf = REAL(SOILP)[10]; /* root soil depth factor */
    double scsf = REAL(SOILP)[5]; /* stomatal conductance sensitivity factor */
    double transpRes = REAL(SOILP)[6]; /* Resistance to transpiration from soil to leaf */
    double leafPotTh = REAL(SOILP)[7]; /* Leaf water potential threshold */

    int plantDate, emergeDate, harvestDate;
    double baseTemp;
    // double maxLeaves; unused
    double plantEmerge, phyllochron1, phyllochron2;
    double R1, R2, R3, R4, R5, R6;
    double phenoStage = 0.0;

    /* Variables for photosynthesis */
    double vmax1, Rd;

    /* Variables for leaf area index */
    double laiMethod, TTcoef, maxLAI;
    double Ax, LT, a1, a2, k0, L0, LLx, Lx, LNl, Amax, c1, c2, c3, 
           c4, a, b, x0, f, g;

    double plantdensity = REAL(PLANTDENSITY)[0];

    double cwsVec[soilLayers];
    for(i2 = 0; i2 < soilLayers; i2++) {
        cwsVec[i2] = cws[i2];
    } 
    double cwsVecSum = 0.0;
    /* Parameters for calculating leaf water potential */
    double LeafPsim = 0.0;

    plantDate = INTEGER(DATES)[0];
    emergeDate = INTEGER(DATES)[1];
    harvestDate = INTEGER(DATES)[2];

    /* Extracting phenology parameters */

    baseTemp = REAL(PHENOP)[0];
    // maxLeaves = REAL(PHENOP)[1]; unused
    plantEmerge = REAL(PHENOP)[2];
    phyllochron1 = REAL(PHENOP)[3];
    phyllochron2 = REAL(PHENOP)[4];
    R1 = REAL(PHENOP)[5];
    R2 = REAL(PHENOP)[6];
    R3 = REAL(PHENOP)[7];
    R4 = REAL(PHENOP)[8];
    R5 = REAL(PHENOP)[9];
    R6 = REAL(PHENOP)[10];

    /* Extracting photosynthesis parameters */

    vmax = vmax11;


    /* Extracting LAI parms */
    laiMethod = REAL(LAIP)[0];
    TTcoef = REAL(LAIP)[1];
    maxLAI = REAL(LAIP)[2];
    Ax = REAL(LAIP)[3];
    LT = REAL(LAIP)[4];
    a1 = REAL(LAIP)[5];
    a2 = REAL(LAIP)[6];
    k0 = REAL(LAIP)[7];
    L0 = REAL(LAIP)[8];
    LLx = REAL(LAIP)[9];
    Lx = REAL(LAIP)[10];
    LNl = REAL(LAIP)[11];
    Amax = REAL(LAIP)[12];
    c1 = REAL(LAIP)[13];
    c2 = REAL(LAIP)[14];
    c3 = REAL(LAIP)[15];
    c4 = REAL(LAIP)[16];
    a = REAL(LAIP)[17];
    b = REAL(LAIP)[18];
    x0 = REAL(LAIP)[19];
    f = REAL(LAIP)[20];
    g = REAL(LAIP)[21];


    for(i = 0; i < vecsize; i++)
	{
        /* First calculate the elapsed Thermal Time*/
        /* The idea is that here I need to divide by the time step
           to calculate the thermal time. For example, a 3 hour time interval
           would mean that the division would need to by 8 */

        /* If before planting date phenology is not calculated */
        if(doy[i] < plantDate) {
            REAL(PhenoStage)[i] = -1;
            REAL(TTTc)[i] = 0;
        } else {

            if(doy[i] >= harvestDate) {
                REAL(TTTc)[i] = TTc;
                REAL(PhenoStage)[i] = -1;
            } else {
                if(temp[i] > baseTemp) {
                    TTc += (temp[i] - baseTemp)  / (24/timestep); 
                }
                REAL(TTTc)[i] = TTc; 

                if(TTc < plantEmerge || doy[i] < emergeDate) {
                    REAL(PhenoStage)[i] = 0.0;
                } else {
                    if(REAL(PhenoStage)[i-1] < 0.10) {
                        REAL(PhenoStage)[i] = floor(TTc / phyllochron1) / 100;
                        TTc_V10 = TTc;
                    } else
                        if(REAL(PhenoStage)[i-1] < 1) {
                            REAL(PhenoStage)[i] = 0.10 + floor((TTc - TTc_V10) / phyllochron2) / 100;
                            // TTc_Vn = TTc; unused
                        }

                    if(TTc > R1) REAL(PhenoStage)[i] = 1;

                    if(TTc > R2) REAL(PhenoStage)[i] = 2;

                    if(TTc > R3) REAL(PhenoStage)[i] = 3;

                    if(TTc > R4) REAL(PhenoStage)[i] = 4;

                    if(TTc > R5) REAL(PhenoStage)[i] = 5;

                    if(TTc > R6) REAL(PhenoStage)[i] = 6;

                    /* if(doy[i] >= harvestDate) REAL(PhenoStage)[i] = -1; */
                }

            }
        }

        /* There are several possible approaches to modleing LAI. One is solely based on thermal time */
        /* another one is based on specific leaf area and the third one in based on modeling individual leaves */
        /* citations below */

        /* LAI following Lizaso J, Batchelor WD, Westgate ME. A
         * leaf area model to simulate cultivar-specific
         * expansion and senescence of maize leaves. Field
         * Crops Research. 2003;80(1):1-17. Available at:
         * http://linkinghub.elsevier.com/retrieve/pii/S037842900200151X. */

        phenoStage = REAL(PhenoStage)[i];
        if(doy[i] < emergeDate) {
            LAI = 0.0;
            CanopyA = 0.0;
            CanopyT = 0.0;
            for(i4=0;i4<MAXLEAFNUMBER;i4++) {
                REAL(LAImat)[i4 + i*MAXLEAFNUMBER] = 0.0;
            }
        } else {
            if(laiMethod == 0.0) {
                /* Implementing the simplest method based on thermal time */
                LAI = fabs(TTc * TTcoef - plantEmerge * TTcoef);
            }

            if(laiMethod == 1.0) {
                LAI = Leaf * Sp + 1e-3;
            }

            if(laiMethod == 2.0) {
                tmpLAI = laiLizasoFun(TTc, phenoStage, phyllochron1,
                        phyllochron2, Ax, LT, k0,
                        a1, a2, L0, LLx, Lx, LNl);

                for(i4=0;i4<MAXLEAFNUMBER;i4++) {
                    REAL(LAImat)[i4 + i*MAXLEAFNUMBER] = tmpLAI.leafarea[i4];
                }
                LAI = tmpLAI.totalLeafArea * 1e-4 * plantdensity; 
            }

            if(laiMethod == 3.0) {
                tmpLAI = laiBirchdiscontinuousFun(phenoStage, LT, Amax, 
                        c1, c2, c3, c4); 
                /* Amax = area of largest leaf (-1 if unspecified in which
                 * case it will be calculated by Birch eq 13), c1 is coefficient
                 * for Birch eq 15, c2 is coefficient for Birch eq 16, c3 and c4
                 * are the first and second coefficients in Birch eq 17
                 */
                for (i4 = 0; i4 < MAXLEAFNUMBER; i4++) {
                    REAL(LAImat)[i4 + i * MAXLEAFNUMBER] = tmpLAI.leafarea[i4];
                }
                LAI = tmpLAI.totalLeafArea * 1e-4 * plantdensity;
            }

            if(laiMethod == 4.0) {
                tmpLAI = laiBirchcontinuousFun(phenoStage, a, b, Amax, x0, 
                        f, g, TTc, LT);
                /* a and b are coefficients to Birch Eq 5, x0 is leaf number
                 * of largest leaf, and f and g are coefficients to Birch
                 * Eq 11.
                 */
                for (i4 = 0; i4 < MAXLEAFNUMBER; i4++) {
                    REAL(LAImat)[i4 + i * MAXLEAFNUMBER] = tmpLAI.leafarea[i4];
                }
                LAI = tmpLAI.totalLeafArea / 1e4 * plantdensity;
            }
            /* Total leaf area will be in cm2, for a meter squared I need to divide by 1e4*/
            if(LAI > maxLAI) LAI = maxLAI;
            if(LAI < 1e-3) LAI = 1e-3;
        }


        /* There is a different value of Rd for vegetative vs. reproductive */
        if(phenoStage < 1) {
            Rd = Rd11;
            vmax1 = vmax11;
        } else {
            Rd = Rd12;
            vmax1 = vmax12;
        }

        /* Calculate Canopy Assimilation  */
        /* Canopy Assimilation should only happen between emergence and R6 */
        /* if(doy[i] > emergeDate && doy[i] < harvestDate) { //} */
        if(doy[i] > emergeDate && TTc < R6) {

            Canopy = CanAC(LAI, doy[i], hr[i],
                    solar[i], temp[i], rh[i], windspeed[i],
                    lat, nlayers, vmax, alpha, kparm, beta,
					Rd, Catm, b0, b1, theta, kd, chil,
					heightf, LeafN, kpLN, lnb0, lnb1, lnFun, upperT, lowerT, nitrop, 0.04, 0, StomWS, ws);

            CanopyA = Canopy.Assim * timestep;
            CanopyT = Canopy.Trans * timestep;
            q++;

            /* Rprintf("LAI: %.4f, vmax: %.1f",LAI,vmax);  */
        } else {
            CanopyA = 0.0;
            CanopyT = 0.0;
        }

        /* Section about N deficiency on Vmax */
        /* The equation below works well for the vegetative part */
        if(phenoStage < 1) {
            LeafN = iLeafN * pow(Stem + Leaf,-kLN); 
            if(LeafN > iLeafN) LeafN = iLeafN;
            LeafNR = LeafN;
        } else {
            LeafN = LeafNR - (TTc - R1) * (kLN * 0.0045);
            if(LeafN < 1e-3) LeafN = 1e-3;  
        }

        if(phenoStage == -1) LeafN = 0;

        /* vmax = (iLeafN - LeafN) * vmax_b1 + vmax1; */
        vmax = vmax1 * (2/(1 + exp(-vmax_b1*(LeafN - 0.02))) - 1); 
        /* if(vmax < 0) { */
        /* 	Rprintf("vmax1: %.1f, vmax_b1: %.3f, LeafN: %.1f",vmax1,vmax_b1,LeafN); */
        /*         error("stop here"); */
        /* } */

        alpha = (iLeafN - LeafN) * alpha_b1 + alpha1; 


        /* Inserting the multilayer model */
        if(soilLayers > 1) {
            soilMLS = soilML(precip[i], CanopyT, &cwsVec[0], soilDepth, soilDepths, FieldC, WiltP,
                    phi1, phi2, soTexS, wsFun, soilLayers, Root, 
                    LAI, 0.68, temp[i], solar[i], windspeed[i], rh[i], 
                    hydrDist, rfl, rsec, rsdf);

            StomWS = soilMLS.rcoefPhoto;
            LeafWS = soilMLS.rcoefSpleaf;
            soilEvap = soilMLS.SoilEvapo;

            for(i3 = 0; i3 < soilLayers; i3++) {
                cwsVec[i3] = soilMLS.cws[i3];
                cwsVecSum += cwsVec[i3];
                REAL(cwsMat)[i3 + i*soilLayers] = soilMLS.cws[i3];
                REAL(rdMat)[i3 + i*soilLayers] = soilMLS.rootDist[i3];
            }
            waterCont = cwsVecSum / soilLayers;
            cwsVecSum = 0.0;
        } else {
            soilEvap = SoilEvapo(LAI, 0.68, temp[i], solar[i], waterCont, FieldC, WiltP, windspeed[i], rh[i], rsec);
            TotEvap = soilEvap + CanopyT;
            WaterS = watstr(precip[i], TotEvap, waterCont, soilDepth, FieldC, WiltP, phi1, phi2, soilType, wsFun);   
            waterCont = WaterS.awc;
            StomWS = WaterS.rcoefPhoto ; 
            LeafWS = WaterS.rcoefSpleaf;
            REAL(cwsMat)[i] = waterCont;
            REAL(psimMat)[i] = WaterS.psim;
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
             * Transpiration is in Mg ha-2 hr-1
             * Multiply by 1e3 to go from Mg to kg
             * Multiply by 1e-4 to go from ha to m^2 
             * This needs to go from hours to seconds that's
             * why the conversion factor is (1/3600).*/
            LeafPsim = WaterS.psim - (CanopyT * 1e3 * 1e-4 * 1.0/3600.0) * transpRes;

            /* From WIMOVAVC the proposed equation to simulate the effect of water
             * stress on stomatal conductance */
            if(LeafPsim < leafPotTh) {
                /* StomWS = 1 - ((LeafPsim - leafPotTh)/1000 *
                 * scsf); In WIMOVAC this equation is used but
                 * the absolute values are taken from the
                 * potentials. Since they both should be
                 * negative and leafPotTh is greater than
                 * LeafPsim this can be rearranged to*/ 
                StomWS = 1 - ((leafPotTh - LeafPsim)/1000 * scsf);
                /* StomWS = 1; */
                if(StomWS < 0.1) StomWS = 0.1;
            } else {
                StomWS = 1;
            }
        } else {
            LeafPsim = 0;
        }

        /* Need to incoporate the partitioning of carbon to plant components  */
        dbpS = maize_sel_dbp_coef(REAL(MALLOCP), phenoStage);

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;

        /* if(i == 300) { */
        /* 	Rprintf("kLeaf %.2f",kLeaf,"\n"); */
        /* 	Rprintf("kStem %.2f",kStem,"\n"); */
        /* 	Rprintf("kRoot %.2f",kRoot,"\n"); */
        /* 	Rprintf("kGrain %.2f",kGrain,"\n"); */
        /* } */
        /* Creating the increase in biomass for each component */

        if(kLeaf > 0 || TTc > R6) {
            newLeaf = CanopyA * kLeaf * LeafWS;
            newLeaf = resp(newLeaf, mrc1, temp[i]);
        } else {
            newLeaf = Leaf * kLeaf;
            Stem += kStem * -newLeaf * 0.9;
            Root += kRoot * -newLeaf * 0.9;
            Grain += kGrain * -newLeaf * 0.9;
        }

        if(kStem > 0 || TTc > R6) {
            newStem = CanopyA * kStem;
            newStem = resp(newStem, mrc1, temp[i]);
        } else {
            newStem = Stem * kStem;
            Leaf += kLeaf * -newStem   * 0.9;
            Root += kRoot * -newStem * 0.9;
            Grain += kGrain * -newStem * 0.9;
        }

        newRoot = CanopyA * kRoot;

        /* I need a better implementation of reproductive specific stress */
        if(LeafWS < StomWS) {
            newGrain = CanopyA * kGrain * LeafWS;
        } else {
            newGrain = CanopyA * kGrain * StomWS;
        }


        if(doy[i] < emergeDate + 7) {
            if(newLeaf < 0) newLeaf = 0.0;
            if(newStem < 0) newStem = 0.0;
            if(newRoot < 0) newRoot = 0.0;
            if(newGrain < 0) newGrain = 0.0;
        }

        Grain += newGrain;

        /* Implementing senescence */
        *(sti+q) = newLeaf;
        *(sti2+q) = newStem;
        *(sti3+q) = newRoot;

        /* Senescence for leaf */
        if(TTc < seneLeaf || TTc > R6) {
            Leaf += newLeaf;
        } else {
            Leaf += newLeaf - *(sti+k);
            k++;
        }

        /* Senescence for stem */
        if(TTc < seneStem || TTc > R6) {
            Stem += newStem;
        } else {
            Stem += newStem - *(sti2+j);
            j++;
        }

        /* Senescence for root */
        if(TTc < seneRoot || TTc > R6) {
            Root += newRoot;
        } else {
            Root += newRoot - *(sti3+m);
            m++;
        }

        /* Collecting results */
        REAL(DayofYear)[i] =  doy[i];
        REAL(Hour)[i] =  hr[i];
        REAL(CanopyAssim)[i] =  CanopyA;
        REAL(CanopyTrans)[i] =  CanopyT; 
        REAL(LAIc)[i] = LAI;
        REAL(LeafVec)[i] = Leaf;
        REAL(StemVec)[i] = Stem;
        REAL(RootVec)[i] = Root;
        REAL(GrainVec)[i] = Grain;
        REAL(VmaxVec)[i] = vmax;
        REAL(LeafNVec)[i] = LeafN;
        REAL(SoilEvaporation)[i] = soilEvap;
        REAL(LeafPsimVec)[i] = LeafPsim;
        REAL(SoilWatCont)[i] = waterCont;
        REAL(StomatalCondCoefs)[i] = StomWS;
    }

    /* Populating the results of the Century model */
    REAL(SCpools)[0] = 1;
    REAL(SCpools)[1] = 1;
    REAL(SCpools)[2] = 1;
    REAL(SCpools)[3] = 1;
    REAL(SCpools)[4] = 1;
    REAL(SCpools)[5] = 1;
    REAL(SCpools)[6] = 1;
    REAL(SCpools)[7] = 1;
    REAL(SCpools)[8] = 1;

    REAL(SNpools)[0] = 1;
    REAL(SNpools)[1] = 1;
    REAL(SNpools)[2] = 1;
    REAL(SNpools)[3] = 1;
    REAL(SNpools)[4] = 1;
    REAL(SNpools)[5] = 1;
    REAL(SNpools)[6] = 1;
    REAL(SNpools)[7] = 1;
    REAL(SNpools)[8] = 1;

    SET_VECTOR_ELT(lists, 0, DayofYear);
    SET_VECTOR_ELT(lists, 1, Hour);
    SET_VECTOR_ELT(lists, 2, TTTc);
    SET_VECTOR_ELT(lists, 3, PhenoStage);
    SET_VECTOR_ELT(lists, 4, CanopyAssim);
    SET_VECTOR_ELT(lists, 5, CanopyTrans);
    SET_VECTOR_ELT(lists, 6, LAIc);
    SET_VECTOR_ELT(lists, 7, LeafVec);
    SET_VECTOR_ELT(lists, 8, StemVec);
    SET_VECTOR_ELT(lists, 9, RootVec);
    SET_VECTOR_ELT(lists, 10, GrainVec);
    SET_VECTOR_ELT(lists, 11, LAImat);
    SET_VECTOR_ELT(lists, 12, VmaxVec);
    SET_VECTOR_ELT(lists, 13, LeafNVec);
    SET_VECTOR_ELT(lists, 14, cwsMat);
    SET_VECTOR_ELT(lists, 15, psimMat);
    SET_VECTOR_ELT(lists, 16, rdMat);
    SET_VECTOR_ELT(lists, 17, SCpools);
    SET_VECTOR_ELT(lists, 18, SNpools);
    SET_VECTOR_ELT(lists, 19, LeafPsimVec);
    SET_VECTOR_ELT(lists, 20, SoilEvaporation);
    SET_VECTOR_ELT(lists, 21, SoilWatCont);
    SET_VECTOR_ELT(lists, 22, StomatalCondCoefs);

    SET_STRING_ELT(names,0,mkChar("DayofYear"));
    SET_STRING_ELT(names,1,mkChar("Hour"));
    SET_STRING_ELT(names,2,mkChar("TTTc"));
    SET_STRING_ELT(names,3,mkChar("PhenoStage"));
    SET_STRING_ELT(names,4,mkChar("CanopyAssim"));
    SET_STRING_ELT(names,5,mkChar("CanopyTrans"));
    SET_STRING_ELT(names,6,mkChar("LAI"));
    SET_STRING_ELT(names,7,mkChar("Leaf"));
    SET_STRING_ELT(names,8,mkChar("Stem"));
    SET_STRING_ELT(names,9,mkChar("Root"));
    SET_STRING_ELT(names,10,mkChar("Grain"));
    SET_STRING_ELT(names,11,mkChar("LAImat"));
    SET_STRING_ELT(names,12,mkChar("VmaxVec"));
    SET_STRING_ELT(names,13,mkChar("LeafNVec"));
    SET_STRING_ELT(names,14,mkChar("cwsMat"));
    SET_STRING_ELT(names,15,mkChar("psimMat"));
    SET_STRING_ELT(names,16,mkChar("rdMat"));
    SET_STRING_ELT(names,17,mkChar("SCpools"));
    SET_STRING_ELT(names,18,mkChar("SNpools"));
    SET_STRING_ELT(names,19,mkChar("LeafPsimVec"));
    SET_STRING_ELT(names,20,mkChar("SoilEvaporation"));
    SET_STRING_ELT(names,21,mkChar("SoilWatCont"));
    SET_STRING_ELT(names,22,mkChar("StomatalCondCoefs"));

    setAttrib(lists,R_NamesSymbol,names);
    UNPROTECT(25);
    return(lists);
}
