#include <stdexcept>
#include <sstream>
#include <Rinternals.h>
#include <cmath>  // For isnan
#include "module_library/BioCro.h"
#include "modules.h"
#include "biocro_simulation.h"
#include "module_wrapper_factory.h"
#include "R_helper_functions.h"
#include "standalone_ss.h"
#include "solver_library/SystemSolverFactory.hpp"
#include "validate_system.h"

using std::string;
using std::vector;
using std::unique_ptr;

extern "C" {

SEXP R_Gro_solver(SEXP initial_state,
                  SEXP parameters,
                  SEXP varying_parameters,
                  SEXP steady_state_module_names,
                  SEXP derivative_module_names,
                  SEXP solver_type,
                  SEXP solver_output_step_size,
                  SEXP solver_adaptive_error_tol,
                  SEXP solver_adaptive_max_steps,
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

        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];
        string solver_type_string = CHAR(STRING_ELT(solver_type, 0));
        double output_step_size = REAL(solver_output_step_size)[0];
        double adaptive_error_tol = REAL(solver_adaptive_error_tol)[0];
        int adaptive_max_steps = (int) REAL(solver_adaptive_max_steps)[0];

        biocro_simulation gro(s, ip, vp, ss_names, deriv_names,
                              solver_type_string, output_step_size,
                              adaptive_error_tol, adaptive_max_steps);
        state_vector_map result = gro.run_simulation();

        if (loquacious) {
            Rprintf(gro.generate_report().c_str());
        }

        return list_from_map(result);
    }
    catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_solver: ") + e.what()).c_str());
    }
    catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_solver.");
    }
}

SEXP R_Gro_deriv(SEXP state,
                 SEXP time,
                 SEXP parameters,
                 SEXP varying_parameters,
                 SEXP steady_state_module_names,
                 SEXP derivative_module_names,
                 SEXP verbose)
{
    try {
        // Convert the inputs into the proper format
        state_map s = map_from_list(state);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(varying_parameters);

        if (vp.begin()->second.size() == 0) {
            return R_NilValue;
        }

        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);

        bool verb = LOGICAL(VECTOR_ELT(verbose, 0))[0];
        double t = REAL(time)[0];

        // Create a system
        System sys(s, ip, vp, ss_names, deriv_names, verb, Rprintf);

        // Get the state in the correct format
        std::vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        std::vector<std::string> state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        std::vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, t);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) result[state_param_names[i]] = dxdt[i];

        // Return the resulting derivatives
        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_deriv: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_deriv.");
    }
}

SEXP R_Gro_ode(SEXP state,
               SEXP steady_state_module_names,
               SEXP derivative_module_names,
               SEXP verbose)
{
    try {
        // Get the input state
        state_map s = map_from_list(state);

        // There are a few special parameters that a system requires at startup,
        //  so make sure they are properly defined

        // Form the list of invariant parameters, making sure it includes the timestep
        state_map ip;
        if (s.find("timestep") == s.end()) {
            // The timestep is not defined in the input state, so assume it is 1
            ip["timestep"] = 1.0;
        } else {
            // The timestep is defined in the input state, so copy its value into the invariant parameters
            //  and remove it from the state
            ip["timestep"] = s["timestep"];
            s.erase("timestep");
        }

        // Form the list of varying parameters, making sure it includes doy and hour
        state_vector_map vp;
        if (s.find("doy") == s.end()) {
            // The doy is not defined in the input state, so assume it is 0
            std::vector<double> temp_vec;
            temp_vec.push_back(0.0);
            vp["doy"] = temp_vec;
        } else {
            // The doy is defined in the input state, so copy its value into the varying parameters
            std::vector<double> temp_vec;
            temp_vec.push_back(s["doy"]);
            vp["doy"] = temp_vec;
            s.erase("doy");
        }
        if (s.find("hour") == s.end()) {
            // The hour is not defined in the input state, so assume it is 0
            std::vector<double> temp_vec;
            temp_vec.push_back(0.0);
            vp["hour"] = temp_vec;
        } else {
            // The hour is defined in the input state, so copy its value into the varying parameters
            std::vector<double> temp_vec;
            temp_vec.push_back(s["hour"]);
            vp["hour"] = temp_vec;
            s.erase("hour");
        }

        // Get the module names
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);

        // Get the verbosity
        bool verb = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Make the system
        System sys(s, ip, vp, ss_names, deriv_names, verb, Rprintf);

        // Get the current state in the correct format
        std::vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        std::vector<std::string> state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        std::vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, 0);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) result[state_param_names[i]] = dxdt[i];

        // Return the resulting derivatives
        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_ode: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_ode.");
    }

    // Return an indication of success
    vector<string> result;
    result.push_back("System test completed");
    return r_string_vector_from_vector(result);
}

