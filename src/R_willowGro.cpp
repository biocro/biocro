/*
 *  BioCro/src/willowGro.c by Dandan Wang and Deepak Jaiswal Copyright (C) 2010-2015
 *
 */

#include <R.h>
#include <Rinternals.h>
#include "c3photo.h"
#include "BioCro.h"

extern "C" {

SEXP willowGro(
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


        SEXP HEIGHTF,          /* Height factor                      11 */
        SEXP NLAYERS,          /* Number of layers int he canopy     12 */
        SEXP INITIAL_BIOMASS,  /* Ini PLANT                          13 */
        SEXP SENCOEFS,         /* sene coefs                         15 */
        SEXP TIMESTEP,         /* time step                          16 */
        SEXP VECSIZE,          /* vector size                        17 */
        SEXP SPLEAF,           /* Spec Leaf Area                     18 */
        SEXP SPD,              /* Spec Lefa Area Dec                 19 */
        SEXP DBPCOEFS,         /* Dry Bio Coefs                      20 */
        SEXP THERMALP,         /* Themal Periods                     21 */
        SEXP TBASE,            /* Base temp  thermal time            22 */
        SEXP VMAX,             /* Vmax of photo                      23 */
        SEXP ALPHA,            /* Quantum yield                      24 */
        SEXP THETA,            /* theta param (photo)                26 */
        SEXP BETA,             /* beta param  (photo)                27 */
        SEXP RD,               /* Dark Resp   (photo)                28 */
        SEXP CATM,             /* CO2 atmosph                        29 */
        SEXP B0,               /* Int (Ball-Berry)                   30 */
        SEXP B1,               /* Slope (Ball-Berry)                 31 */
        SEXP SOILCOEFS,        /* Soil Coefficients                  32 */
        SEXP ILEAFN,           /* Ini Leaf Nitrogen                  33 */
        SEXP KLN,              /* Decline in Leaf Nitr               34 */
        SEXP VMAXB1,           /* Effect of N on Vmax                35 */
        SEXP ALPHAB1,          /* Effect of N on alpha               36 */
        SEXP MRESP,            /* Maintenance resp                   37 */
        SEXP SOILTYPE,         /* Soil type                          38 */
        SEXP WSFUN,            /* Water Stress Func                  39 */
        SEXP WS,               /* Water stress flag                  31 */
        SEXP CENTCOEFS,        /* Century coefficients               40 */
        SEXP CENTTIMESTEP,     /* Century timestep                   41 */
        SEXP CENTKS,           /* Century decomp rates               42 */
        SEXP SOILLAYERS,       /* # soil layers                      43 */
        SEXP SOILDEPTHS,       /* Soil Depths                        44 */
        SEXP CWS,              /* Current water status               45 */
        SEXP HYDRDIST,         /* Hydraulic dist flag                46 */
        SEXP SECS,             /* Soil empirical coefs               47 */
        SEXP KPLN,             /* Leaf N decay                       48 */
        SEXP LNB0,             /* Leaf N Int                         49 */
        SEXP LNB1,             /* Leaf N slope                       50 */
        SEXP LNFUN,            /* Leaf N func flag                   51 */
        SEXP UPPERTEMP,        /* Temperature Limitations photoParms 52 */
        SEXP LOWERTEMP,        /*                                    53 */
        SEXP JMAX,             /*                                    54 */
        SEXP JMAXB1,           /*                                    55 */
        SEXP O2,               /*                                    55 */
        SEXP GROWTHRESP,       /*                                    57 */
        SEXP STOMATAWS)        /*                                    58 */
{
    double lat = REAL(LAT)[0];
    int *doy = INTEGER(DOY);
    int *hour = INTEGER(HR);
    double *solar = REAL(SOLAR);
    double *temp = REAL(TEMP);
    double *rh = REAL(RH);
    double *windspeed = REAL(WINDSPEED);
    double *precip = REAL(PRECIP);
    double kd = REAL(KD)[0];
    // double chil = REAL(CHIL)[0]; unused
    //
    //
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
    double tbase = REAL(TBASE)[0];
    double vmax1 = REAL(VMAX)[0];
    double alpha1 = REAL(ALPHA)[0];
    double theta = REAL(THETA)[0];
    // double beta = REAL(BETA)[0]; set but not used
    double Rd = REAL(RD)[0];
    double Catm = REAL(CATM)[0];
    double b0 = REAL(B0)[0];
    double b1 = REAL(B1)[0];
    double *soilcoefs = REAL(SOILCOEFS);
    double ileafn = REAL(ILEAFN)[0];
    double kLN = REAL(KLN)[0];
    // double vmaxb1 = REAL(VMAXB1)[0];
    // double alphab1 = REAL(ALPHAB1)[0];
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
    // double upperT = REAL(UPPERTEMP)[0];
    // double lowerT = REAL(LOWERTEMP)[0];
    double jmax1 = REAL(JMAX)[0];
    // double jmaxb1 = REAL(JMAXB1)[0]; unused
    // double o2 = REAL(O2)[0];
    double GrowthRespFraction = REAL(GROWTHRESP)[0];
    double StomataWS = REAL(STOMATAWS)[0];

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
    PROTECT(lists = allocVector(VECSXP, 29));
    PROTECT(names = allocVector(STRSXP, 29));

    PROTECT(DayofYear = allocVector(REALSXP, vecsize));
    PROTECT(Hour = allocVector(REALSXP, vecsize));
    PROTECT(CanopyAssim = allocVector(REALSXP, vecsize));
    PROTECT(CanopyTrans = allocVector(REALSXP, vecsize));
    PROTECT(Leafy = allocVector(REALSXP, vecsize));
    PROTECT(Stemy = allocVector(REALSXP, vecsize));
    PROTECT(Rooty = allocVector(REALSXP, vecsize));
    PROTECT(Rhizomey = allocVector(REALSXP, vecsize));
    PROTECT(Grainy = allocVector(REALSXP, vecsize));
    PROTECT(LAIc = allocVector(REALSXP, vecsize));
    PROTECT(TTTc = allocVector(REALSXP, vecsize));
    PROTECT(SoilWatCont = allocVector(REALSXP, vecsize));
    PROTECT(StomatalCondCoefs = allocVector(REALSXP, vecsize));
    PROTECT(LeafReductionCoefs = allocVector(REALSXP, vecsize));
    PROTECT(LeafNitrogen = allocVector(REALSXP, vecsize));
    PROTECT(AboveLitter = allocVector(REALSXP, vecsize));
    PROTECT(BelowLitter = allocVector(REALSXP, vecsize));
    PROTECT(VmaxVec = allocVector(REALSXP, vecsize));
    PROTECT(AlphaVec = allocVector(REALSXP, vecsize));
    PROTECT(SpVec = allocVector(REALSXP, vecsize));
    PROTECT(MinNitroVec = allocVector(REALSXP, vecsize));
    PROTECT(RespVec = allocVector(REALSXP, vecsize));
    PROTECT(SoilEvaporation = allocVector(REALSXP, vecsize));
    PROTECT(cwsMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(psimMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(rdMat = allocMatrix(REALSXP, soilLayers, vecsize));
    PROTECT(SCpools = allocVector(REALSXP, 9));
    PROTECT(SNpools = allocVector(REALSXP, 9));
    PROTECT(LeafPsimVec = allocVector(REALSXP, vecsize));

	if ( vecsize > 8760 ) error("The number of timesteps must be less than or equal to 8760. You probably want to change the day1 or dayn arguments.");

    struct BioGro_results_str *results = (struct BioGro_results_str*)malloc(sizeof(struct BioGro_results_str));
    if (results) {
        initialize_biogro_results(results, soilLayers, vecsize);
    } else {
        Rprintf("Out of memory in R_willoGro.cpp.\n");
        return R_NilValue;
    }
	
	double (*leaf_n_limitation)(double, double, struct Model_state) = biomass_leaf_nitrogen_limitation;

    double o2 = 210;

    // double GPP; unused
    double leafdeathrate1;
    int A, B;
    double Tfrosthigh, Tfrostlow, leafdeathrate, Deadleaf;

    Tfrosthigh = REAL(SENCOEFS)[4];
    Tfrostlow = REAL(SENCOEFS)[5];
    leafdeathrate = REAL(SENCOEFS)[6];

    double newLeafcol[8760];
    double newStemcol[8760];
    double newRootcol[8760];
    double newRhizomecol[8760];

    double Rhizome = initial_biomass[0];
    double Stem = initial_biomass[1];
    double Leaf = initial_biomass[2];
    double Root = initial_biomass[3];

    int i, i3;

    double Grain = 0.0;
    double TTc = 0.0;
    double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kRhizome = 0.0, kGrain = 0.0;
    double newLeaf = 0.0, newStem = 0.0, newRoot = 0.0, newRhizome = 0.0, newGrain = 0.0, newStemLitter = 0.0, newLeafLitter = 0.0, newRhizomeLitter = 0.0, newRootLitter = 0.0;

	struct Model_state current_state = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /* Variables needed for collecting litter */
    double LeafLitter = centcoefs[20], StemLitter = centcoefs[21];
    double RootLitter = centcoefs[22], RhizomeLitter = centcoefs[23];
    double LeafLitter_d = 0.0, StemLitter_d = 0.0;
    double RootLitter_d = 0.0, RhizomeLitter_d = 0.0;
    double ALitter = 0.0, BLitter = 0.0;

    double *sti , *sti2, *sti3, *sti4; 
    double Remob;
    int k = 0, q = 0, m = 0, n = 0;
    int ri = 0;

    double LeafWS;
    double CanopyA, CanopyT;
    double LeafN_0 = ileafn;
    double LeafN = ileafn; /* Need to set it because it is used by CanA before it is computed */
    double iSp = Sp;
    double vmax = vmax1;
    double alpha = alpha1;

    /* Century */
    double MinNitro = centcoefs[19];
    int doyNfert = static_cast<int>(centcoefs[18]);
    double Nfert;
    double SCCs[9];
    double Resp = 0.0;

    /* Maintenance respiration */
    const double mrc1 = mresp[0];
    const double mrc2 = mresp[1]; 

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
    const double seneRhizome = sencoefs[3];

    struct Can_Str Canopy = {0,0,0};
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    struct dbp_str dbpS;
    struct soilML_str soilMLS;
    struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
    soTexS = soilTchoose(soilType);

    double LAI = Leaf * Sp;

    /* Creation of pointers outside the loop */
    sti = &newLeafcol[0]; /* This creates sti to be a pointer to the position 0
                             in the newLeafcol vector */
    sti2 = &newStemcol[0];
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

    results->centS.SCs[0] = 0.0;
    results->centS.SCs[1] = 0.0;
    results->centS.SCs[2] = 0.0;
    results->centS.SCs[3] = 0.0;
    results->centS.SCs[4] = 0.0;
    results->centS.SCs[5] = 0.0;
    results->centS.SCs[6] = 0.0;
    results->centS.SCs[7] = 0.0;
    results->centS.SCs[8] = 0.0;
    results->centS.Resp = 0.0;

    SCCs[0] = centcoefs[0];
    SCCs[1] = centcoefs[1];
    SCCs[2] = centcoefs[2];
    SCCs[3] = centcoefs[3];
    SCCs[4] = centcoefs[4];
    SCCs[5] = centcoefs[5];
    SCCs[6] = centcoefs[6];
    SCCs[7] = centcoefs[7];
    SCCs[8] = centcoefs[8];


    for(i=0; i < vecsize; ++i)
    {
        newLeafLitter = newStemLitter = newRootLitter = newRhizomeLitter= 0;

        /* The specific leaf area declines with the growing season at least in
           Miscanthus.  See Danalatos, Nalianis and Kyritsis "Growth and Biomass
           Productivity of Miscanthus sinensis "Giganteus" under optimum cultural
           management in north-eastern greece. */
        if(i%24 == 0) {
            Sp = iSp - (doy[i] - doy[0]) * SpD;
        }

        LAI = Leaf * Sp;
		LeafN = leaf_n_limitation(kLN, LeafN_0, current_state);
        // vmax = (LeafN_0 - LeafN) * vmaxb1 + vmax1; 


        /* Picking the dry biomass partitioning coefficients */
        dbpS = sel_dbp_coef(dbpcoefs, thermalp, TTc);

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;
        kRhizome = dbpS.kRhiz;

        /* First calculate the elapsed Thermal Time*/
        if(temp[i] > tbase) {
            TTc += (temp[i]-tbase) / (24/ (double)timestep); 
        }

        /*  Do the magic! Calculate growth*/

		current_state.leaf = Leaf;
		current_state.stem = Stem;
		current_state.root = Root;
		current_state.rhizome = Rhizome;
		current_state.lai = LAI;
		current_state.grain = Grain;
		current_state.k_leaf = kLeaf;
		current_state.k_stem = kStem;
		current_state.k_root = kRoot;
		current_state.k_rhizome = kRhizome;
		current_state.k_grain = kGrain;
		current_state.new_leaf = newLeaf;
		current_state.new_stem = newStem;
		current_state.new_root = newRoot;
		current_state.new_rhizome = newRhizome;
		current_state.new_grain = newGrain;
		current_state.thermal_time = TTc;
		current_state.doy = doy[i];
		current_state.hour = hour[i];
		current_state.solar = solar[i];
		current_state.temp = temp[i];
		current_state.rh = rh[i];
		current_state.windspeed = windspeed[i];
		current_state.StomataWS = StomataWS;
		current_state.LeafN = LeafN;
		current_state.vmax = vmax;
		current_state.alpha = alpha;

        Canopy = c3CanAC(LAI, doy[i], hour[i],
                solar[i], temp[i], rh[i], windspeed[i],
                lat, nlayers, vmax, jmax1,
				Rd, Catm, o2, b0, b1, theta, kd,
                heightf, LeafN, kpLN, lnb0, lnb1, lnfun, StomataWS, ws);

        CanopyA = Canopy.Assim * timestep * (1.0 - GrowthRespFraction);
        CanopyT = Canopy.Trans * timestep;

        /* if(ISNAN(Leaf)) { */
        /*    Rprintf("Leaf %.2f \n",Leaf); */
        /*    Rprintf("kLeaf %.2f \n",kLeaf); */
        /*    Rprintf("newLeaf %.2f \n",newLeaf); */
        /*    Rprintf("LeafWS %.2f \n",LeafWS); */
        /*    error("something is NA \n"); */
        /* } */

        /* if(ISNAN(CanopyA)) { */
        /*    Rprintf("LAI %.2f \n",LAI);  */
        /*    Rprintf("Leaf %.2f \n",Leaf); */
        /*    Rprintf("irtl %.2f \n", irtl); */
        /*    Rprintf("Rhizome %.2f \n",Rhizome); */
        /*    Rprintf("Sp %.2f \n",Sp);    */
        /*    Rprintf("vmax1 %.2f \n",vmax1); */
        /*    Rprintf("alpha1 %.2f \n",alpha1); */
        /*    Rprintf("kparm %.2f \n",kparm); */
        /*    Rprintf("theta %.2f \n",theta); */
        /*    Rprintf("beta %.2f \n",beta); */
        /*    Rprintf("Rd %.2f \n",Rd);  */
        /*    Rprintf("Catm %.2f \n",Catm); */
        /*    Rprintf("b01 %.2f \n",b01); */
        /*    Rprintf("b11 %.2f \n",b11); */
        /*    Rprintf("StomataWS %.2f \n",StomataWS); */
        /*    Rprintf("kd %.2f \n",kd);                  */
        /*    Rprintf("Sp %.2f \n",Sp);                   */
        /*    Rprintf("doy[i] %.i %.i \n",i,doy[i]);  */
        /*    Rprintf("hour[i] %.i %.i \n",i,hour[i]); */
        /*    Rprintf("solar[i] %.i %.2f \n",i,solar[i]); */
        /*    Rprintf("temp[i] %.i %.2f \n",i,temp[i]); */
        /*    Rprintf("rh[i] %.i %.2f \n",i,rh[i]); */
        /*    Rprintf("windspeed[i] %.i %.2f \n",i,windspeed[i]); */
        /*    Rprintf("lat %.i %.2f \n",i,lat); */
        /*    Rprintf("nlayers %.i %.i \n",i,nlayers);    */
        /*    error("something is NA \n"); */
        /* } */

        /* Inserting the multilayer model */
        if(soilLayers > 1) {
            soilMLS = soilML(precip[i], CanopyT, cws, soilDepth, soilDepths, FieldC, WiltP,
                    phi1, phi2, soTexS, wsFun, soilLayers, Root, 
                    LAI, 0.68, temp[i], solar[i], windspeed[i], rh[i], 
                    hydrDist, rfl, rsec, rsdf);

            StomataWS = soilMLS.rcoefPhoto;
            LeafWS = soilMLS.rcoefSpleaf;
            soilEvap = soilMLS.SoilEvapo;

            for(i3=0; i3 < soilLayers; ++i3) {
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
            WaterS = watstr(precip[i], TotEvap, waterCont, soilDepth, FieldC, WiltP, phi1, phi2, soilType, wsFun);   
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

        /* Nitrogen fertilizer */
        /* Only the day in which the fertilizer was applied this is available */
        /* When the day of the year is equal to the day the N fert was applied
         * then there is addition of fertilizer */
        if(doyNfert == doy[i]) {
            Nfert = centcoefs[17] / 24.0;
        } else {
            Nfert = 0;
        }                

        if (ISNAN(kRhizome) || ISNAN(kLeaf) || ISNAN(kRoot) || ISNAN(kStem) || ISNAN(kGrain)) {
            Rprintf("kLeaf %.2f, kStem %.2f, kRoot %.2f, kRhizome %.2f, kGrain %.2f \n", kLeaf, kStem, kRoot, kRhizome, kGrain);
            Rprintf("iter %i \n", i);
        }

        /* Here I can insert the code for Nitrogen limitations on photosynthesis
           parameters. This is taken From Harley et al. (1992) Modelling cotton under
           elevated CO2. PCE. This is modeled as a simple linear relationship between
           leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
           availability and possibly by the Thermal time accumulated.*/

        /* The crop demand for nitrogen is the leaf concentration times the amount of biomass.
           This modifies the amount of N available in the soil. 
           MinNitro is the available amount of N (kg/m2). 
           The demand is in Mg/ha. I need a conversion factor of 
           multiply by 1000, divide by 10000. */

        MinNitro = MinNitro - LeafN * (Stem + Leaf) * 1e-1;
        if(MinNitro < 0) MinNitro = 1e-3;

        if(kLeaf >= 0) {
            newLeaf = CanopyA * kLeaf * LeafWS ; 
            /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
                Plant. Phys. 46, 233-235. For this the water stress coefficient is different
                for leaf and vmax. */

            if(ISNAN(newLeaf)) {
                Rprintf("LeafWS %.2f \n", LeafWS);
                Rprintf("CanopyA %.2f \n", CanopyA);
            }

            /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
            newLeaf = resp(newLeaf, mrc1, temp[i]);

            *(sti+i) = newLeaf; /* This populates the vector newLeafcol. It makes sense
                                   to use i because when kLeaf is negative no new leaf is
                                   being accumulated and thus would not be subjected to senescence */
        } else {
            error("kLeaf should be positive");
        }

        if(TTc < seneLeaf) {
            Leaf += newLeaf;
        } else {
            A = lat >=0.0;
            B = doy[i]>=180.0 ;

            if((A && B)||((!A) && (!B))) {
                // Here we are checking/evaluating frost kill
                if (temp[i] > Tfrostlow) {
                    leafdeathrate1 = 100 * (temp[i] - Tfrosthigh) / (Tfrostlow - Tfrosthigh);
                    leafdeathrate1 = (leafdeathrate1 > 100.0) ? 100.0 : leafdeathrate1;
                } else {
                    leafdeathrate1 = 0.0;
                }
                //Rprintf("Death rate due to frost = %f, %f, %f, %f\n", leafdeathrate1, temp[i], Tfrosthigh, Tfrostlow);
                //Rprintf("%f, %f, %f, %f\n", leafdeathrate, temp[i], Tfrosthigh, Tfrostlow);
                leafdeathrate = (leafdeathrate > leafdeathrate1) ? leafdeathrate : leafdeathrate1;
                Deadleaf = Leaf * leafdeathrate * (0.01 / 24); // 0.01 is to convert from percent to fraction and 24 iss to convert daily to hourly
                Remob = Deadleaf * 0.6;
                newLeafLitter += (Deadleaf - Remob); /* Collecting the leaf litter */ 
                Rhizome += kRhizome * Remob;
                Stem += kStem * Remob;
                Root += kRoot * Remob;
                Grain += kGrain * Remob;
                newLeaf = newLeaf - Deadleaf + (kLeaf * Remob);
                k++;
                //Rprintf("%f,%f,%f,%f\n",leafdeathrate,Deadleaf,Remob,Leaf);
                //      error("stop");
            }
            Leaf += newLeaf;
        }

        /* New Stem*/
        if(kStem >= 0) {
            newStem = CanopyA * kStem ;
            newStem = resp(newStem, mrc1, temp[i]);
            *(sti2+i) = newStem;
        } else {
            error("kStem should be positive");
        }

        if(TTc < seneStem) {
            Stem += newStem;
        } else {
            Stem += newStem - *(sti2+q);
            newStemLitter += *(sti2+q);
            ++q;
        }

        if(kRoot > 0) {
            newRoot = CanopyA * kRoot ;
            newRoot = resp(newRoot, mrc2, temp[i]);
            *(sti3+i) = newRoot;
        } else {
            newRoot = Root * kRoot ;
            Rhizome += kRhizome * -newRoot * 0.9;
            Stem += kStem * -newRoot       * 0.9;
            Leaf += kLeaf * -newRoot * 0.9;
            Grain += kGrain * -newRoot * 0.9;
        }

        if(TTc < seneRoot) {
            Root += newRoot;
        } else {
            Root += newRoot - *(sti3+m);
            newRootLitter += *(sti3+m);
            ++m;
        }

        if(kRhizome > 0) {
            newRhizome = CanopyA * kRhizome ;
            newRhizome = resp(newRhizome, mrc2, temp[i]);
            *(sti4+ri) = newRhizome;
            /* Here i will not work because the rhizome goes from being a source
               to a sink. I need its own index. Let's call it rhizome's i or ri.*/
            ++ri;
        } else {
            if(Rhizome < 0) {
                Rhizome = 1e-4;
                warning("Rhizome became negative");
            }

            newRhizome = Rhizome * kRhizome;
            Root += kRoot * -newRhizome ;
            Stem += kStem * -newRhizome ;
            Leaf += kLeaf * -newRhizome ;
            Grain += kGrain * -newRhizome;
        }

        if(TTc < seneRhizome) {
            Rhizome += newRhizome;
        } else {
            Rhizome += newRhizome - *(sti4+n);
            newRhizomeLitter += *(sti4+n);
            ++n;
        }

        if((kGrain < 1e-10) || (TTc < thermalp[4])) {
            newGrain = 0.0;
            Grain += newGrain;
        } else {
            newGrain = CanopyA * kGrain;
            /* No respiration for grain at the moment */
            /* No senescence either */
            Grain += newGrain;  
        }

        if(i % 24*centTimestep == 0) {
            LeafLitter_d = LeafLitter * ((0.1/30)*centTimestep);
            StemLitter_d = StemLitter * ((0.1/30)*centTimestep);
            RootLitter_d = RootLitter * ((0.1/30)*centTimestep);
            RhizomeLitter_d = RhizomeLitter * ((0.1/30)*centTimestep);

            newLeafLitter -= LeafLitter_d;
            newStemLitter -= StemLitter_d;
            newRootLitter -= RootLitter_d;
            newRhizomeLitter -= RhizomeLitter_d;

            results->centS = Century(&LeafLitter_d, &StemLitter_d, &RootLitter_d, &RhizomeLitter_d,
                    waterCont, temp[i], centTimestep, SCCs, WaterS.runoff,
                    Nfert, /* N fertilizer*/
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
                    soilType, 
                    centks);
        }

        MinNitro = results->centS.MinN; /* These should be kg / m^2 per week? */
        Resp = results->centS.Resp;
        SCCs[0] = results->centS.SCs[0];
        SCCs[1] = results->centS.SCs[1];
        SCCs[2] = results->centS.SCs[2];
        SCCs[3] = results->centS.SCs[3];
        SCCs[4] = results->centS.SCs[4];
        SCCs[5] = results->centS.SCs[5];
        SCCs[6] = results->centS.SCs[6];
        SCCs[7] = results->centS.SCs[7];
        SCCs[8] = results->centS.SCs[8];

        LeafLitter += newLeafLitter;
        StemLitter += newStemLitter;
        RootLitter += newRootLitter;
        RhizomeLitter += newRhizomeLitter;

        ALitter = LeafLitter + StemLitter;
        BLitter = RootLitter + RhizomeLitter;

		results->day_of_year[i] = doy[i];
		results->hour[i] = hour[i];
        results->CanopyAssim[i] =  CanopyA;
		results->canopy_transpiration[i] = CanopyT;
        results->Leafy[i] = Leaf;
        results->Stemy[i] = Stem;
        results->Rooty[i] =  Root;
        results->Rhizomey[i] = Rhizome;
        results->Grainy[i] = Grain;
        results->LAIc[i] = LAI;
		results->thermal_time[i] = TTc;
		results->soil_water_content[i] = waterCont;
		results->stomata_cond_coefs[i] = StomataWS;
		results->leaf_reduction_coefs[i] = LeafWS;
		results->leaf_nitrogen[i] = LeafN;
		results->above_ground_litter[i] = ALitter;
		results->below_ground_litter[i] = BLitter;
		results->vmax[i] = vmax;
		results->alpha[i] = alpha;
		results->specific_leaf_area[i] = Sp;
		results->min_nitro[i] = MinNitro / (24 / (double)centTimestep);
		results->respiration[i] = Resp / (24*centTimestep);
		results->soil_evaporation[i] = soilEvap;
		results->leaf_psim[i] = LeafPsim;
		for(int layer = 0; layer < soilLayers; ++layer) {
			results->psim[layer + i * soilLayers] = psi[layer + i * soilLayers];
			results->water_status[layer + i * soilLayers] = water_status[layer + i * soilLayers];
			results->root_distribution[layer + i * soilLayers] = root_distribution[layer + i * soilLayers];
		}

        REAL(DayofYear)[i] =  INTEGER(DOY)[i];
        REAL(Hour)[i] =  INTEGER(HR)[i];
        REAL(CanopyAssim)[i] =  CanopyA;
        REAL(CanopyTrans)[i] =  CanopyT; 
        REAL(Leafy)[i] = Leaf;
        REAL(Stemy)[i] = Stem;
        REAL(Rooty)[i] =  Root;
        REAL(Rhizomey)[i] = Rhizome;
        REAL(Grainy)[i] = Grain;
        REAL(LAIc)[i] = LAI;
		REAL(TTTc)[i] = TTc;
        REAL(SoilWatCont)[i] = waterCont;
        REAL(StomatalCondCoefs)[i] = StomataWS;
        REAL(LeafReductionCoefs)[i] = LeafWS;
        REAL(LeafNitrogen)[i] = LeafN;
        REAL(AboveLitter)[i] = ALitter;
        REAL(BelowLitter)[i] = BLitter;
        REAL(VmaxVec)[i] = vmax;
        REAL(AlphaVec)[i] = alpha;
        REAL(SpVec)[i] = Sp;
        REAL(MinNitroVec)[i] = MinNitro/ (24*centTimestep);
        REAL(RespVec)[i] = Resp / (24*centTimestep);
        REAL(SoilEvaporation)[i] = soilEvap;
        REAL(LeafPsimVec)[i] = LeafPsim;

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
}  // extern "C"

