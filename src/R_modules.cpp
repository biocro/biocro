#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "state_map.h"
#include "module_library/module_wrapper_factory.h"
#include "modules.h"

extern "C" {

SEXP R_module_info(SEXP module_name_input, SEXP verbose)
{
    try {
        // module_name_input should be a string vector with one element
        std::vector<std::string> module_name_vector =
            make_vector(module_name_input);
        std::string module_name = module_name_vector[0];

        // Convert verbose to a boolean
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Make maps for the module's inputs and outputs
        state_map module_inputs;
        state_map module_outputs;

        // Get the module wrapper
        auto w = module_wrapper_factory::create(module_name);

        // Get the module's inputs and give them default values
        double const default_value = 1.0;
        std::vector<std::string> inputs = w->get_inputs();
        for (std::string param : inputs) {
            module_inputs[param] = default_value;
        }

        // Get the module's outputs and give them default values
        std::vector<std::string> outputs = w->get_outputs();
        for (std::string param : outputs) {
            module_outputs[param] = default_value;
        }

        // Try to create an instance of the module
        bool create_success = true;
        bool is_differential = false;
        bool requires_euler_ode_solver = false;
        std::string creation_error_message = "none";
        try {
            std::unique_ptr<module_base> module_ptr = w->createModule(
                module_inputs,
                &module_outputs);

            // Check to see if the module is a differential module
            is_differential = module_ptr->is_differential();

            // Check to see if the module requires an Euler ode_solver
            requires_euler_ode_solver = module_ptr->requires_euler_ode_solver();
        } catch (std::exception const& e) {
            create_success = false;
            creation_error_message = e.what();
        }

        // Send some messages to the user if required
        if (loquacious) {
            // Module name
            Rprintf("\n\nModule name:\n  %s\n\n", module_name.c_str());

            // Module inputs
            Rprintf("Module input quantities:");
            if (inputs.size() == 0)
                Rprintf(" none\n\n");
            else {
                for (std::string param : inputs) {
                    Rprintf("\n  %s", param.c_str());
                }
                Rprintf("\n\n");
            }

            // Module outputs
            Rprintf("Module output quantities:");
            if (outputs.size() == 0)
                Rprintf(" none\n\n");
            else {
                for (std::string param : outputs) {
                    Rprintf("\n  %s", param.c_str());
                }
                Rprintf("\n\n");
            }

            if (create_success) {
                // Module type
                Rprintf("Module type (differential or direct):\n  ");
                if (is_differential)
                    Rprintf("differential\n\n");
                else
                    Rprintf("direct\n\n");

                // Euler requirement
                Rprintf("Requires a fixed step size Euler ode_solver:\n  ");
                if (requires_euler_ode_solver)
                    Rprintf("yes\n\n");
                else
                    Rprintf("no\n\n");
            } else {
                Rprintf("Error: could not create the module\n");
                Rprintf("Additional details:\n");
                Rprintf(creation_error_message.c_str());
                Rprintf("\n\n");
            }
        }

        // Return a list containing information about the module
        return list_from_module_info(
            module_name,
            module_inputs,
            module_outputs,
            is_differential,
            requires_euler_ode_solver,
            creation_error_message);

    } catch (quantity_access_error const& qae) {
        Rf_error((std::string("Caught quantity access error in R_module_info: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_module_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_module_info.");
    }
}

SEXP R_evaluate_module(SEXP module_name_input, SEXP input_quantities)
{
    try {
        // module_name_input should be a string vector with one element
        std::vector<std::string> module_name_vector = make_vector(module_name_input);
        std::string module_name = module_name_vector[0];

        // input_quantities should be a state map
        // use it to initialize the quantity list
        state_map quantities = map_from_list(input_quantities);
        state_map module_output_map;

        // Get the module's outputs and add them to the output list with default
        //  values of 0.0
        // Note: since differential modules add their output to the module_output_map,
        //  the result only makes sense if each quantity is initialized to 0
        auto w = module_wrapper_factory::create(module_name);
        std::vector<std::string> module_outputs = w->get_outputs();
        for (std::string param : module_outputs) module_output_map[param] = 0.0;

        std::unique_ptr<module_base> module_ptr = w->createModule(quantities, &module_output_map);

        module_ptr->run();

        return list_from_map(module_output_map);
    } catch (quantity_access_error const& qae) {
        Rf_error((std::string("Caught quantity access error in R_evaluate_module: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_evaluate_module: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_evaluate_module.");
    }
}

}  // extern "C"
