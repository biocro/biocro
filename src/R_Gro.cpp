#include <stdexcept>
#include <sstream>
#include <Rinternals.h>
#include "module_library/BioCro.h"
#include "modules.h"
#include "Gro.h"
#include "module_library/ModuleFactory.h"
#include "R_helper_functions.h"
#include "standalone_ss.h"

using std::string;
using std::vector;
using std::unique_ptr;

extern "C" {

SEXP R_Gro(SEXP initial_state,
		SEXP parameters,
		SEXP varying_parameters,
		SEXP steady_state_module_names,
		SEXP derivative_module_names,
		SEXP verbose)
{
	try {
		state_map s = map_from_list(initial_state);
		state_map ip = map_from_list(parameters);
		state_vector_map vp = map_vector_from_list(varying_parameters);
		
		if (vp.begin()->second.size() == 0) {
			return R_NilValue;
		}
		
		std::vector<std::string> ss_names = make_vector(steady_state_module_names);
		std::vector<std::string> deriv_names = make_vector(derivative_module_names);
		
		bool verb = LOGICAL(VECTOR_ELT(verbose, 0))[0];
		
		state_vector_map result = Gro(s, ip, vp, ss_names, deriv_names, verb);
		return list_from_map(result);
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_Gro: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_Gro.");
	}
}

SEXP R_get_module_info(SEXP module_name_input)
{
	try {
		// module_name_input should be a string vector with one element
		std::vector<std::string> module_name_vector = make_vector(module_name_input);
		std::string module_name = module_name_vector[0];
		
		// Make a module factory
		std::unordered_map<std::string, double> parameters;
		std::unordered_map<std::string, double> vector_module_output;
		ModuleFactory module_factory(&parameters, &vector_module_output);
		
		// Get the module's inputs and add them to the parameter list with default
		//  values
		std::vector<std::string> module_inputs = module_factory.get_inputs(module_name);
		for(std::string param : module_inputs) parameters[param] = 1.0;
		std::unordered_map<std::string, double> input_map = parameters;
		
		// Get the module's outputs and add them to the parameter list with default
		//  values
		std::vector<std::string> module_outputs = module_factory.get_outputs(module_name);
		for(std::string param : module_outputs) parameters[param] = 1.0;
		
		// Try to create an instance of the module
		vector_module_output = parameters;
		std::unique_ptr<Module> module_ptr = module_factory.create(module_name);
		
		// Check to see if the module is a derivative module
		bool is_deriv = module_ptr->is_deriv();
		
		// Send some messages to the user
		
		// Module name
		Rprintf("\n\nModule name:\n  %s\n\n", module_name.c_str());
		
		// Module type
		if(is_deriv) Rprintf("Module type (derivative or steady state):\n  derivative\n\n");
		else Rprintf("Module type (derivative or steady state):\n  steady state\n\n");
		
		// Module inputs
		Rprintf("Module input parameters:");
		if(module_inputs.size() == 0) Rprintf(" none\n\n");
		else {
			for(std::string param : module_inputs) Rprintf("\n  %s", param.c_str());
			Rprintf("\n\n");
		}
		
		// Module outputs
		Rprintf("Module output parameters:");
		if(module_outputs.size() == 0) Rprintf(" none\n\n");
		else {
			for(std::string param : module_outputs) Rprintf("\n  %s", param.c_str());
			Rprintf("\n\n");
		}
		
		// Return a map containing the module's input parameters
		return list_from_map(input_map);
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_get_module_info: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_get_module_info.");
	}
}

SEXP R_get_standalone_ss_info(SEXP module_name_input)
{
	try {
		// module_name_input should be a string vector with one or more elements
		std::vector<std::string> module_name_vector = make_vector(module_name_input);
		
		// Make a standalone_ss object with verbose = TRUE
		// Supply empty in/output_param_ptrs since we don't
		// know which are required. This will cause
		// a std::length_error to be generated.
		std::unordered_map<std::string, const double*> input_param_ptrs;
		std::unordered_map<std::string, double*> output_param_ptrs;
		Standalone_SS module_combo(module_name_vector, input_param_ptrs, output_param_ptrs, true);
	}
	catch (std::length_error const &e) {
		// This is to be expected, so
		// don't do anything special
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_get_standalone_ss_info: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_get_standalone_ss_info.");
	}
	
	// Return an indication of success
	vector<string> result;
	result.push_back("success");
	return r_string_vector_from_vector(result);
}

SEXP R_test_module(SEXP module_name_input, SEXP input_parameters)
{
	try {
		// module_name_input should be a string vector with one element
		std::vector<std::string> module_name_vector = make_vector(module_name_input);
		std::string module_name = module_name_vector[0];
		
		// input_parameters should be a state map
		// use it to initialize the parameter list
		state_map parameters = map_from_list(input_parameters);
		
		// Make a module factory
		std::unordered_map<std::string, double> vector_module_output;
		ModuleFactory module_factory(&parameters, &vector_module_output);
		
		// Get the module's outputs and add them to the parameter list with default
		//  values of -1000000.0, which should make it clear if a module fails to
		//  update one of its outputs
		std::vector<std::string> module_outputs = module_factory.get_outputs(module_name);
		for(std::string param : module_outputs) parameters[param] = -1000000.0;
		
		// Create an instance of the module
		vector_module_output = parameters;
		std::unique_ptr<Module> module_ptr = module_factory.create(module_name);
		
		// Run the module
		Rprintf("\n\nStarting to run %s...", module_name.c_str());
		module_ptr->run();
		Rprintf(" done!\n\n");
		
		// Return a map containing the module's parameter list
		return list_from_map(vector_module_output);
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_test_module: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_test_module.");
	}
}

SEXP R_get_all_modules()
{
	try {
		// Make a module factory using some bogus inputs
		std::unordered_map<std::string, double> parameters;
		std::unordered_map<std::string, double> vector_module_output;
		ModuleFactory module_factory(&parameters, &vector_module_output);
		
		// Get the list of all modules from the module factory
		std::vector<std::string> result = module_factory.get_modules();
		
		// Return it
		return r_string_vector_from_vector(result);
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_get_all_modules: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_get_all_modules.");
	}
}

SEXP R_get_all_param()
{
	try {
		// Make a module factory using some bogus inputs
		std::unordered_map<std::string, double> parameters;
		std::unordered_map<std::string, double> vector_module_output;
		ModuleFactory module_factory(&parameters, &vector_module_output);
		
		// Get the list of all modules from the module factory
		std::unordered_map<std::string, std::vector<std::string>> result = module_factory.get_all_param();
		
		// Return it
		return list_from_map(result);
	}
	catch (std::exception const &e) {
		error(string(string("Caught exception in R_get_all_param: ") + e.what()).c_str());
	}
	catch (...) {
		error("Caught unhandled exception in R_get_all_param.");
	}
}

/*
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

SEXP R_get_module_requirements(SEXP modules_list)
{
    try {
        vector<string> names_vector = make_vector(modules_list);
        vector<unique_ptr<IModule>> modules;
        for (auto it = names_vector.begin(); it != names_vector.end(); ++it) {
            modules.push_back(module_factory(*it));
        }

        vector<string> parameters;
        for (auto it = modules.begin(); it != modules.end(); ++it) {
            std::vector<string> temp = (*it)->list_required_state();
            parameters.insert(parameters.end(), temp.begin(), temp.end());
        }

        return r_string_vector_from_vector(parameters);

    } catch (std::exception const &e) {
        error(string(string("Caught exception in R_run_module: ") + e.what()).c_str());
    } catch (...) {
        error("Caught unhandled exception in R_run_module.");
    }
}
*/

} // extern "C"