SEXP R_get_module_info(SEXP module_name_input)
{
    try {
        // module_name_input should be a string vector with one element
        std::vector<std::string> module_name_vector = make_vector(module_name_input);
        std::string module_name = module_name_vector[0];

        // Make a module factory
        std::unordered_map<std::string, double> parameters;
        std::unordered_map<std::string, double> module_output_map;
        module_wrapper_factory module_factory;

        // Get the module's description
        auto w = module_factory.create(module_name);
        std::string description = w->get_description();

        // Get the module's inputs and add them to the parameter list with default
        //  values
        std::vector<std::string> module_inputs = w->get_inputs();
        for (std::string param : module_inputs) parameters[param] = 1.0;
        std::unordered_map<std::string, double> input_map = parameters;

        // Get the module's outputs and add them to the parameter list with default
        //  values
        std::vector<std::string> module_outputs = w->get_outputs();
        for (std::string param : module_outputs) parameters[param] = 1.0;

        // Try to create an instance of the module
        module_output_map = parameters;
        std::unique_ptr<Module> module_ptr = w->createModule(&parameters, &module_output_map);

        // Check to see if the module is a derivative module
        bool is_deriv = module_ptr->is_deriv();

        // Send some messages to the user

        // Module name
        Rprintf("\n\nModule name:\n  %s\n\n", module_name.c_str());

        // Module type
        if (is_deriv)
            Rprintf("Module type (derivative or steady state):\n  derivative\n\n");
        else
            Rprintf("Module type (derivative or steady state):\n  steady state\n\n");

        // Module description
        Rprintf("Module description:\n%s\n\n", description.c_str());

        // Module inputs
        Rprintf("Module input parameters:");
        if (module_inputs.size() == 0)
            Rprintf(" none\n\n");
        else {
            for (std::string param : module_inputs) Rprintf("\n  %s", param.c_str());
            Rprintf("\n\n");
        }

        // Module outputs
        Rprintf("Module output parameters:");
        if (module_outputs.size() == 0)
            Rprintf(" none\n\n");
        else {
            for (std::string param : module_outputs) Rprintf("\n  %s", param.c_str());
            Rprintf("\n\n");
        }

        // Return a map containing the module's input parameters
        return list_from_map(input_map);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_module_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_module_info.");
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
        Standalone_SS module_combo(module_name_vector, input_param_ptrs, output_param_ptrs, true, Rprintf);
    } catch (std::length_error const& e) {
        // This is to be expected, so
        // don't do anything special
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_standalone_ss_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_standalone_ss_info.");
    }

    // Return an indication of success
    vector<string> result;
    result.push_back("Standalone_ss test completed");
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

        std::unordered_map<std::string, double> module_output_map;
        module_wrapper_factory module_factory;

        // Get the module's outputs and add them to the output list with default
        //  values of 0.0
        // Note: since derivative modules add their output to the module_output_map,
        //  the result only makes sense if each parameter is initialized to 0
        auto w = module_factory.create(module_name);
        std::vector<std::string> module_outputs = w->get_outputs();
        for (std::string param : module_outputs) module_output_map[param] = 0.0;

        std::unique_ptr<Module> module_ptr = w->createModule(&parameters, &module_output_map);

        module_ptr->run();

        return list_from_map(module_output_map);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_test_module: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_module.");
    }
}

