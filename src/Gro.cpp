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

    state_map current_state = initial_state;
    state_map temp_derivs;

    auto n_rows = varying_parameters.begin()->second.size();
    state_vector_map state_history = allocate_state(current_state, n_rows);  // Allocating memory is not necessary, but it makes it slightly faster.
    state_vector_map deriv_history = allocate_state(current_state, n_rows);  // TODO: I think these are allocated and never used. Maybe it's better to not allocate memory this map.

    state_vector_map results = state_history;

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
        append_state_to_vector(current_state, state_history);
        append_state_to_vector(current_state, results);

        // The following copies invariant parameters, and the last values in varying_parameters into the variable "p";
        state_map p = combine_state(invariant_parameters, at(varying_parameters, i));

        /*
         * 1) Calculate all state-dependent state variables.
         */

        /* NOTE: I think it's easier to follow if all of the set up is done in one place,
         * so I've moved all of those things to the top of the loop.
         * This section is for state variables that are not modified directly by derivatives.
         * No derivaties should be calulated here.
         * This will make it so that the code in section 2 is order independent.
         */

        p["Sp"] = p.at("iSp") - (p.at("doy") - varying_parameters.at("doy")[0]) * p.at("SpD");
        p["lai"] = state_history.at("Leaf")[i] * p.at("Sp");

        /* Model photosynthetic parameters as a linear relationship between
           leaf nitrogen and vmax and alpha. Leaf Nitrogen should be modulated by N
           availability and possibly by the thermal time.
           (Harley et al. 1992. Modelling cotton under elevated CO2. PCE) */
        p["LeafN"] = leaf_n_limitation(combine_state(at(state_history, i), p));
        p["vmax"] = (p.at("LeafN_0") - p.at("LeafN")) * p.at("vmaxb1") + p.at("vmax1");
        p["alpha"] = (p.at("LeafN_0") - p.at("LeafN")) * p.at("alphab1") + p.at("alpha1");

        dbpS = sel_dbp_coef(dbpcoefs, thermalp, state_history.at("TTc")[i]);

        p["kLeaf"] = dbpS.kLeaf;
        p["kStem"] = dbpS.kStem;
        p["kRoot"] = dbpS.kRoot;
        p["kGrain"] = dbpS.kGrain;
        p["kRhizome"] = dbpS.kRhiz;

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

        if(p.at("temp") > p.at("tbase")) {
            state_history["TTc"][i] += (p.at("temp") - p.at("tbase")) / (24/p.at("timestep")); 
        }
        derivs += canopy_photosynthesis_module->run(state_history, deriv_history, p);

        p["CanopyA"] = derivs["Assim"] * p.at("timestep");
        p["CanopyT"] = derivs["Trans"] * p.at("timestep");

        derivs += soil_evaporation_module->run(state_history, deriv_history, p);

        derivs += growth_module->run(state_history, deriv_history, p);

        derivs += senescence_module->run(state_history, deriv_history, p);

        /*
         * 3) Update the state variables.
         */

        /* NOTE: This is the only spot where where state should be updated.
         * By updating everything at the end, the order of the previous statements will not
         * affect output. It should also allow us to use an ODE solver.
         */

        current_state = at(state_history, i);
        current_state = update_state(current_state, derivs);

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
        //results["canopy_assimilation"][i] = s["CanopyA"];
        results["canopy_assimilation"].push_back(p["CanopyA"]);
        results["canopy_transpiration"].push_back(p["CanopyT"]);
        results["lai"].push_back(p.at("lai"));
        //results["soil_water_content"].push_back(s.at("waterCont"));
        results["stomatal_conductance_coefs"].push_back(current_state.at("StomataWS"));
        //results["leaf_reduction_coefs"].push_back(s.at("LeafWS"));
        //results["leaf_nitrogen"].push_back(s.at("LeafN"));
        results["vmax"].push_back(p.at("vmax"));
        results["alpha"].push_back(p.at("alpha"));
        results["specific_leaf_area"].push_back(p.at("Sp"));
        results["soil_evaporation"].push_back(derivs["soilEvap"]);
        //results["kLeaf"].push_back(kLeaf);
        //results["newLeafcol"].push_back(newLeafcol[i]);
        //results["newStemcol"].push_back(newStemcol[i]);
        //results["newRootcol"].push_back(newRootcol[i]);
        //results["newRhizomecol"].push_back(newRhizomecol[i]);
    }
    return results;
}

