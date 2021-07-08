#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "state_map.h"
#include "validate_dynamical_system.h"
#include "dynamical_system.h"

extern "C" {

SEXP R_validate_dynamical_system_inputs(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_module_names,
    SEXP differential_module_names,
    SEXP silent)
{
    try {
        // Convert inputs from R formats
        state_map s = map_from_list(initial_values);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(drivers);
        std::vector<std::string> direct_names = make_vector(direct_module_names);
        std::vector<std::string> differential_names = make_vector(differential_module_names);
        bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];

        // Check the validity
        std::string msg;
        bool valid = validate_dynamical_system_inputs(msg, s, ip, vp, direct_names, differential_names);

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

            msg = analyze_system_inputs(s, ip, vp, direct_names, differential_names);
            Rprintf(msg.c_str());

            // Print a space to improve readability
            Rprintf("\n");
        }

        return r_logical_from_boolean(valid);

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_validate_dynamical_system_inputs: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_dynamical_system_inputs.");
    }
}

}  // extern "C"