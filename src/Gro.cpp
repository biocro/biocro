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
        map<string, double> initial_state,
        map<string, double> invariant_parameters,
        map<string, vector<double>> varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
		double (*leaf_n_limitation)(state_map model_state))
{
    vector<double>::size_type n = varying_parameters.begin()->second.size();
    map<string, vector<double>> results;
    results["canopy_assimilation"] = vector<double>(n);
    results["canopy_transpiration"] = vector<double>(n);
    results["leaf_mass"] = vector<double>(n);
    results["stem_mass"] = vector<double>(n);
    results["root_mass"] = vector<double>(n);
    results["rhizome_mass"] = vector<double>(n);
    results["grain_mass"] = vector<double>(n);
    results["lai"] = vector<double>(n);
    results["thermal_time"] = vector<double>(n);
    results["soil_water_content"] = vector<double>(n);
    results["stomatal_conductance_coefs"] = vector<double>(n);
    results["leaf_reduction_coefs"] = vector<double>(n);
    results["leaf_nitrogen"] = vector<double>(n);
    results["above_ground_litter"] = vector<double>(n);
    results["below_ground_litter"] = vector<double>(n);
    results["vmax"] = vector<double>(n);
    results["alpha"] = vector<double>(n);
    results["specific_leaf_area"] = vector<double>(n);
    results["min_nitro"] = vector<double>(n);
    results["respiration"] = vector<double>(n);
    results["soil_evaporation"] = vector<double>(n);
    results["leaf_psim"] = vector<double>(n);
    results["kLeaf"] = vector<double>(n);

    state_map state = initial_state;
    state_map s;
    map<string, double> fluxes;

    int vecsize = n;

    state["LeafN_0"] = state["ileafn"];
    state["LeafN"] = state["ileafn"]; /* Need to set it because it is used by CanA before it is computed */
    state["TTc"] = 0;



    //// Old framework stuff.

    int k = 0;
    double newLeafcol[8760];

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
    for(int i = 0; i < vecsize; ++i)
    {
        s = combine_state(state, invariant_parameters, varying_parameters, i);
        s["Sp"] = s.at("iSp") - (s.at("doy") - varying_parameters.at("doy")[0]) * s.at("SpD");

        s["lai"] = s.at("Leaf") * s.at("Sp");
		s["LeafN"] = leaf_n_limitation(s);
        s["vmax"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmaxb1") + s.at("vmax1");
        s["alpha"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("alphab1") + s.at("alpha1");

        /* The specific leaf area declines with the growing season at least in
           Miscanthus.  See Danalatos, Nalianis and Kyritsis "Growth and Biomass
           Productivity of Miscanthus sinensis "Giganteus" under optimum cultural
           management in north-eastern greece*/

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

            fluxes["newLeaf"] = resp(fluxes["newLeaf"], s.at("mrc1"), s.at("temp"));

            *(newLeafcol+i) = fluxes.at("newLeaf"); /* This populates the vector newLeafcol. It makes sense
                                   to use i because when kLeaf is negative no new leaf is
                                   being accumulated and thus would not be subjected to senescence */
        } else {
            fluxes["newLeaf"] = s.at("Leaf") * kLeaf;
            s["Rhizome"] += kRhizome * -fluxes["newLeaf"] * 0.9; /* 0.9 is the efficiency of retranslocation */
            s["Stem"] += kStem * -fluxes["newLeaf"] * 0.9;
            s["Root"] += kRoot * -fluxes["newLeaf"] * 0.9;
            s["Grain"] += kGrain * -fluxes["newLeaf"] * 0.9;
        }

        if (i == 0) output_map(s);
        if (s["TTc"] < s.at("seneLeaf")) {
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
        state = replace_state(state, s);

        results["canopy_assimilation"][i] =  CanopyA;
		results["canopy_transpiration"][i] = CanopyT;
        results["leaf_mass"][i] = s["Leaf"];
        results["stem_mass"][i] = s["Stem"];
        results["root_mass"][i] =  s["Root"];
        results["rhizome_mass"][i] = s["Rhizome"];
        results["grain_mass"][i] = s["Grain"];
        results["lai"][i] = s["lai"];
		results["thermal_time"][i] = s["TTc"];
		results["soil_water_content"][i] = s["waterCont"];
		results["leaf_reduction_coefs"][i] = s["LeafWS"];
		results["leaf_nitrogen"][i] = s["LeafN"];
		results["vmax"][i] = s["vmax"];
		results["alpha"][i] = s["alpha"];
		results["specific_leaf_area"][i] =s[" Sp"];
		results["soil_evaporation"][i] = soilEvap;
		results["kLeaf"][i] = kLeaf;
    }
    return results;
}