SEXP R_test_standalone_ss(SEXP module_name_input, SEXP input_parameters, SEXP verbose)
{
    try {
        // module_name_input should be a string vector with one or more elements
        std::vector<std::string> module_name_vector = make_vector(module_name_input);

        // input_parameters should be a state map
        state_map i_parameters = map_from_list(input_parameters);

        // Get the verbosity
        bool verb = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        module_wrapper_factory module_factory;

        // Get all the outputs from the modules in the list, initializing
        //  their values to -1000000.0, which should make it obvious if any are not
        //  updated by the standalone_ss object
        state_map o_parameters;
        for (size_t i = 0; i < module_name_vector.size(); i++) {
            auto w = module_factory.create(module_name_vector[i]);
            std::vector<std::string> outputs = w->get_outputs();
            for (std::string p : outputs) o_parameters[p] = -1000000.0;
        }

        // Make input and output pointer maps and set their values
        std::unordered_map<std::string, const double*> input_param_ptrs;
        std::unordered_map<std::string, double*> output_param_ptrs;
        for (auto x : i_parameters) input_param_ptrs[x.first] = &i_parameters[x.first];
        for (auto x : o_parameters) {
            output_param_ptrs[x.first] = new double;
            *output_param_ptrs[x.first] = x.second;
        }

        // Make a standalone_ss object with verbose = TRUE
        Standalone_SS module_combo(module_name_vector, input_param_ptrs, output_param_ptrs, verb, Rprintf);

        // Run the standalone_ss
        module_combo.run();

        // Update the outputs
        for (auto x : output_param_ptrs) o_parameters[x.first] = *(x.second);

        // Return a map containing the module's parameter list
        return list_from_map(o_parameters);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_test_standalone_ss: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_standalone_ss.");
    }
}

SEXP R_test_system(SEXP initial_state,
                   SEXP parameters,
                   SEXP varying_parameters,
                   SEXP steady_state_module_names,
                   SEXP derivative_module_names)
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

        System sys(s, ip, vp, ss_names, deriv_names, TRUE);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_test_system: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_system.");
    }

    // Return an indication of success
    vector<string> result;
    result.push_back("System test completed");
    return r_string_vector_from_vector(result);
}

SEXP R_validate_system_inputs(
    SEXP initial_state,
    SEXP parameters,
    SEXP varying_parameters,
    SEXP steady_state_module_names,
    SEXP derivative_module_names,
    SEXP print_extra_info)
{
    // Convert inputs from R formats
    state_map s = map_from_list(initial_state);
    state_map ip = map_from_list(parameters);
    state_vector_map vp = map_vector_from_list(varying_parameters);
    std::vector<std::string> ss_names = make_vector(steady_state_module_names);
    std::vector<std::string> deriv_names = make_vector(derivative_module_names);
    bool print_extra = LOGICAL(VECTOR_ELT(print_extra_info, 0))[0];

    // Check the validity
    Rprintf("\nChecking the validity of the system inputs:\n");
    std::string msg;
    bool valid = validate_system_inputs(msg, s, ip, vp, ss_names, deriv_names);
    Rprintf(msg.c_str());
    
    // Print additional information if required
    if (print_extra) {
        Rprintf("\nPrinting additional information about the system inputs:\n");
        msg = analyze_system_inputs(s, ip, vp, ss_names, deriv_names);
        Rprintf(msg.c_str());
    }
    
    // Print a space to improve readability
    Rprintf("\n");

    // Return an indication of success
    vector<string> result;
    if (valid) {
        result.push_back("System inputs are valid");
    } else {
        result.push_back("System inputs are not valid");
    }

    return r_string_vector_from_vector(result);
}

SEXP R_get_all_modules()
{
    try {
        module_wrapper_factory module_factory;

        std::vector<std::string> result = module_factory.get_modules();

        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_all_modules: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_modules.");
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

}  // extern "C"
