/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <memory>
#include <R.h>
#include <Rinternals.h>
#include "BioCro.h"
#include "Century.h"
#include "modules.h"

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::unique_ptr<IModule> const &canopy_photosynthesis_module,
        std::unique_ptr<IModule> const &soil_evaporation_module,
        double (*leaf_n_limitation)(state_map const &model_state))
{
    state_map state = initial_state;
    state_map s;
    state_map derivs;

    auto n_rows = varying_parameters.begin()->second.size();
    state_vector_map results = allocate_state(state, n_rows);  // Allocating memory is not necessary, but it makes it slightly faster.

    vector<double> newLeafcol(n_rows), newStemcol(n_rows), newRootcol(n_rows), newRhizomecol(n_rows);  // If initialized with a size, a vector<double> will start with that many elements, each initialized to 0.
    int k = 0, ri = 0, q = 0, m = 0, n = 0;  // These are indexes that keep track of positions in the previous four variables.

    double CanopyA = 0, CanopyT = 0, soilEvap = 0;
    double kLeaf, kRoot, kStem, kRhizome, kGrain;
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    struct dbp_str dbpS;

    double dbpcoefs[] = {
        invariant_parameters.at("kStem1"), invariant_parameters.at("kLeaf1"), invariant_parameters.at("kRoot1"), invariant_parameters.at("kRhizome1"),
        invariant_parameters.at("kStem2"), invariant_parameters.at("kLeaf2"), invariant_parameters.at("kRoot2"), invariant_parameters.at("kRhizome2"),
        invariant_parameters.at("kStem3"), invariant_parameters.at("kLeaf3"), invariant_parameters.at("kRoot3"), invariant_parameters.at("kRhizome3"),
        invariant_parameters.at("kStem4"), invariant_parameters.at("kLeaf4"), invariant_parameters.at("kRoot4"), invariant_parameters.at("kRhizome4"),
        invariant_parameters.at("kStem5"), invariant_parameters.at("kLeaf5"), invariant_parameters.at("kRoot5"), invariant_parameters.at("kRhizome5"),
        invariant_parameters.at("kStem6"), invariant_parameters.at("kLeaf6"), invariant_parameters.at("kRoot6"), invariant_parameters.at("kRhizome6"), invariant_parameters.at("kGrain6")
    };

    double thermalp[] = {
        invariant_parameters.at("tp1"), invariant_parameters.at("tp2"), invariant_parameters.at("tp3"), invariant_parameters.at("tp4"), invariant_parameters.at("tp5"), invariant_parameters.at("tp6")
    };

    for(size_t i = 0; i < n_rows; ++i)
    {
        /*
         * 1) Calculate all state-dependent state variables.
         */

        /* NOTE: I think it's easier to follow if all of the set up is done in one place,
         * so I've moved all of those things to the top of the loop.
         * This section is for state variables that are not modified directly by derivatives.
         * No derivaties should be calulated here.
         * This will make it so that the code in section 2 is order independent.
         */

        s = combine_state(state, invariant_parameters, varying_parameters, i);
        s["Sp"] = s.at("iSp") - (s.at("doy") - varying_parameters.at("doy")[0]) * s.at("SpD");
        s["lai"] = s.at("Leaf") * s.at("Sp");

        /* Model photosynthetic parameters as a linear relationship between
           leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
           availability and possibly by the thermal time.
           (Harley et al. 1992. Modelling cotton under elevated CO2. PCE) */
        s["LeafN"] = leaf_n_limitation(s);
        s["vmax"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmaxb1") + s.at("vmax1");
        s["alpha"] = (s.at("LeafN_0") - s.at("LeafN")) * s.at("alphab1") + s.at("alpha1");

        dbpS = sel_dbp_coef(dbpcoefs, thermalp, s.at("TTc"));

        kLeaf = dbpS.kLeaf;
        kStem = dbpS.kStem;
        kRoot = dbpS.kRoot;
        kGrain = dbpS.kGrain;
        kRhizome = dbpS.kRhiz;

        /*
         * 2) Calculate derivatives between state variables.
         */

        /* NOTE: This section should be written to calculate derivates only. No state should be modified.
         * All derivatives should depend only on current state. I.e., derivates should not depend on other derivaties or previous state.
         * I've changed it to try to meet these requirements, but it currently does not meet them.
         * E.g., 1) s["TTc"] is changed at the very begining, modifying state;
         * 2) all of the partitioning code depends on CanopyA, so it depends on a derivative;
         * 3) the senescence code depends on derivates from previous state. 
         * When this section adheres to those guidelines, we can start replacing all of these sections with "modules",
         * that are called as "derivs = module->run(state);" like the canopy_photosynthesis_module is called now.
         */
        derivs.clear();  // There's no guarantee that each derivative will be set in each iteration, so start by setting every derivative to 0.

        if(s.at("temp") > s.at("tbase")) {
            s["TTc"] += (s.at("temp") - s.at("tbase")) / (24/s.at("timestep")); 
        }

        derivs = canopy_photosynthesis_module->run(s);

        CanopyA = derivs["Assim"] * s.at("timestep");
        CanopyT = derivs["Trans"] * s.at("timestep");

        soilEvap = soil_evaporation_module->run(s).at("soilEvap");
        s["TotEvap"] = soilEvap + CanopyT;

        WaterS = watstr(s.at("precip"), s.at("TotEvap"), s.at("waterCont"), s.at("soilDepth"), s.at("FieldC"),
                s.at("WiltP"), s.at("phi1"), s.at("phi2"), s.at("soilType"), s.at("wsFun"));

        s["waterCont"] = WaterS.awc;
        s["StomataWS"] = WaterS.rcoefPhoto;
        s["LeafWS"] = WaterS.rcoefSpleaf;

        if (kLeaf > 0) {
            newLeafcol[i] = CanopyA * kLeaf * s.at("LeafWS");
            /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
                Plant. Phys. 46, 233-235. For this the water stress coefficient is different
                for leaf and vmax. */

            /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
            newLeafcol[i] = resp(newLeafcol[i], s.at("mrc1"), s.at("temp"));

            derivs["newLeaf"] = newLeafcol[i]; /* It makes sense to use i because when kLeaf
                                                  is negative no new leaf is being accumulated
                                                  and thus would not be subjected to senescence. */
        } else {
            derivs["newLeaf"] += s.at("Leaf") * kLeaf;
            derivs["newRhizome"] += kRhizome * -derivs.at("newLeaf") * 0.9; /* 0.9 is the efficiency of retranslocation */
            derivs["newStem"] += kStem * -derivs.at("newLeaf") * 0.9;
            derivs["newRoot"] += kRoot * -derivs.at("newLeaf") * 0.9;
            derivs["newGrain"] += kGrain * -derivs.at("newLeaf") * 0.9;
        }

        if (s.at("TTc") >= s.at("seneLeaf")) {
            derivs["newLeaf"] -= newLeafcol[k]; /* This means that the new value of leaf is
                                                   the previous value plus the newLeaf
                                                   (Senescence might start when there is
                                                   still leaf being produced) minus the leaf
                                                   produced at the corresponding k. */
            double Remob = newLeafcol[k] * 0.6;
            derivs["LeafLitter"] += newLeafcol[k] * 0.4; /* Collecting the leaf litter */ 
            derivs["newRhizome"] += kRhizome * Remob;
            derivs["newStem"] += kStem * Remob;
            derivs["newRoot"] += kRoot * Remob;
            derivs["newGrain"] += kGrain * Remob;
            ++k;
        }

        if (kStem >= 0) {
            newStemcol[i] = CanopyA * kStem;
            newStemcol[i] = resp(newStemcol[i], s.at("mrc1"), s.at("temp"));
            derivs["newStem"] += newStemcol[i];
        } else {
            error("kStem should be positive");
        }

        if (s.at("TTc") >= s.at("seneStem")) {
            derivs["newStem"] -= newStemcol[q];
            derivs["StemLitter"] += newStemcol[q];
            ++q;
        }

        if (kRoot > 0) {
            newRootcol[i] = CanopyA * kRoot;
            newRootcol[i] = resp(newRootcol[i], s.at("mrc2"), s.at("temp"));
            derivs["newRoot"] += newRootcol[i];
        } else {
            derivs["newRoot"] += s.at("Root") * kRoot;
            derivs["newRhizome"] += kRhizome * -derivs.at("newRoot") * 0.9;
            derivs["newStem"] += kStem * -derivs.at("newRoot") * 0.9;
            derivs["newLeaf"] += kLeaf * -derivs.at("newRoot") * 0.9;
            derivs["newGrain"] += kGrain * -derivs.at("newRoot") * 0.9;
        }

        if (s.at("TTc") >= s.at("seneRoot")) {
            derivs["newRoot"] -= newRootcol[m];
            derivs["RootLitter"] += newRootcol[m];
            ++m;
        }

        if (kRhizome > 0) {
            newRhizomecol[ri] = CanopyA * kRhizome;
            newRhizomecol[ri] = resp(newRhizomecol[ri], s.at("mrc2"), s.at("temp"));
            derivs["newRhizome"] += newRhizomecol[ri];
            /* Here i will not work because the rhizome goes from being a source
               to a sink. I need its own index. Let's call it rhizome's i or ri.*/
            ++ri;
        } else {
            derivs["newRhizome"] += s.at("Rhizome") * kRhizome;
            if ( derivs["newRhizome"] > s.at("Rhizome") ) {  // Check if this would make the rhizome mass negative.
                derivs["newRhizome"] = s.at("Rhizome");
                warning("Rhizome flux would make rhizome mass negative.");
            }

            derivs["newRoot"] += kRoot * -derivs.at("newRhizome");
            derivs["newStem"] += kStem * -derivs.at("newRhizome");
            derivs["newLeaf"] += kLeaf * -derivs.at("newRhizome");
            derivs["newGrain"] += kGrain * -derivs.at("newRhizome");
        }

        if (s.at("TTc") >= s.at("seneRhizome")) {
            derivs["newRhizome"] -= newRhizomecol[n];
            derivs["RhizomeLitter"] += newRhizomecol[n];
            ++n;
        }

        if ((kGrain >= 1e-10) && (s["TTc"] >= thermalp[4])) {
            derivs["newGrain"] += CanopyA * kGrain;
            /* No respiration for grain at the moment */
            /* No senescence either */
        }

        if (i % 24 * s.at("centTimestep") == 0) {
            double coefficient = ((0.1 / 30) * s.at("centTimestep"));
            derivs["LeafLitter"] -= s.at("LeafLitter") * coefficient;
            derivs["StemLitter"] -= s.at("StemLitter") * coefficient;
            derivs["RootLitter"] -= s.at("RootLitter") * coefficient;
            derivs["RhizomeLitter"] -= s.at("RhizomeLitter") * coefficient;
        }

        /*
         * 3) Update the state variables.
         */

        /* NOTE: This is the only spot where where state should be updated.
         * By updating everything at the end, the order of the previous statements will not
         * affect output. It should also allow us to use an ODE solver.
         *
         * Now I'm writting out all of the changes, but I will change it so that it can be done
         * with a function as follows: s = replace_state(s, derivs);
         * Then this section will always be two lines of code regardless of what comes previously.
         */

        s["Leaf"] += derivs["newLeaf"];
        s["Stem"] += derivs["newStem"];
        s["Root"] += derivs["newRoot"];
        s["Rhizome"] += derivs["newRhizome"];
        s["Grain"] += derivs["newGrain"];
        s["LeafLitter"] += derivs["LeafLitter"];
        s["RootLitter"] += derivs["RootLitter"];
        s["RhizomeLitter"] += derivs["RhizomeLitter"];
        s["StemLitter"] += derivs["StemLitter"];
        state = replace_state(state, s);  // Variables that exists in both "state" and "s" are copied from "s" to "state", overwriting values in "state".

        /*
         * 4) Record variables in the results map.
         */

        /* NOTE: We can write a recorder function so that the user can specify
         * what they want to record, but I don't know that anyone will care
         * and for now it's easier to just record all of the state variables and other
         * things of interest.
         */

        // Record everything that is in "state".
        append_state_to_vector(state, results);

        // Record other parameters of interest.
        results["canopy_assimilation"].push_back(CanopyA);
        results["canopy_transpiration"].push_back(CanopyT);
        results["lai"].push_back(s.at("lai"));
        results["soil_water_content"].push_back(s.at("waterCont"));
        results["stomatal_conductance_coefs"].push_back(s.at("StomataWS"));
        results["leaf_reduction_coefs"].push_back(s.at("LeafWS"));
        results["leaf_nitrogen"].push_back(s.at("LeafN"));
        results["vmax"].push_back(s.at("vmax"));
        results["alpha"].push_back(s.at("alpha"));
        results["specific_leaf_area"].push_back(s.at("Sp"));
        results["soil_evaporation"].push_back(soilEvap);
    }
    return results;
}

