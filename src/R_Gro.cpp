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
#include "simultaneous_equations.h"
#include "numerical_jacobian.h"

using std::string;
using std::unique_ptr;
using std::vector;

extern "C" {

SEXP R_Gro_solver(
    SEXP initial_state,
    SEXP parameters,
    SEXP varying_parameters,
    SEXP steady_state_module_names,
    SEXP derivative_module_names,
    SEXP solver_type,
    SEXP solver_output_step_size,
    SEXP solver_adaptive_rel_error_tol,
    SEXP solver_adaptive_abs_error_tol,
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
        double adaptive_rel_error_tol = REAL(solver_adaptive_rel_error_tol)[0];
        double adaptive_abs_error_tol = REAL(solver_adaptive_abs_error_tol)[0];
        int adaptive_max_steps = (int)REAL(solver_adaptive_max_steps)[0];

        biocro_simulation gro(s, ip, vp, ss_names, deriv_names,
                              solver_type_string, output_step_size,
                              adaptive_rel_error_tol, adaptive_abs_error_tol,
                              adaptive_max_steps);
        state_vector_map result = gro.run_simulation();

        if (loquacious) {
            Rprintf(gro.generate_report().c_str());
        }

        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_solver: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_solver.");
    }
}

SEXP R_Gro_deriv(
    SEXP state,
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
        System sys(s, ip, vp, ss_names, deriv_names);

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

SEXP R_Gro_ode(
    SEXP state,
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
        System sys(s, ip, vp, ss_names, deriv_names);

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

SEXP R_get_module_info(SEXP module_name_input, SEXP verbose)
{
    try {
        // module_name_input should be a string vector with one element
        std::vector<std::string> module_name_vector = make_vector(module_name_input);
        std::string module_name = module_name_vector[0];

        // Convert verbose to a boolean
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

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

        // Check to see if the module is compatible with adaptive step size solvers
        bool is_adaptive_compatible = module_ptr->is_adaptive_compatible();

        // Send some messages to the user if required

        if (loquacious) {
            // Module name
            Rprintf("\n\nModule name:\n  %s\n\n", module_name.c_str());

            // Module type
            if (is_deriv)
                Rprintf("Module type (derivative or steady state):\n  derivative\n\n");
            else
                Rprintf("Module type (derivative or steady state):\n  steady state\n\n");

            // Adaptive compatibility
            if (is_adaptive_compatible)
                Rprintf("Compatible with adaptive step size solvers:\n  yes\n\n");
            else
                Rprintf("Compatible with adaptive step size solvers:\n  no\n\n");

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
        }

        // Return a map containing the module's input parameters
        return list_from_map(input_map);
    } catch (quantity_access_error const& qae) {
        std::string error_msg = std::string("Caught quantity access error in R_test_module: ") + qae.what();
        Rf_error(error_msg.c_str());
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

        std::string message = std::string("\nFinding all quantities required as inputs or produced as outputs by the modules:\n");

        // Get the required inputs
        string_set module_inputs = find_strictly_required_inputs(std::vector<string_vector>{module_name_vector});
        process_criterion<string_set>(
            message,
            [=]() -> string_set { return module_inputs; },
            [](string_set string_list) -> std::string { return create_message(
                                                               std::string("No quantities were required by the set of modules"),
                                                               std::string("The following quantities were required by the set of modules:"),
                                                               std::string(""),
                                                               string_list); });

        // Get the outputs
        string_set module_outputs = find_unique_module_outputs(std::vector<string_vector>{module_name_vector});
        process_criterion<string_set>(
            message,
            [=]() -> string_set { return module_outputs; },
            [](string_set string_list) -> std::string { return create_message(
                                                            std::string("No quantities were produced by any of the modules"),
                                                            std::string("The following quantities were each produced by at least one module:"),
                                                            std::string(""),
                                                            string_list); });

        // Make a bogus input/output pointer vectors
        double bogus_value;

        std::unordered_map<std::string, const double*> bogus_input_ptrs;
        for (std::string const& name : module_inputs) {
            bogus_input_ptrs[name] = &bogus_value;
        }

        std::unordered_map<std::string, double*> bogus_output_ptrs;
        for (std::string const& name : module_outputs) {
            bogus_output_ptrs[name] = &bogus_value;
        }

        // Use the bogus pointers to check the validity of the set of modules
        message += std::string("\nChecking the ability to create a Standalone_SS from these modules, assuming all inputs and outputs are properly supplied:\n");
        bool valid = validate_standalone_ss_inputs(message, module_name_vector, bogus_input_ptrs, bogus_output_ptrs);
        if (valid) {
            message += std::string("\nConclusion: The set of modules is valid and can be used to specify a Standalone_SS object.\n\n");
        } else {
            message += std::string("\n");
            Rprintf(message.c_str());
            throw std::logic_error(std::string("The set of modules is not valid and cannot be used to specify a Standalone_SS object."));
        }

        // Print the message
        Rprintf(message.c_str());

        // Return a map containing the module's input parameters
        return list_from_map(state_map_from_names(module_inputs));

    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_standalone_ss_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_standalone_ss_info.");
    }
}

SEXP R_validate_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP steady_state_module_names,
    SEXP silent)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        std::vector<std::string> uq = make_vector(unknown_quantities);
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];
        
        std::string msg;
        bool valid = validate_simultanous_equations_inputs(msg, kq, uq, ss_names);
        
        if (!be_quiet) {
            Rprintf("\nChecking the validity of the simultaneous_equations inputs:\n");

            Rprintf(msg.c_str());

            if (valid) {
                Rprintf("\nSimultaneous_equations inputs are valid\n");
            } else {
                Rprintf("\nSimultaneous_equations inputs are not valid\n");
            }

            Rprintf("\nPrinting additional information about the simultaneous_equations inputs:\n");

            msg = analyze_simultanous_equations_inputs(ss_names);
            Rprintf(msg.c_str());

            // Print a space to improve readability
            Rprintf("\n");
        }

        return r_logical_from_boolean(valid);

    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_validate_simultaneous_equations: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_simultaneous_equations.");
    }
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
    } catch (quantity_access_error const& qae) {
        std::string error_msg = std::string("Caught quantity access error in R_test_module: ") + qae.what();
        Rf_error(error_msg.c_str());
    } catch (std::exception const& e) {
        std::string error_msg = std::string("Caught exception in R_test_module: ") + e.what();
        Rf_error(error_msg.c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_module.");
    }
}

