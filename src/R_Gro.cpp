#include <stdexcept>
#include <sstream>
#include <Rinternals.h>
#include "BioCro.h"
#include "modules.h"
#include "ModuleFactory.h"
#include "R_helper_functions.h"

using std::string;
using std::vector;
using std::unique_ptr;

extern "C" {

SEXP R_Gro(SEXP initial_state,
        SEXP invariate_parameters,
        SEXP varying_parameters,
        SEXP canopy_photosynthesis_module,
        SEXP soil_water_module,
        SEXP growth_module,
        SEXP senescence_module,
        SEXP stomata_water_stress_module,
        SEXP leaf_water_stress_module)
{
    try {

        state_map s = map_from_list(initial_state);
        state_map ip = map_from_list(invariate_parameters);
        state_vector_map vp = map_vector_from_list(varying_parameters);

        if (vp.begin()->second.size() == 0) {
            return R_NilValue;
        }

        vector<unique_ptr<IModule>> steady_state_modules;
        vector<unique_ptr<IModule>> derivative_modules;

        steady_state_modules.push_back(module_factory("soil_type_selector"));
        steady_state_modules.push_back(module_factory(CHAR(STRING_ELT(stomata_water_stress_module, 0))));
        steady_state_modules.push_back(module_factory(CHAR(STRING_ELT(leaf_water_stress_module, 0))));
        steady_state_modules.push_back(module_factory("parameter_calculator"));
        steady_state_modules.push_back(module_factory("partitioning_coefficient_selector"));
        steady_state_modules.push_back(module_factory("soil_evaporation"));
        steady_state_modules.push_back(module_factory(CHAR(STRING_ELT(canopy_photosynthesis_module, 0))));

        derivative_modules.push_back(module_factory(CHAR(STRING_ELT(senescence_module, 0))));
        derivative_modules.push_back(module_factory(CHAR(STRING_ELT(growth_module, 0))));
        derivative_modules.push_back(module_factory("thermal_time_accumulator"));
        derivative_modules.push_back(module_factory(CHAR(STRING_ELT(soil_water_module, 0))));

        vector<string> required_state = {"iSp", "doy", "Leaf",
            "LeafN_0", "vmax_n_intercept", "vmax1", "alphab1",
            "alpha1", "TTc", "temp", "tbase", "timestep",
            "mrc1", "seneLeaf", "Stem", "seneStem",
            "mrc2", "Root", "seneRoot", "Rhizome", "seneRhizome", "kln", "growth_respiration_fraction"};

        state_map all_state = combine_state(combine_state(ip, at(vp, 0)), s);

        vector<string> missing_state;
        for (auto it = required_state.begin(); it != required_state.end(); ++it) {
            if (all_state.find(*it) == all_state.end()) {
                missing_state.push_back(*it);
            }
        }

        if (!missing_state.empty()) {
            std::ostringstream message;
            message << "The following state variables are required but are missing: ";
            for(vector<string>::iterator it = missing_state.begin(); it != missing_state.end() - 1; ++it) {
                message << *it << ", ";
            }
            message << missing_state.back() << ".";
            error(message.str().c_str());
        }

        state_vector_map result = Gro(s, ip, vp, steady_state_modules, derivative_modules);
        return (list_from_map(result));

    } catch (std::exception const &e) {
        error(string(string("Caught exception in R_Gro: ") + e.what()).c_str());
    } catch (...) {
        error("Caught unhandled exception in R_Gro.");
    }
}

SEXP R_Gro_ode(SEXP state,
        SEXP steady_state_modules_list,
        SEXP derivative_modules_list)
{
    try {
        state_map s = map_from_list(state);

        if (s.size() == 0) {
            return R_NilValue;
        }

        //ModuleFactory module_factory;

        vector<string> steady_state_names_vector = make_vector(steady_state_modules_list);
        vector<unique_ptr<IModule>> steady_state_modules;
        steady_state_names_vector.reserve(steady_state_names_vector.size());
        for (auto it = steady_state_names_vector.begin(); it != steady_state_names_vector.end(); ++it) {
            steady_state_modules.push_back(module_factory(*it));
        }

        vector<string> derivative_names_vector = make_vector(derivative_modules_list);
        vector<unique_ptr<IModule>> derivative_modules;
        derivative_modules.reserve(derivative_names_vector.size());
        for (auto it = derivative_names_vector.begin(); it != derivative_names_vector.end(); ++it) {
            derivative_modules.push_back(module_factory(*it));
        }

        state_map result = Gro(s, steady_state_modules, derivative_modules);
        return list_from_map(result);

    } catch (std::exception const &e) {
        error(string(string("Caught exception in R_Gro_ode: ") + e.what()).c_str());
    } catch (...) {
        error("Caught unhandled exception in R_Gro_ode.");
    }
}

SEXP R_run_modules(SEXP state,
        SEXP modules_list)
{
    try {
        state_map s = map_from_list(state);

        if (s.size() == 0) {
            return R_NilValue;
        }

        //ModuleFactory module_factory;

        vector<string> names_vector = make_vector(modules_list);
        vector<unique_ptr<IModule>> modules;
        names_vector.reserve(names_vector.size());
        for (auto it = names_vector.begin(); it != names_vector.end(); ++it) {
            modules.push_back(module_factory(*it));
        }

        for (auto it = modules.begin(); it != modules.end(); ++it) {
            state_map temp = (*it)->run(s);
            s.insert(temp.begin(), temp.end());
        }

        return list_from_map(s);

    } catch (std::exception const &e) {
        error(string(string("Caught exception in R_run_module: ") + e.what()).c_str());
    } catch (...) {
        error("Caught unhandled exception in R_run_module.");
    }
}
} // extern "C"

