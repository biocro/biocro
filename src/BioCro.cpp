/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include "BioCro.h"
#include "Century.h"

void BioGro(
        double lat,                   /* Latitude                            1 */
        int doy[],                    /* Day of the year                     2 */
        int hour[],                     /* Hour of the day                     3 */
        double solar[],               /* Solar Radiation                     4 */
        double temp[],                /* Temperature                         5 */
        double rh[],                  /* Relative humidity                   6 */
        double windspeed[],           /* Wind Speed                          7 */
        double precip[],              /* Precipitation                       8 */
        double kd,                    /* K D (ext coeff diff)                9 */
        double chil,                  /* Chi, leaf angle distribution       10 */
        double leafwidth,             /* Width of a leaf                    11 */
        int et_equation,              /* Integer to indicate ET equation    12 */
        double heightf,               /* Height factor                      13 */
        int nlayers,                  /* Number of layers in the canopy     14 */
		double initial_biomass[4],
        double sencoefs[],            /* sene coefs                         17 */
        int timestep,                 /* time step                          18 */
        int vecsize,                  /* vector size                        19 */
        double Sp,                    /* Spec Leaf Area                     20 */
        double SpD,                   /* Spec Lefa Area Dec                 21 */
        double dbpcoefs[25],          /* Dry Bio Coefs                      22 */
        double thermalp[],            /* Themal Periods                     23 */
        double tbase,                 /* Base temperature for thermal time     */
        double vmax1,                 /* Vmax of photo                      24 */
        double alpha1,                /* Quantum yield                      25 */
        double kparm,                 /* k parameter (photo)                26 */
        double theta,                 /* theta param (photo)                27 */
        double beta,                  /* beta param  (photo)                28 */
        double Rd,                    /* Dark Resp   (photo)                29 */
        double Catm,                  /* CO2 atmosph                        30 */
        double b0,                    /* Int (Ball-Berry)                   31 */
        double b1,                    /* Slope (Ball-Berry)                 32 */
        double soilcoefs[],           /* Soil Coefficients                  33 */
        double ileafn,                /* Ini Leaf Nitrogen                  34 */
        double kLN,                   /* Decline in Leaf Nitr               35 */
        double vmaxb1,                /* Effect of N on Vmax                36 */
        double alphab1,               /* Effect of N on alpha               37 */
        double mresp[],               /* Maintenance resp                   38 */
        int soilType,                 /* Soil type                          39 */
        int wsFun,                    /* Water Stress Func                  40 */
        int ws,                       /* Water stress flag                  41 */
        double centcoefs[],           /* Century coefficients               42 */
        int centTimestep,             /* Century timestep                   43 */
        double centks[],              /* Century decomp rates               44 */
        int soilLayers,               /* # soil layers                      45 */
        double soilDepths[],          /* Soil Depths                        46 */
        double cws[],                 /* Current water status               47 */
        int hydrDist,                 /* Hydraulic dist flag                48 */
        double secs[],                /* Soil empirical coefs               49 */
        double kpLN,                  /* Leaf N decay                       50 */
        double lnb0,                  /* Leaf N Int                         51 */
        double lnb1,                  /* Leaf N slope                       52 */
        int lnfun ,                   /* Leaf N func flag                   53 */
        double upperT,                /* Upper photoParm temperature limit  54 */
        double lowerT,                /* Lower photoParm temperature limit  55 */
        struct nitroParms nitroP,     /* Nitrogen parameters                56 */
		double StomataWS,
		double (*leaf_n_limitation)(double, double, struct Model_state),
    	struct BioGro_results_str *results)
{
    double newLeafcol[8760];
    double newStemcol[8760];
    double newRootcol[8760];
    double newRhizomecol[8760];

	double Rhizome = initial_biomass[0];
	double Stem = initial_biomass[1];
	double Leaf = initial_biomass[2];
	double Root = initial_biomass[3];

    int i, i3;

    double LAI = 0.0, Grain = 0.0;
    double TTc = 0.0;
    double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kRhizome = 0.0, kGrain = 0.0;
    double newLeaf = 0.0, newStem = 0.0, newRoot = 0.0, newRhizome = 0.0, newGrain = 0.0;

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

    LAI = Leaf * Sp;

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


