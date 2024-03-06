#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error and Rprintf
#include <memory>                          // for unique_ptr
#include "framework/R_helper_functions.h"  // for mc_vector_from_list, list_from_module_info, list_from_map
#include "framework/state_map.h"           // for state_map, string_vector
#include "framework/module_creator.h"
#include "framework/module.h"
#include "R_modules.h"

using std::string;

extern "C" {

/**
 *  @brief Determines vital information about a single module
 *
 *  @param [in] mw_ptr_vec A single-element vector containing one R external
 *              pointer pointing to a module_creator object, typically
 *              produced by the `R_module_creators()` function. If the
 *              vector has more than one element, only the first will be used.
 *
 *  @param [in] verbose When verbose is TRUE, print information to the R console
 *
 *  @return An R list containing several named elements that describe the
 *          module. See `list_from_module_info()` for more details.
 */
SEXP R_module_info(SEXP mw_ptr_vec, SEXP verbose)
{
    try {
        // Get the module_creator pointer
        module_creator* w = mc_vector_from_list(mw_ptr_vec)[0];

        // Convert verbose to a boolean
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Get the module's name
        string module_name = w->get_name();

        // Make maps for the module's inputs and outputs
        state_map module_inputs;
        state_map module_outputs;

        // Get the module's inputs and give them default values
        double const default_value = 1.0;
        string_vector inputs = w->get_inputs();
        for (string param : inputs) {
            module_inputs[param] = default_value;
        }

        // Get the module's outputs and give them default values
        string_vector outputs = w->get_outputs();
        for (string param : outputs) {
            module_outputs[param] = default_value;
        }

        // Try to create an instance of the module
        bool create_success = true;
        bool is_differential = false;
        bool requires_euler_ode_solver = false;
        string creation_error_message = "none";
        try {
            std::unique_ptr<module> module_ptr = w->create_module(
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
                for (string param : inputs) {
                    Rprintf("\n  %s", param.c_str());
                }
                Rprintf("\n\n");
            }

            // Module outputs
            Rprintf("Module output quantities:");
            if (outputs.size() == 0)
                Rprintf(" none\n\n");
            else {
                for (string param : outputs) {
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
                Rprintf("%s", creation_error_message.c_str());
                Rprintf("\n\n");
            }
        }

        // Return a list containing information about the module
        return list_from_module_info(
            module_name,
            keys(module_inputs),
            keys(module_outputs),
            is_differential,
            requires_euler_ode_solver,
            creation_error_message);

    } catch (quantity_access_error const& qae) {
        Rf_error("%s", (string("Caught quantity access error in R_module_info: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error("%s", (string("Caught exception in R_module_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_module_info.");
    }
}

/**
 *  @brief Determines the values of a module's output quantities from the
 *         supplied values of its input quantities
 *
 *  @param [in] mw_ptr_vec A single-element vector containing one R external
 *              pointer pointing to a module_creator object, typically
 *              produced by the `R_module_creators()` function. If the
 *              vector has more than one element, only the first will be used.
 *
 *  @param [in] input_quantities A list of named numeric elements where the name
 *              of each element corresponds to one of the module's input
 *              quantities. Any element whose name does not correspond to one of
 *              the module's input quantities will be ignored by the module.
 *
 *  @return A list of named numeric elements where the name of each element
 *          corresponds to the one of the module's output quantities
 */
SEXP R_evaluate_module(SEXP mw_ptr_vec, SEXP input_quantities)
{
    try {
        // Get the module_creator pointer
        module_creator* w = mc_vector_from_list(mw_ptr_vec)[0];

        // input_quantities should be a state map
        // use it to initialize the quantity list
        state_map quantities = map_from_list(input_quantities);
        state_map module_output_map;

        // Get the module's outputs and add them to the output list with default
        // values of 0.0. Since derivative modules add their output values to
        // the values in module_output_map, the result only makes sense if each
        // parameter is initialized to 0.
        string_vector module_outputs = w->get_outputs();
        for (string param : module_outputs) {
            module_output_map[param] = 0.0;
        }

        std::unique_ptr<module> module_ptr =
            w->create_module(quantities, &module_output_map);

        module_ptr->run();

        return list_from_map(module_output_map);

    } catch (quantity_access_error const& qae) {
        Rf_error("%s", (string("Caught quantity access error in R_evaluate_module: ") + qae.what()).c_str());
    } catch (std::exception const& e) {
        Rf_error("%s", (string("Caught exception in R_evaluate_module: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_evaluate_module.");
    }
}

}  // extern "C"
