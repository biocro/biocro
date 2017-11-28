/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include "BioCro.h"
#include "modules.h"
#include "math.h"

using std::vector;
using std::string;

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules)
{
    state_map current_state = initial_state;

    size_t n_rows = varying_parameters.begin()->second.size();
    state_vector_map state_history = allocate_state(current_state, n_rows);  // Allocating memory is not necessary, but it makes it slightly faster.
    state_vector_map results = state_history;
    state_vector_map deriv_history;

    vector<vector<string>> key_vector = {keys(initial_state), keys(invariant_parameters), keys(varying_parameters)};
    if (any_key_is_duplicated(key_vector)) {
        throw std::range_error("A parameter may appear in only one of the state lists.");
    }

    state_map p = combine_state(current_state, combine_state(invariant_parameters, at(varying_parameters, 0)));
    vector<string> missing_state;

    /* 
     * This should use dependency resolution to find missing state. It currently doesn't work because it's checking
     * whether the initial state has all of the required values, but it also needs to check whether a module provides a value.
     *
     * Modules check for missing state if exceptions are thrown, so this isn't necessary, but it has the advantage that 
     * it will break before any calculations are done, and compiles a list of all missing state.

    for (auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        vector<string> temp = (*it)->state_requirements_are_met(p);
        missing_state.insert(missing_state.end(), temp.begin(), temp.end());
    }

    if (!missing_state.empty()) {
        std::ostringstream message;
        message << "The following required state variables are missing in Gro: " << join_string_vector(missing_state);
        throw std::out_of_range(message.str());
    }
    */

    for (size_t i = 0; i < n_rows; ++i)
    {
        append_state_to_vector(current_state, state_history);
        append_state_to_vector(current_state, results);

        /*
         * 1) Calculate all state-dependent state variables.
         */

        /* NOTE: 
         * This section is for state variables that are not modified by derivatives.
         * No derivaties should be calulated here.
         * This makes it so that the code in section 2 is order independent.
         */
        p = combine_state(combine_state(invariant_parameters, at(varying_parameters, i)), current_state);

        for (auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
            state_map temp = (*it)->run(p);
            p.insert(temp.begin(), temp.end());
        }

        /*
         * 2) Calculate derivatives between state variables.
         */

        /* NOTE: This section should be written to calculate derivates only. No state should be modified.
         * All derivatives should depend only on current state. I.e., derivates should not depend on other derivaties or previous state.
         */

        state_map derivs; // There's no guarantee that each derivative will be set in each iteration, by declaring the variable within the loop all derivates will be set to 0 at each iteration.
        derivs.reserve(p.size());
        for (auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
            derivs += (*it)->run(state_history, deriv_history, p);
        }
    
        /*
         * 3) Update the state variables.
         */

        /* NOTE: This is the only spot where where state should be updated.
         * By updating everything at the end, the order of the previous statements will not
         * affect output. It should also allow us to use an ODE solver.
         */

        current_state = at(state_history, i);
        update_state(current_state, derivs * p.at("timestep"));

        append_state_to_vector(derivs, deriv_history);

        /*
         * 4) Record additional variables the results.
         */

        /* NOTE: We can write a recorder function so that the user can specify
         * what they want to record, but I don't know that anyone will care
         * and for now it's easier to just record all of the state variables and other
         * things of interest.
         */

        results["canopy_assimilation_rate"].emplace_back(p["canopy_assimilation_rate"]);
        results["canopy_transpiration_rate"].emplace_back(p["canopy_transpiration_rate"]);
        results["rate_constant_root_scale"].emplace_back(p["rate_constant_root_scale"]);
        results["lai"].emplace_back(p["lai"]);
        //results["soil_water_content"].emplace_back(s.at("soil_water_content"));
        results["stomatal_conductance_coefs"].emplace_back(p["StomataWS"]);
        results["LeafWS"].emplace_back(p["LeafWS"]);
        //results["leaf_reduction_coefs"].emplace_back(s.at("LeafWS"));
        //results["leaf_nitrogen"].emplace_back(s.at("LeafN"));
        results["vmax"].emplace_back(p["vmax"]);
        results["alpha"].emplace_back(p["alpha"]);
        results["specific_leaf_area"].emplace_back(p["Sp"]);
        results["soil_evaporation_rate"].emplace_back(p["soil_evaporation_rate"]);
        //results["kLeaf"].emplace_back(kLeaf);
        results["newLeafcol"].emplace_back(derivs["newLeafcol"]);
        results["newStemcol"].emplace_back(derivs["newStemcol"]);
        results["newRootcol"].emplace_back(derivs["newRootcol"]);
        results["newRhizomecol"].emplace_back(derivs["newRhizomecol"]);
        results["dLeaf"].emplace_back(derivs["Leaf"]);
        results["dGrain"].emplace_back(derivs["Grain"]);
        results["dStem"].emplace_back(derivs["Stem"]);
        results["dRoot"].emplace_back(derivs["Root"]);
        results["dRhizome"].emplace_back(derivs["Rhizome"]);

        results["dsubstrate_pool_leaf"].emplace_back(derivs["substrate_pool_leaf"]);
        results["dsubstrate_pool_stem"].emplace_back(derivs["substrate_pool_stem"]);
        results["dsubstrate_pool_grain"].emplace_back(derivs["substrate_pool_grain"]);
        results["dsubstrate_pool_root"].emplace_back(derivs["substrate_pool_root"]);
        results["dsubstrate_pool_rhizome"].emplace_back(derivs["substrate_pool_rhizome"]);

        results["transport_leaf_to_stem"].emplace_back(derivs["transport_leaf_to_stem"]);
        results["transport_stem_to_grain"].emplace_back(derivs["transport_stem_to_grain"]);
        results["transport_stem_to_root"].emplace_back(derivs["transport_stem_to_root"]);
        results["transport_stem_to_rhizome"].emplace_back(derivs["transport_stem_to_rhizome"]);

        results["utilization_leaf"].emplace_back(derivs["utilization_leaf"]);
        results["utilization_stem"].emplace_back(derivs["utilization_stem"]);
        results["utilization_grain"].emplace_back(derivs["utilization_grain"]);
        results["utilization_root"].emplace_back(derivs["utilization_root"]);



        //results["cws1"].emplace_back(current_state.at("cws1"));
        //results["cws2"].emplace_back(current_state.at("cws2"));
    }
    return results;
}

state_map Gro(
    state_map state,
    std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
    std::vector<std::unique_ptr<IModule>> const &derivative_modules)
{
    for (auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        state_map temp = (*it)->run(state);
        state.insert(temp.begin(), temp.end());
    }

    state_map derivs;
    derivs.reserve(state.size());
    for (auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
        derivs += (*it)->run(state);
    }
    
return derivs;
}

