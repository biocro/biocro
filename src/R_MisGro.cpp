/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <R.h>
#include <Rinternals.h>
#include "BioCro.h"

extern "C" {

SEXP MisGro(
        SEXP LAT,              /* Latitude                            1 */
        SEXP DOY,              /* Day of the year                     2 */
        SEXP HR,               /* Hour of the day                     3 */
        SEXP SOLAR,            /* Solar Radiation                     4 */
        SEXP TEMP,             /* Temperature                         5 */
        SEXP RH,               /* Relative humidity                   6 */
        SEXP WINDSPEED,        /* Wind Speed                          7 */
        SEXP PRECIP,           /* Precipitation                       8 */
        SEXP KD,               /* K D (ext coeff diff)                9 */
        SEXP CHIL,             /* Chi, leaf angle distribution       10 */
        SEXP LEAFWIDTH,        /* Width of a leaf                    11 */
        SEXP ET_EQUATION,      /* Integer to indicate ET equation    12 */
        SEXP HEIGHTF,          /* Height factor                      13 */
        SEXP NLAYERS,          /* Number of layers in the canopy     14 */
		SEXP INITIAL_BIOMASS,
        SEXP SENCOEFS,         /* sene coefs                         17 */
        SEXP TIMESTEP,         /* time step                          18 */
        SEXP VECSIZE,          /* vector size                        19 */
        SEXP SPLEAF,           /* Spec Leaf Area                     20 */
        SEXP SPD,              /* Spec Lefa Area Dec                 21 */
        SEXP DBPCOEFS,         /* Dry Bio Coefs                      22 */
        SEXP THERMALP,         /* Themal Periods                     23 */
		SEXP THERMAL_BASE_TEMP,/* Base temperature of GDD               */
        SEXP VMAX,             /* Vmax of photo                      24 */
        SEXP ALPHA,            /* Quantum yield                      25 */
        SEXP KPARM,            /* k parameter (photo)                26 */
        SEXP THETA,            /* theta param (photo)                27 */
        SEXP BETA,             /* beta param  (photo)                28 */
        SEXP RD,               /* Dark Resp   (photo)                29 */
        SEXP CATM,             /* CO2 atmosph                        30 */
        SEXP B0,               /* Int (Ball-Berry)                   31 */
        SEXP B1,               /* Slope (Ball-Berry)                 32 */
        SEXP SOILCOEFS,        /* Soil Coefficients                  33 */
        SEXP ILEAFN,           /* Ini Leaf Nitrogen                  34 */
        SEXP KLN,              /* Decline in Leaf Nitr               35 */
        SEXP VMAXB1,           /* Effect of N on Vmax                36 */
        SEXP ALPHAB1,          /* Effect of N on alpha               37 */
        SEXP MRESP,            /* Maintenance resp                   38 */
        SEXP SOILTYPE,         /* Soil type                          39 */
        SEXP WSFUN,            /* Water Stress Func                  40 */
        SEXP WS,               /* Water stress flag                  41 */
        SEXP CENTCOEFS,        /* Century coefficients               42 */
        SEXP CENTTIMESTEP,     /* Century timestep                   43 */
        SEXP CENTKS,           /* Century decomp rates               44 */
        SEXP SOILLAYERS,       /* # soil layers                      45 */
        SEXP SOILDEPTHS,       /* Soil Depths                        46 */
        SEXP CWS,              /* Current water status               47 */
        SEXP HYDRDIST,         /* Hydraulic dist flag                48 */
        SEXP SECS,             /* Soil empirical coefs               49 */
        SEXP KPLN,             /* Leaf N decay                       50 */
        SEXP LNB0,             /* Leaf N Int                         51 */
        SEXP LNB1,             /* Leaf N slope                       52 */
        SEXP LNFUN,            /* Leaf N func flag                   53 */
        SEXP UPPERTEMP,        /* Upper photoParm temperature limit  54 */
        SEXP LOWERTEMP,        /* Lower photoParm temperature limit  55 */
        SEXP NNITROP,          /* Nitrogen parameters                56 */
		SEXP STOMWS)
{
    /* Creating pointers to avoid calling functions REAL and INTEGER so much */
    double lat = REAL(LAT)[0];
    int *doy = INTEGER(DOY);
    int *hr = INTEGER(HR);
    double *solar = REAL(SOLAR);
    double *temp = REAL(TEMP);
    double *rh = REAL(RH);
    double *windspeed = REAL(WINDSPEED);
    double *precip = REAL(PRECIP);
    double kd = REAL(KD)[0];
    double chil = REAL(CHIL)[0];
    double leafwidth = REAL(LEAFWIDTH)[0];
    int et_equation = REAL(ET_EQUATION)[0]; /* It comes as a REAL but I use an integer from here on */
    double heightf = REAL(HEIGHTF)[0];
    int nlayers = INTEGER(NLAYERS)[0];
	double *initial_biomass = REAL(INITIAL_BIOMASS);
    double *sencoefs = REAL(SENCOEFS);
    int timestep = INTEGER(TIMESTEP)[0];
    int vecsize;
    double Sp = REAL(SPLEAF)[0]; 
    double SpD = REAL(SPD)[0];
    double *dbpcoefs = REAL(DBPCOEFS);
    double *thermalp = REAL(THERMALP);
	double thermal_base_temperature = REAL(THERMAL_BASE_TEMP)[0];
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
    double kLN = REAL(KLN)[0];
    double vmaxb1 = REAL(VMAXB1)[0];
    double alphab1 = REAL(ALPHAB1)[0];
    double *mresp = REAL(MRESP);
    int soilType = INTEGER(SOILTYPE)[0];
    int wsFun = INTEGER(WSFUN)[0];
    int ws = INTEGER(WS)[0];
    double *centcoefs = REAL(CENTCOEFS);
    int centTimestep = INTEGER(CENTTIMESTEP)[0];
    double *centks = REAL(CENTKS);
    int soilLayers = INTEGER(SOILLAYERS)[0];
    double *soilDepths = REAL(SOILDEPTHS);
    double *cws = REAL(CWS);
    double hydrDist = INTEGER(HYDRDIST)[0];
    double *secs = REAL(SECS);
    double kpLN = REAL(KPLN)[0];
    double lnb0 = REAL(LNB0)[0]; 
    double lnb1 = REAL(LNB1)[0];
    int lnfun = INTEGER(LNFUN)[0];
    double upperT = REAL(UPPERTEMP)[0];
    double lowerT = REAL(LOWERTEMP)[0];
    /*Reading NitroP Variables */
    struct nitroParms nitrop;
	double StomWS = REAL(STOMWS)[0];
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
    SEXP CanopyAssim;
    SEXP CanopyTrans;
    SEXP Leafy;
    SEXP Stemy;
    SEXP Rooty;
    SEXP Rhizomey;
    SEXP Grainy;
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
    SEXP MinNitroVec;
    SEXP RespVec;
    SEXP SoilEvaporation;
    SEXP cwsMat;
    SEXP psimMat; /* Holds the soil water potential */
    SEXP rdMat;
    SEXP SCpools;
    SEXP SNpools;
    SEXP LeafPsimVec;

    vecsize = length(DOY);
    PROTECT(lists = allocVector(VECSXP,29));
    PROTECT(names = allocVector(STRSXP,29));

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
    PROTECT(cwsMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(psimMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(rdMat = allocMatrix(REALSXP,soilLayers,vecsize));
    PROTECT(SCpools = allocVector(REALSXP,9));
    PROTECT(SNpools = allocVector(REALSXP,9));
    PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));

    struct BioGro_results_str *results = (struct BioGro_results_str*)malloc(sizeof(struct BioGro_results_str));
    if (results) {
        initialize_biogro_results(results, soilLayers, vecsize);
    } else {
        Rprintf("Out of memory in R_MisGro.cpp.\n");
        return R_NilValue;
    }

    BioGro(lat, doy, hr, solar, temp, rh,
            windspeed, precip, kd, chil,
            leafwidth, et_equation, heightf, nlayers, initial_biomass,
            sencoefs, timestep, vecsize,
            Sp, SpD, dbpcoefs, thermalp, thermal_base_temperature,
            vmax1, alpha1, kparm, theta, beta, Rd, Catm, b0, b1, soilcoefs, ileafn, kLN,
            vmaxb1, alphab1, mresp, soilType, wsFun,
            ws, centcoefs, centTimestep, centks,
            soilLayers, soilDepths, cws, hydrDist,
            secs, kpLN, lnb0, lnb1, lnfun, upperT, lowerT, nitrop, StomWS, biomass_leaf_nitrogen_limitation, results);


    for(int i = 0; i < vecsize; i++) {
        REAL(DayofYear)[i] = results->day_of_year[i];
        REAL(Hour)[i] = results->hour[i];
        REAL(CanopyAssim)[i] = results->CanopyAssim[i];
        REAL(CanopyTrans)[i] = results->canopy_transpiration[i];
        REAL(Leafy)[i] = results->Leafy[i];
        REAL(Stemy)[i] = results->Stemy[i];
        REAL(Rooty)[i] = results->Rooty[i];
        REAL(Rhizomey)[i] = results->Rhizomey[i];
        REAL(Grainy)[i] = results->Grainy[i];
        REAL(LAIc)[i] = results->LAIc[i];
        REAL(TTTc)[i] = results->thermal_time[i];
        REAL(SoilWatCont)[i] = results->soil_water_content[i];
        REAL(StomatalCondCoefs)[i] = results->stomata_cond_coefs[i];
        REAL(LeafReductionCoefs)[i] = results->leaf_reduction_coefs[i];
        REAL(LeafNitrogen)[i] = results->leaf_nitrogen[i];
        REAL(AboveLitter)[i] = results->above_ground_litter[i];
        REAL(BelowLitter)[i] = results->below_ground_litter[i];
        REAL(VmaxVec)[i] = results->vmax[i];
        REAL(AlphaVec)[i] = results->alpha[i];
        REAL(SpVec)[i] = results->specific_leaf_area[i];
        REAL(MinNitroVec)[i] = results->min_nitro[i];
        REAL(RespVec)[i] = results->respiration[i];
        REAL(SoilEvaporation)[i] = results->soil_evaporation[i];
        REAL(LeafPsimVec)[i] = results->leaf_psim[i];

        for(int layer = 0; layer < soilLayers; layer++) {
            REAL(psimMat)[layer + i * soilLayers] = results->psim[layer + i * soilLayers];
            REAL(cwsMat)[layer + i * soilLayers] = results->water_status[layer + i * soilLayers];
            REAL(rdMat)[layer + i * soilLayers] = results->root_distribution[layer + i * soilLayers];
        }
    }

    /* Populating the results of the Century model */
    REAL(SCpools)[0] = results->centS.SCs[0];
    REAL(SCpools)[1] = results->centS.SCs[1];
    REAL(SCpools)[2] = results->centS.SCs[2];
    REAL(SCpools)[3] = results->centS.SCs[3];
    REAL(SCpools)[4] = results->centS.SCs[4];
    REAL(SCpools)[5] = results->centS.SCs[5];
    REAL(SCpools)[6] = results->centS.SCs[6];
    REAL(SCpools)[7] = results->centS.SCs[7];
    REAL(SCpools)[8] = results->centS.SCs[8];

    REAL(SNpools)[0] = results->centS.SNs[0];
    REAL(SNpools)[1] = results->centS.SNs[1];
    REAL(SNpools)[2] = results->centS.SNs[2];
    REAL(SNpools)[3] = results->centS.SNs[3];
    REAL(SNpools)[4] = results->centS.SNs[4];
    REAL(SNpools)[5] = results->centS.SNs[5];
    REAL(SNpools)[6] = results->centS.SNs[6];
    REAL(SNpools)[7] = results->centS.SNs[7];
    REAL(SNpools)[8] = results->centS.SNs[8];

    SET_VECTOR_ELT(lists, 0, DayofYear);
    SET_VECTOR_ELT(lists, 1, Hour);
    SET_VECTOR_ELT(lists, 2, CanopyAssim);
    SET_VECTOR_ELT(lists, 3, CanopyTrans);
    SET_VECTOR_ELT(lists, 4, Leafy);
    SET_VECTOR_ELT(lists, 5, Stemy);
    SET_VECTOR_ELT(lists, 6, Rooty);
    SET_VECTOR_ELT(lists, 7, Rhizomey);
    SET_VECTOR_ELT(lists, 8, Grainy);
    SET_VECTOR_ELT(lists, 9, LAIc);
    SET_VECTOR_ELT(lists, 10, TTTc);
    SET_VECTOR_ELT(lists, 11, SoilWatCont);
    SET_VECTOR_ELT(lists, 12, StomatalCondCoefs);
    SET_VECTOR_ELT(lists, 13, LeafReductionCoefs);
    SET_VECTOR_ELT(lists, 14, LeafNitrogen);
    SET_VECTOR_ELT(lists, 15, AboveLitter);
    SET_VECTOR_ELT(lists, 16, BelowLitter);
    SET_VECTOR_ELT(lists, 17, VmaxVec);
    SET_VECTOR_ELT(lists, 18, AlphaVec);
    SET_VECTOR_ELT(lists, 19, SpVec);
    SET_VECTOR_ELT(lists, 20, MinNitroVec);
    SET_VECTOR_ELT(lists, 21, RespVec);
    SET_VECTOR_ELT(lists, 22, SoilEvaporation);
    SET_VECTOR_ELT(lists, 23, cwsMat);
    SET_VECTOR_ELT(lists, 24, psimMat);
    SET_VECTOR_ELT(lists, 25, rdMat);
    SET_VECTOR_ELT(lists, 26, SCpools);
    SET_VECTOR_ELT(lists, 27, SNpools);
    SET_VECTOR_ELT(lists, 28, LeafPsimVec);

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
    setAttrib(lists,R_NamesSymbol,names);
    UNPROTECT(31);
    free_biogro_results(results);
    return(lists);
}
}
