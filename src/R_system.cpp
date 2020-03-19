#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "state_map.h"
#include "validate_system.h"
#include "system.h"

extern "C" {

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
        Rf_error((std::string("Caught exception in R_validate_system_inputs: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_system_inputs.");
    }
}

}  // extern "C"