    for(i = 0; i < vecsize; i++)
    {
        /* First calculate the elapsed Thermal Time*/
        if(temp[i] > tbase) {
            TTc += (temp[i]-tbase) / (24/timestep); 
        }

        /* Do the magic! Calculate growth*/

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

        Canopy = CanAC(LAI, doy[i], hour[i],
                solar[i], temp[i], rh[i], windspeed[i],
                lat, nlayers, vmax, alpha, kparm, beta,
                Rd, Catm, b0, b1, theta, kd, chil,
                heightf, LeafN, kpLN, lnb0, lnb1, lnfun, upperT, lowerT,
				nitroP, leafwidth, et_equation, StomataWS, ws);

        CanopyA = Canopy.Assim * timestep;
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
        if (soilLayers > 1) {
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
            WaterS = watstr(precip[i], TotEvap, waterCont, soilDepth, FieldC, WiltP, phi1, phi2, soilType, wsFun);
            waterCont = WaterS.awc;
            StomataWS = WaterS.rcoefPhoto;
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

        /* Picking the dry biomass partitioning coefficients */
        dbpS = sel_dbp_coef(dbpcoefs, thermalp, TTc);

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;
        kRhizome = dbpS.kRhiz;

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

		LeafN = leaf_n_limitation(kLN, LeafN_0, current_state);

        vmax = (LeafN_0 - LeafN) * vmaxb1 + vmax1;
        alpha = (LeafN_0 - LeafN) * alphab1 + alpha1;

        /* The crop demand for nitrogen is the leaf concentration times the amount of biomass.
           This modifies the amount of N available in the soil. 
           MinNitro is the available amount of N (kg/m2). 
           The demand is in Mg/ha. I need a conversion factor of 
           multiply by 1000, divide by 10000. */

        MinNitro = MinNitro - LeafN * (Stem + Leaf) * 1e-1;
        if(MinNitro < 0) MinNitro = 1e-3;

        if (kLeaf > 0) {
            newLeaf = CanopyA * kLeaf * LeafWS;
            /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
                Plant. Phys. 46, 233-235. For this the water stress coefficient is different
                for leaf and vmax. */
            /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
            /* The 0.02 and 0.03 are constants here but vary depending on species
               as pointed out in that reference. */

            if(ISNAN(newLeaf)) {
                Rprintf("LeafWS %.2f \n", LeafWS);
                Rprintf("CanopyA %.2f \n", CanopyA);
            }

            newLeaf = resp(newLeaf, mrc1, temp[i]);

            *(sti+i) = newLeaf; /* This populates the vector newLeafcol. It makes sense
                                   to use i because when kLeaf is negative no new leaf is
                                   being accumulated and thus would not be subjected to senescence */
        } else {
            newLeaf = Leaf * kLeaf;
            Rhizome += kRhizome * -newLeaf * 0.9; /* 0.9 is the efficiency of retranslocation */
            Stem += kStem * -newLeaf * 0.9;
            Root += kRoot * -newLeaf * 0.9;
            Grain += kGrain * -newLeaf * 0.9;
        }

        if (TTc < seneLeaf) {
            Leaf += newLeaf;
        } else {
            Leaf += newLeaf - *(sti+k); /* This means that the new value of leaf is
                                           the previous value plus the newLeaf
                                           (Senescence might start when there is
                                           still leaf being produced) minus the leaf
                                           produced at the corresponding k.*/
            Remob = *(sti+k) * 0.6;
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

        if (i%24 == 0) {
            Sp = iSp - (doy[i] - doy[0]) * SpD;
        }

        LAI = Leaf * Sp;

        /* New Stem*/
        if (kStem >= 0) {
            newStem = CanopyA * kStem;
            newStem = resp(newStem, mrc1, temp[i]);
            *(sti2+i) = newStem;
        } else {
            error("kStem should be positive");
        }

        if (TTc < seneStem) {
            Stem += newStem;
        } else {
            Stem += newStem - *(sti2+q);
            StemLitter += *(sti2+q);
            q++;
        }

        if (kRoot > 0) {
            newRoot = CanopyA * kRoot;
            newRoot = resp(newRoot, mrc2, temp[i]);
            *(sti3+i) = newRoot;
        } else {
            newRoot = Root * kRoot;
            Rhizome += kRhizome * -newRoot * 0.9;
            Stem += kStem * -newRoot       * 0.9;
            Leaf += kLeaf * -newRoot * 0.9;
            Grain += kGrain * -newRoot * 0.9;
        }

        if (TTc < seneRoot) {
            Root += newRoot;
        } else {
            Root += newRoot - *(sti3+m);
            RootLitter += *(sti3+m);
            m++;
        }

        if (kRhizome > 0) {
            newRhizome = CanopyA * kRhizome;
            newRhizome = resp(newRhizome, mrc2, temp[i]);
            *(sti4+ri) = newRhizome;
            /* Here i will not work because the rhizome goes from being a source
               to a sink. I need its own index. Let's call it rhizome's i or ri.*/
            ri++;
        } else {
            if (Rhizome < 0) {
                Rhizome = 1e-4;
                warning("Rhizome became negative");
            }

            newRhizome = Rhizome * kRhizome;
            Root += kRoot * -newRhizome;
            Stem += kStem * -newRhizome;
            Leaf += kLeaf * -newRhizome;
            Grain += kGrain * -newRhizome;
        }

        if (TTc < seneRhizome) {
            Rhizome += newRhizome;
        } else {
            Rhizome += newRhizome - *(sti4+n);
            RhizomeLitter += *(sti4+n);
            n++;
        }

        if ((kGrain < 1e-10) || (TTc < thermalp[4])) {
            newGrain = 0.0;
            Grain += newGrain;
        } else {
            newGrain = CanopyA * kGrain;
            /* No respiration for grain at the moment */
            /* No senescence either */
            Grain += newGrain;
        }

        if (i % 24*centTimestep == 0) {
            LeafLitter_d = LeafLitter * ((0.1/30)*centTimestep);
            StemLitter_d = StemLitter * ((0.1/30)*centTimestep);
            RootLitter_d = RootLitter * ((0.1/30)*centTimestep);
            RhizomeLitter_d = RhizomeLitter * ((0.1/30)*centTimestep);

            LeafLitter -= LeafLitter_d;
            StemLitter -= StemLitter_d;
            RootLitter -= RootLitter_d;
            RhizomeLitter -= RhizomeLitter_d;

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
                    centcoefs[15], /* Root litter N */
                    centcoefs[16], /* Rhizome litter N */
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
		results->min_nitro[i] = MinNitro / (24 / centTimestep);
		results->respiration[i] = Resp / (24*centTimestep);
		results->soil_evaporation[i] = soilEvap;
		results->leaf_psim[i] = LeafPsim;
		for(int layer = 0; layer < soilLayers; layer++) {
			results->psim[layer + i * soilLayers] = psi[layer + i * soilLayers];
			results->water_status[layer + i * soilLayers] = water_status[layer + i * soilLayers];
			results->root_distribution[layer + i * soilLayers] = root_distribution[layer + i * soilLayers];
		}
    }
}

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

