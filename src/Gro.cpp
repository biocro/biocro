/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <memory>
#include <R.h>
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

    std::unique_ptr<IModule> senescence_module = std::unique_ptr<IModule>(new thermal_time_senescence_module);
    std::unique_ptr<IModule> growth_module = std::unique_ptr<IModule>(new partitioning_growth_module);

    state_map state = initial_state;
    state_map temp_derivs;
    state["CanopyT"] = 0;
    state["CanopyA"] = 0;
    state["LeafWS"] = 0;
    state["leaf_senescence_index"] = 0;
    state["stem_senescence_index"] = 0;
    state["root_senescence_index"] = 0;
    state["rhizome_senescence_index"] = 0;
    state["Sp"] = 0;
    state["lai"] = 0;
    state["LeafN"] = 0;
    state["vmax"] = 0;
    state["alpha"] = 0;
    state["kLeaf"] = 0;
    state["kStem"] = 0;
    state["kRoot"] = 0;
    state["kGrain"] = 0;
    state["kRhizome"] = 0;
    state["canopy_assimilation"] = 0;
    state["canopy_transpiration"] = 0;
    state["stomatal_conductance_coefs"] = 0;
    state["soil_evaporation"] = 0;

    auto n_rows = varying_parameters.begin()->second.size();
    state_vector_map state_history = allocate_state(state, n_rows);  // Allocating memory is not necessary, but it makes it slightly faster.
    state_vector_map deriv_history = allocate_state(state, n_rows);  // Allocating memory is not necessary, but it makes it slightly faster.


    vector<double> newLeafcol(n_rows), newStemcol(n_rows), newRootcol(n_rows), newRhizomecol(n_rows);  // If initialized with a size, a vector<double> will start with that many elements, each initialized to 0.
    int k = 0, ri = 0, q = 0, m = 0, n = 0;  // These are indexes that keep track of positions in the previous four variables.

    double CanopyA = 0, soilEvap = 0;
    double kLeaf, kRoot, kStem, kRhizome, kGrain;
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
        // The following copies state, invariant parameters, and the last values in varying_parameters into the variable "s";
        state_map p = combine_state(invariant_parameters, at(varying_parameters, i));
        state_map s = combine_state(state, p);

        /*
         * 1) Calculate all state-dependent state variables.
         */

        /* NOTE: I think it's easier to follow if all of the set up is done in one place,
         * so I've moved all of those things to the top of the loop.
         * This section is for state variables that are not modified directly by derivatives.
         * No derivaties should be calulated here.
         * This will make it so that the code in section 2 is order independent.
         */

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

        s["kLeaf"] = kLeaf;
        s["kStem"] = kStem;
        s["kRoot"] = kRoot;
        s["kGrain"] = kGrain;
        s["kRhizome"] = kRhizome;

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
        state_map derivs; // There's no guarantee that each derivative will be set in each iteration, by declaring the variable within the loop all derivates will be set to 0 at each iteration.

        if(s.at("temp") > s.at("tbase")) {
            s["TTc"] += (s.at("temp") - s.at("tbase")) / (24/s.at("timestep")); 
        }

        derivs = canopy_photosynthesis_module->run(s);

        s["CanopyA"] = derivs["Assim"] * s.at("timestep");
        s["CanopyT"] = derivs["Trans"] * s.at("timestep");

        temp_derivs = soil_evaporation_module->run(s); // This function doesn't actually return derivatives.
        
        soilEvap = temp_derivs.at("soilEvap");
        s["waterCont"] = temp_derivs.at("waterCont");
        s["StomataWS"] = temp_derivs.at("StomataWS");
        s["LeafWS"] = temp_derivs.at("LeafWS");

        // NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
        // record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
        // Since it's not recorded, that part will never senesce.
        // Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
        // at the end of the season for all of the tissue to senesce.
        // This doesn't seem like a good approach.

        state = replace_state(state, s);
        append_state_to_vector(state, state_history);

        derivs += growth_module->run(state_history, deriv_history, p);

        //Rprintf("Before senescence.\n");
        derivs += senescence_module->run(state_history, deriv_history, p);

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

        //output_map(derivs);
        s["Leaf"] += derivs["newLeaf"];
        s["Stem"] += derivs["newStem"];
        s["Root"] += derivs["newRoot"];
        s["Rhizome"] += derivs["newRhizome"];
        s["Grain"] += derivs["newGrain"];
        s["LeafLitter"] += derivs["LeafLitter"];
        s["RootLitter"] += derivs["RootLitter"];
        s["RhizomeLitter"] += derivs["RhizomeLitter"];
        s["StemLitter"] += derivs["StemLitter"];
        s["leaf_senescence_index"] += derivs["leaf_senescence_index"];
        s["stem_senescence_index"] += derivs["stem_senescence_index"];
        s["root_senescence_index"] += derivs["root_senescence_index"];
        s["rhizome_senescence_index"] += derivs["rhizome_senescence_index"];
        state = replace_state(state, s);

    //    if ( i < n_rows - 1)
     //       append_state_to_vector(state, state_history);

        /*
         * 4) Record variables in the state_history map.
         */

        /* NOTE: We can write a recorder function so that the user can specify
         * what they want to record, but I don't know that anyone will care
         * and for now it's easier to just record all of the state variables and other
         * things of interest.
         */

        // Record everything that is in "state".
        append_state_to_vector(derivs, deriv_history);

        // Record other parameters of interest.
        state_history["canopy_assimilation"][i] = s["CanopyA"];
        //state_history["canopy_assimilation"].push_back(CanopyA);
        //state_history["canopy_transpiration"].push_back(s.at("CanopyT"));
        //state_history["lai"].push_back(s.at("lai"));
        //state_history["soil_water_content"].push_back(s.at("waterCont"));
        //state_history["stomatal_conductance_coefs"].push_back(s.at("StomataWS"));
        //state_history["leaf_reduction_coefs"].push_back(s.at("LeafWS"));
        //state_history["leaf_nitrogen"].push_back(s.at("LeafN"));
        //state_history["vmax"].push_back(s.at("vmax"));
        //state_history["alpha"].push_back(s.at("alpha"));
        //state_history["specific_leaf_area"].push_back(s.at("Sp"));
        //state_history["soil_evaporation"].push_back(soilEvap);
        //state_history["kLeaf"].push_back(kLeaf);
        //state_history["newLeafcol"].push_back(newLeafcol[i]);
        //state_history["newStemcol"].push_back(newStemcol[i]);
        //state_history["newRootcol"].push_back(newRootcol[i]);
        //state_history["newRhizomecol"].push_back(newRhizomecol[i]);
    }
    return state_history;
}

