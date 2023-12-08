#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error and Rprintf
#include "framework/R_helper_functions.h"  // for r_logical_from_boolean, map_from_list, map_vector_from_list, mc_vector_from_list
#include "framework/state_map.h"           // for state_map, state_vector_map
#include "framework/module_creator.h"      // for mc_vector
#include "framework/validate_dynamical_system.h"
#include "R_dynamical_system.h"

using std::string;

extern "C" {

SEXP R_validate_dynamical_system_inputs(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec,
    SEXP verbose)
{
    try {
        // Convert inputs from R formats
        state_map s = map_from_list(initial_values);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(drivers);
        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        mc_vector differential_mcs = mc_vector_from_list(differential_mc_vec);
        bool be_loud = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Check the validity
        string msg;
        bool valid = validate_dynamical_system_inputs(msg, s, ip, vp, direct_mcs, differential_mcs);

        // Print feedback and additional information if required
        if (be_loud) {
            Rprintf("\nChecking the validity of the system inputs:\n");

            Rprintf("%s", msg.c_str());

            if (valid) {
                Rprintf("\nSystem inputs are valid\n");
            } else {
                Rprintf("\nSystem inputs are not valid\n");
            }

            Rprintf("\nPrinting additional information about the system inputs:\n");

            msg = analyze_system_inputs(s, ip, vp, direct_mcs, differential_mcs);
            Rprintf("%s", msg.c_str());

            // Print a space to improve readability
            Rprintf("\n");
        }

        return r_logical_from_boolean(valid);

    } catch (std::exception const& e) {
        Rf_error("%s", (string("Caught exception in R_validate_dynamical_system_inputs: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_dynamical_system_inputs.");
    }
}

}  // extern "C"
