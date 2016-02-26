/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <memory>
#include <R.h>
#include <Rinternals.h>
#include <math.h>
#include <Rmath.h>
#include "BioCro.h"
#include "Century.h"
#include "modules.h"

map<string, vector<double>> Gro(
        map<string, double> const &initial_state,
        map<string, double> const &invariant_parameters,
        map<string, vector<double>> const &varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
		double (*leaf_n_limitation)(state_map const &model_state))
{
    auto n_rows = varying_parameters.begin()->second.size();
    map<string, vector<double>> results;
    results["canopy_assimilation"] = vector<double>(n_rows);
    results["canopy_transpiration"] = vector<double>(n_rows);
    results["leaf_mass"] = vector<double>(n_rows);
    results["stem_mass"] = vector<double>(n_rows);
    results["root_mass"] = vector<double>(n_rows);
    results["rhizome_mass"] = vector<double>(n_rows);
    results["grain_mass"] = vector<double>(n_rows);
    results["lai"] = vector<double>(n_rows);
    results["thermal_time"] = vector<double>(n_rows);
    results["soil_water_content"] = vector<double>(n_rows);
    results["stomatal_conductance_coefs"] = vector<double>(n_rows);
    results["leaf_reduction_coefs"] = vector<double>(n_rows);
    results["leaf_nitrogen"] = vector<double>(n_rows);
    results["above_ground_litter"] = vector<double>(n_rows);
    results["below_ground_litter"] = vector<double>(n_rows);
    results["vmax"] = vector<double>(n_rows);
    results["alpha"] = vector<double>(n_rows);
    results["specific_leaf_area"] = vector<double>(n_rows);
    results["min_nitro"] = vector<double>(n_rows);
    results["respiration"] = vector<double>(n_rows);
    results["soil_evaporation"] = vector<double>(n_rows);
    results["leaf_psim"] = vector<double>(n_rows);
    results["kLeaf"] = vector<double>(n_rows);

    state_map state = initial_state;
    state_map s;
    map<string, double> fluxes;

    //// Old framework stuff.
    int k = 0, ri = 0, q = 0, m = 0, n = 0;
    double newLeafcol[8760];
    double newStemcol[8760];
    double newRootcol[8760];
    double newRhizomecol[8760];

    double CanopyA, CanopyT;
    double soilEvap = 0;
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    struct dbp_str dbpS;
    double kLeaf, kRoot, kStem, kRhizome, kGrain;
    Rprintf("Before dbpcoefs\n");

    double dbpcoefs[] = {
        invariant_parameters.at("kStem1"), invariant_parameters.at("kLeaf1"), invariant_parameters.at("kRoot1"), invariant_parameters.at("kRhizome1"),
        invariant_parameters.at("kStem2"), invariant_parameters.at("kLeaf2"), invariant_parameters.at("kRoot2"), invariant_parameters.at("kRhizome2"),
        invariant_parameters.at("kStem3"), invariant_parameters.at("kLeaf3"), invariant_parameters.at("kRoot3"), invariant_parameters.at("kRhizome3"),
        invariant_parameters.at("kStem4"), invariant_parameters.at("kLeaf4"), invariant_parameters.at("kRoot4"), invariant_parameters.at("kRhizome4"),
        invariant_parameters.at("kStem5"), invariant_parameters.at("kLeaf5"), invariant_parameters.at("kRoot5"), invariant_parameters.at("kRhizome5"),
        invariant_parameters.at("kStem6"), invariant_parameters.at("kLeaf6"), invariant_parameters.at("kRoot6"), invariant_parameters.at("kRhizome6"), invariant_parameters.at("kGrain6")
    };

    Rprintf("Before thermalp\n");
    double thermalp[] = {
        invariant_parameters.at("tp1"), invariant_parameters.at("tp2"), invariant_parameters.at("tp3"), invariant_parameters.at("tp4"), invariant_parameters.at("tp5"), invariant_parameters.at("tp6")
    };

    Rprintf("Before loop\n");
    for(size_t i = 0; i < n_rows; ++i)
    {
        s = combine_state(state, invariant_parameters, varying_parameters, i);
        s["Sp"] = s.at("iSp") - (s.at("doy") - varying_parameters.at("doy")[0]) * s.at("SpD");

        s["lai"] = s.at("Leaf") * s.at("Sp");
		s["LeafN"] = leaf_n_limitation(s);
        s["vmax"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmaxb1") + s.at("vmax1");
        s["alpha"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("alphab1") + s.at("alpha1");

        if(s.at("temp") > s.at("tbase")) {
            s["TTc"] += (s.at("temp")-s.at("tbase")) / (24/s.at("timestep")); 
        }

        fluxes = canopy_photosynthesis_module->run(s);

        CanopyA = fluxes["Assim"] * s.at("timestep");
        CanopyT = fluxes["Trans"] * s.at("timestep");

        soilEvap = SoilEvapo(s.at("lai"), 0.68, s.at("temp"), s.at("solar"), s.at("waterCont"),
                s.at("FieldC"), s.at("WiltP"), s.at("windspeed"), s.at("rh"), s.at("rsec"));
        s["TotEvap"] = soilEvap + CanopyT;
        WaterS = watstr(s.at("precip"), s.at("TotEvap"), s.at("waterCont"), s.at("soilDepth"), s.at("FieldC"),
                s.at("WiltP"), s.at("phi1"), s.at("phi2"), s.at("soilType"), s.at("wsFun"));
        s["waterCont"] = WaterS.awc;
        s["StomataWS"] = WaterS.rcoefPhoto;
        s["LeafWS"] = WaterS.rcoefSpleaf;

        /* Picking the dry biomass partitioning coefficients */
        dbpS = sel_dbp_coef(dbpcoefs, thermalp, s.at("TTc"));

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;
        kRhizome = dbpS.kRhiz;

        /* Here I can insert the code for Nitrogen limitations on photosynthesis
           parameters. This is taken From Harley et al. (1992) Modelling cotton under
           elevated CO2. PCE. This is modeled as a simple linear relationship between
           leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
           availability and possibly by the Thermal time accumulated.*/

        if (kLeaf > 0) {
            fluxes["newLeaf"] = CanopyA * kLeaf * s.at("LeafWS");
            /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
                Plant. Phys. 46, 233-235. For this the water stress coefficient is different
                for leaf and vmax. */
            /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
            /* The 0.02 and 0.03 are constants here but vary depending on species
               as pointed out in that reference. */

            fluxes["newLeaf"] = resp(fluxes.at("newLeaf"), s.at("mrc1"), s.at("temp"));

            *(newLeafcol+i) = fluxes.at("newLeaf"); /* This populates the vector newLeafcol. It makes sense
                                   to use i because when kLeaf is negative no new leaf is
                                   being accumulated and thus would not be subjected to senescence */
        } else {
            fluxes["newLeaf"] = s.at("Leaf") * kLeaf;
            s["Rhizome"] += kRhizome * -fluxes.at("newLeaf") * 0.9; /* 0.9 is the efficiency of retranslocation */
            s["Stem"] += kStem * -fluxes.at("newLeaf") * 0.9;
            s["Root"] += kRoot * -fluxes.at("newLeaf") * 0.9;
            s["Grain"] += kGrain * -fluxes.at("newLeaf") * 0.9;
        }

        if (s.at("TTc") < s.at("seneLeaf")) {
            s["Leaf"] += fluxes.at("newLeaf");
        } else {
            s["Leaf"] += fluxes.at("newLeaf") - *(newLeafcol+k); /* This means that the new value of leaf is
                                           the previous value plus the newLeaf
                                           (Senescence might start when there is
                                           still leaf being produced) minus the leaf
                                           produced at the corresponding k.*/
            double Remob = *(newLeafcol+k) * 0.6;
            s["LeafLitter"] += *(newLeafcol+k) * 0.4; /* Collecting the leaf litter */ 
            s["Rhizome"] += kRhizome * Remob;
            s["Stem"] += kStem * Remob;
            s["Root"] += kRoot * Remob;
            s["Grain"] += kGrain * Remob;
            ++k;
        }

        if (kStem >= 0) {
            fluxes["newStem"] = CanopyA * kStem;
            fluxes["newStem"] = resp(fluxes.at("newStem"), s.at("mrc1"), s.at("temp"));
            *(newStemcol+i) = fluxes.at("newStem");
        } else {
            error("kStem should be positive");
        }

        if (s.at("TTc") < s.at("seneStem")) {
            s["Stem"] += fluxes.at("newStem");
        } else {
            s["Stem"] += fluxes.at("newStem") - *(newStemcol+q);
            s["StemLitter"] += *(newStemcol+q);
            ++q;
        }

        if (kRoot > 0) {
            fluxes["newRoot"] = CanopyA * kRoot;
            fluxes["newRoot"] = resp(fluxes.at("newRoot"), s.at("mrc2"), s.at("temp"));
            *(newRootcol+i) = fluxes.at("newRoot");
        } else {
            fluxes["newRoot"] = s.at("Root") * kRoot;
            s["Rhizome"] += kRhizome * -fluxes.at("newRoot") * 0.9;
            s["Stem"] += kStem * -fluxes.at("newRoot")       * 0.9;
            s["Leaf"] += kLeaf * -fluxes.at("newRoot") * 0.9;
            s["Grain"] += kGrain * -fluxes.at("newRoot") * 0.9;
        }

        if (s.at("TTc") < s.at("seneRoot")) {
            s["Root"] += fluxes.at("newRoot");
        } else {
            s["Root"] += fluxes.at("newRoot") - *(newRootcol+m);
            s["RootLitter"] += *(newRootcol+m);
            ++m;
        }

        if (kRhizome > 0) {
            fluxes["newRhizome"] = CanopyA * kRhizome;
            fluxes["newRhizome"] = resp(fluxes.at("newRhizome"), s.at("mrc2"), s.at("temp"));
            *(newRhizomecol+ri) = fluxes.at("newRhizome");
            /* Here i will not work because the rhizome goes from being a source
               to a sink. I need its own index. Let's call it rhizome's i or ri.*/
            ++ri;
        } else {
            if (s.at("Rhizome") < 0) {
                s["Rhizome"] = 1e-4;
                warning("Rhizome became negative");
            }

            fluxes["newRhizome"] = s.at("Rhizome") * kRhizome;
            s["Root"] += kRoot * -fluxes.at("newRhizome");
            s["Stem"] += kStem * -fluxes.at("newRhizome");
            s["Leaf"] += kLeaf * -fluxes.at("newRhizome");
            s["Grain"] += kGrain * -fluxes.at("newRhizome");
        }

       if (s.at("TTc") < s.at("seneRhizome")) {
            s["Rhizome"] += fluxes.at("newRhizome");
        } else {
            s["Rhizome"] += fluxes.at("newRhizome") - *(newRhizomecol+n);
            s["RhizomeLitter"] += *(newRhizomecol+n);
            ++n;
        }

        if ((kGrain < 1e-10) || (s["TTc"] < thermalp[4])) {
            fluxes["newGrain"] = 0.0;
            s["Grain"] += fluxes.at("newGrain");
        } else {
            fluxes["newGrain"] = CanopyA * kGrain;
            /* No respiration for grain at the moment */
            /* No senescence either */
            s["Grain"] += fluxes.at("newGrain");
        }

        state = replace_state(state, s);

        results["canopy_assimilation"][i] =  CanopyA;
		results["canopy_transpiration"][i] = CanopyT;
        results["leaf_mass"][i] = s.at("Leaf");
        results["stem_mass"][i] = s.at("Stem");
        results["root_mass"][i] =  s.at("Root");
        results["rhizome_mass"][i] = s.at("Rhizome");
        results["grain_mass"][i] = s.at("Grain");
        results["lai"][i] = s.at("lai");
		results["thermal_time"][i] = s.at("TTc");
		results["soil_water_content"][i] = s.at("waterCont");
        results["stomatal_conductance_coefs"][i] = s.at("StomataWS");
		results["leaf_reduction_coefs"][i] = s.at("LeafWS");
		results["leaf_nitrogen"][i] = s.at("LeafN");
		results["vmax"][i] = s.at("vmax");
		results["alpha"][i] = s.at("alpha");
		results["specific_leaf_area"][i] =s.at("Sp");
		results["soil_evaporation"][i] = soilEvap;
		results["kLeaf"][i] = kLeaf;
    }
    return results;
}

