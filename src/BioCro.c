/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <math.h>
#include <R.h>
#include <Rmath.h>
#include "AuxBioCro.h"
#include "BioCro.h"
#include "Century.h"

struct BioGro_results_str BioGro(
        double lat,                   /* Latitude                            1 */
        int doy[],                    /* Day of the year                     2 */
        int hr[],                     /* Hour of the day                     3 */
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
        double iRhizome,              /* Ini Rhiz                           15 */
        double irtl,                  /* i rhiz to leaf                     16 */
        double sencoefs[],            /* sene coefs                         17 */
        int timestep,                 /* time step                          18 */
        int vecsize,                  /* vector size                        19 */
        double Sp,                    /* Spec Leaf Area                     20 */
        double SpD,                   /* Spec Lefa Area Dec                 21 */
        double dbpcoefs[25],          /* Dry Bio Coefs                      22 */
        double thermalp[],            /* Themal Periods                     23 */
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
        struct nitroParms nitroP)     /* Nitrogen parameters                56 */
{
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
    litter[0] = centcoefs[20];
    litter[1] = centcoefs[21];
    litter[2] = centcoefs[22];
    litter[3] = centcoefs[23];

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
    double MinNitro = centcoefs[19];
    double SCCs[9];
    // double Resp; unused

    const double mrc1 = mresp[0];
    const double mrc2 = mresp[1];

    struct BioGro_results_str results;
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

    for(i = 0; i < vecsize; i++)
    {
        /* First calculate the elapsed Thermal Time*/
        TTc += (temp[i] / (24/timestep));

        /* Do the magic! Calculate growth*/

        Canopy = CanAC(LAI, doy[i], hr[i],
                solar[i], temp[i], rh[i], windspeed[i],
                lat, nlayers, vmax, alpha, kparm, theta, beta,
                Rd, Catm, b0, b1, StomWS, ws, kd, chil,
                heightf, LeafN, kpLN, lnb0, lnb1, lnfun, upperT, lowerT, nitroP, leafwidth, et_equation);

        CanopyA = Canopy.Assim * timestep;
        CanopyT = Canopy.Trans * timestep;

        if (ISNAN(CanopyA)) {
            Rprintf("LAI %.2f \n", LAI);
            Rprintf("Leaf %.2f \n", Leaf);
            Rprintf("irtl %.2f \n", irtl);
            Rprintf("Rhizome %.2f \n", Rhizome);
            Rprintf("Sp %.2f \n", Sp);
            Rprintf("doy[i] %.i %.i \n", i, doy[i]);
            Rprintf("hr[i] %.i %.i \n", i, hr[i]);
            Rprintf("solar[i] %.i %.2f \n", i, solar[i]);
            Rprintf("temp[i] %.i %.2f \n", i, temp[i]);
            Rprintf("rh[i] %.i %.2f \n", i, rh[i]);
            Rprintf("windspeed[i] %.i %.2f \n", i, windspeed[i]);
            Rprintf("lat %.i %.2f \n", i, lat);
            Rprintf("nlayers %.i %.i \n", i, nlayers);
        }

        /* Inserting the multilayer model */
        if (soilLayers > 1) {
            soilMLS = soilML(precip[i], CanopyT, &cws[0], soilDepth, soilDepths, FieldC, WiltP,
                    phi1, phi2, soTexS, wsFun, soilLayers, Root,
                    LAI, 0.68, temp[i], solar[i], windspeed[i], rh[i],
                    hydrDist, secs[0], secs[1], secs[2]);

            StomWS = soilMLS.rcoefPhoto;
            LeafWS = soilMLS.rcoefSpleaf;
            soilEvap = soilMLS.SoilEvapo;
            for(i3 = 0; i3<soilLayers; i3++) {
                cws[i3] = soilMLS.cws[i3];
            }

        } else {
            soilEvap = SoilEvapo(LAI, 0.68, temp[i], solar[i], waterCont, FieldC, WiltP, windspeed[i], rh[i], secs[1]);
            TotEvap = soilEvap + CanopyT;
            WaterS = watstr(precip[i], TotEvap, waterCont, soilDepth, FieldC, WiltP, phi1, phi2, soilType, wsFun);
            waterCont = WaterS.awc;
            StomWS = WaterS.rcoefPhoto;
            LeafWS = WaterS.rcoefSpleaf;
        }

        /* Picking the dry biomass partitioning coefficients */
        dbpS = sel_dbp_coef(dbpcoefs, thermalp, TTc);

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;
        kRhizome = dbpS.kRhiz;

        if (ISNAN(kRhizome) || ISNAN(kLeaf) || ISNAN(kRoot) || ISNAN(kStem) || ISNAN(kGrain)) {
            Rprintf("kLeaf %.2f, kStem %.2f, kRoot %.2f, kRhizome %.2f, kGrain %.2f \n", kLeaf, kStem, kRoot, kRhizome, kGrain);
            Rprintf("iter %i \n", i);
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

            centS = Century(&LeafLitter_d, &StemLitter_d, &RootLitter_d, &RhizomeLitter_d,
                    waterCont, temp[i], centTimestep, SCCs, WaterS.runoff,
                    centcoefs[17], /* N fertilizer*/
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
                    soilType, centks);
        }

        /* Here I can insert the code for Nitrogen limitations on photosynthesis
           parameters. This is taken From Harley et al. (1992) Modelling cotton under
           elevated CO2. PCE. This is modeled as a simple linear relationship between
           leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
           availability and possibly by the Thermal time accumulated.*/

        MinNitro = centS.MinN;
        // Resp = centS.Resp; set but not used

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

        if (kLeaf > 0) {
            newLeaf = CanopyA * kLeaf * LeafWS;
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
        } else {
            newLeaf = Leaf * kLeaf;
            Rhizome += kRhizome * -newLeaf * 0.9; /* 0.9 is the efficiency of retranslocation */
            Stem += kStem * -newLeaf   * 0.9;
            Root += kRoot * -newLeaf * 0.9;
            Grain += kGrain * -newLeaf * 0.9;
        }

        if (TTc < SeneLeaf) {
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

        /* Rprintf("Sp %.2f \n", Sp); */

        LAI = Leaf * Sp;

        /* New Stem*/
        if (kStem > 0) {
            newStem = CanopyA * kStem;
            newStem = resp(newStem, mrc1, temp[i]);
            *(sti2+i) = newStem;
        }

        if (TTc < SeneStem) {
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

        if (TTc < SeneRoot) {
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

        if (TTc < SeneRhizome) {
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

        ALitter += LeafLitter + StemLitter;
        BLitter += RootLitter + RhizomeLitter;

        results.CanopyAssim[i] =  CanopyA;
        results.Leafy[i] = Leaf;
        results.Stemy[i] = Stem;
        results.Rooty[i] =  Root;
        results.Rhizomey[i] = Rhizome;
        results.Grainy[i] = Grain;
        results.LAIc[i] = LAI;
    }
    return(results);
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

