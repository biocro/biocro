#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error and Rprintf
#include "framework/R_helper_functions.h"  // for map_from_list, map_vector_from_list, mc_vector_from_list, list_from_map
#include "framework/state_map.h"           // for state_map, state_vector_map, string_vector
#include "framework/module_creator.h"      // for mc_vector
#include "framework/biocro_simulation.h"
#include "R_run_biocro.h"

using std::string;

extern "C" {

SEXP R_run_biocro(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec,
    SEXP solver_type,
    SEXP solver_output_step_size,
    SEXP solver_adaptive_rel_error_tol,
    SEXP solver_adaptive_abs_error_tol,
    SEXP solver_adaptive_max_steps,
    SEXP verbose)
{
    try {
        state_map iv = map_from_list(initial_values);
        state_map p = map_from_list(parameters);
        state_vector_map d = map_vector_from_list(drivers);

        if (d.begin()->second.size() == 0) {
            return R_NilValue;
        }

        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        mc_vector differential_mcs = mc_vector_from_list(differential_mc_vec);

        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];
        string solver_type_string = CHAR(STRING_ELT(solver_type, 0));
        double output_step_size = REAL(solver_output_step_size)[0];
        double adaptive_rel_error_tol = REAL(solver_adaptive_rel_error_tol)[0];
        double adaptive_abs_error_tol = REAL(solver_adaptive_abs_error_tol)[0];
        int adaptive_max_steps = (int)REAL(solver_adaptive_max_steps)[0];

        biocro_simulation gro(iv, p, d, direct_mcs, differential_mcs,
                              solver_type_string, output_step_size,
                              adaptive_rel_error_tol, adaptive_abs_error_tol,
                              adaptive_max_steps);
        state_vector_map result = gro.run_simulation();

        if (loquacious) {
            Rprintf("%s", gro.generate_report().c_str());
        }

        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error("%s", string(string("Caught exception in R_run_biocro: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_run_biocro.");
    }
}

}  // extern "C"