SEXP R_test_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP steady_state_module_names)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        state_map uq = map_from_list(unknown_quantities);
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        
        // Split uq into two vectors
        string_vector uq_names = keys(uq);
        std::vector<double> uq_values(uq_names.size());
        for (size_t i = 0; i < uq_names.size(); i++) {
            uq_values[i] = uq[uq_names[i]];
        }
        
        // Make the simultaneous equation object
        std::shared_ptr<simultaneous_equations> se(new simultaneous_equations(kq, uq_names, ss_names));
        
        // Calculate a difference vector
        std::vector<double> output_vector(uq.size());
        se->operator()(uq_values, output_vector);
        
        // Calculate a Jacobian matrix
        boost::numeric::ublas::matrix<double> jacobian(uq.size(), uq.size());
        calculate_jacobian(se, uq_values, jacobian);
        
        // Print its values to the R console
        std::string msg = std::string("\n\n\n");
        for (size_t i = 0; i < jacobian.size1(); ++i) {
            msg += std::string("\n");
            for (size_t j = 0; j < jacobian.size2(); ++j) {
                msg += std::to_string(jacobian(i,j)) + std::string(" ");
            }
        }
        msg += std::string("\n\n\n");
        Rprintf(msg.c_str());
        
        // Return the difference vector in a nice format
        state_map result;
        for (size_t i = 0; i < uq.size(); i++) {
            result[uq_names[i]] = output_vector[i];
        }
        
        return list_from_map(result);
        
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_test_simultaneous_equations: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_simultaneous_equations.");
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
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

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

        // Make a standalone_ss object
        Standalone_SS module_combo(module_name_vector, input_param_ptrs, output_param_ptrs);

        if (loquacious) {
            std::string message = module_combo.generate_startup_report() + std::string("\n");
            Rprintf(message.c_str());
        }

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

SEXP R_test_system(
    SEXP initial_state,
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

        System sys(s, ip, vp, ss_names, deriv_names);
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
    SEXP silent)
{
    try {
        // Convert inputs from R formats
        state_map s = map_from_list(initial_state);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(varying_parameters);
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);
        bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];

        // Check the validity
        std::string msg;
        bool valid = validate_system_inputs(msg, s, ip, vp, ss_names, deriv_names);

        // Print feedback and additional information if required
        if (!be_quiet) {
            Rprintf("\nChecking the validity of the system inputs:\n");

            Rprintf(msg.c_str());

            if (valid) {
                Rprintf("\nSystem inputs are valid\n");
            } else {
                Rprintf("\nSystem inputs are not valid\n");
            }

            Rprintf("\nPrinting additional information about the system inputs:\n");

            msg = analyze_system_inputs(s, ip, vp, ss_names, deriv_names);
            Rprintf(msg.c_str());

            // Print a space to improve readability
            Rprintf("\n");
        }

        return r_logical_from_boolean(valid);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_validate_system_inputs: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_system_inputs.");
    }
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

SEXP R_get_all_quantities()
{
    try {
        std::unordered_map<std::string, std::vector<std::string>> all_quantities = module_wrapper_factory::get_all_quantities();
        return list_from_map(all_quantities);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_all_param: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_param.");
    }
}

SEXP R_get_all_solvers()
{
    try {
        std::vector<std::string> result = system_solver_factory.get_solvers();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_get_all_solvers: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_solvers.");
    }
}

}  // extern "C"
