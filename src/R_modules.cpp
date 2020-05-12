#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "state_map.h"
#include "module_library/module_wrapper_factory.h"
#include "modules.h"

extern "C" {

SEXP R_get_module_info(SEXP module_name_input, SEXP verbose)
{
    try {
        // module_name_input should be a string vector with one element
        std::vector<std::string> module_name_vector = make_vector(module_name_input);
        std::string module_name = module_name_vector[0];

        // Convert verbose to a boolean
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Make a module factory
        state_map parameters;
        state_map module_output_map;

        // Get the module's description
        auto w = module_wrapper_factory::create(module_name);
        std::string description = w->get_description();

        // Get the module's inputs and add them to the parameter list with default
        //  values
        std::vector<std::string> module_inputs = w->get_inputs();
        for (std::string param : module_inputs) parameters[param] = 1.0;
        state_map input_map = parameters;

        // Get the module's outputs and add them to the parameter list with default
        //  values
        std::vector<std::string> module_outputs = w->get_outputs();
        for (std::string param : module_outputs) parameters[param] = 1.0;

        // Try to create an instance of the module
        module_output_map = parameters;
        bool create_success = true;
        bool is_deriv = false;
        bool is_adaptive_compatible = false;
        std::string error_message;
        try {
            std::unique_ptr<Module> module_ptr = w->createModule(&parameters, &module_output_map);

            // Check to see if the module is a derivative module
            is_deriv = module_ptr->is_deriv();

            // Check to see if the module is compatible with adaptive step size solvers
            is_adaptive_compatible = module_ptr->is_adaptive_compatible();
        } catch (std::exception const& e) {
            create_success = false;
            error_message = e.what();
        }

        // Send some messages to the user if required
        if (loquacious) {
            // Module name
            Rprintf("\n\nModule name:\n  %s\n\n", module_name.c_str());

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

            if (create_success) {
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
            } else {
                Rprintf("Error: could not create the module\n");
                Rprintf("Additional details:\n");
                Rprintf(error_message.c_str());
                Rprintf("\n\n");
            }
        }

        // Return a map containing the module's input parameters
        return list_from_map(input_map);
    } catch (quantity_access_error const& qae) {
        Rf_error((std::string("Caught quantity access error in R_test_module: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_module_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_module_info.");
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
        state_map module_output_map;

        // Get the module's outputs and add them to the output list with default
        //  values of 0.0
        // Note: since derivative modules add their output to the module_output_map,
        //  the result only makes sense if each parameter is initialized to 0
        auto w = module_wrapper_factory::create(module_name);
        std::vector<std::string> module_outputs = w->get_outputs();
        for (std::string param : module_outputs) module_output_map[param] = 0.0;

        std::unique_ptr<Module> module_ptr = w->createModule(&parameters, &module_output_map);

        module_ptr->run();

        return list_from_map(module_output_map);
    } catch (quantity_access_error const& qae) {
        Rf_error((std::string("Caught quantity access error in R_test_module: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_test_module: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_module.");
    }
}

}  // extern "C"
