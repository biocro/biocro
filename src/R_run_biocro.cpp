#include <Rinternals.h>
#include <string>
#include <exception>         // for std::exception
#include "state_map.h"       // for state_map, state_vector_map, string_vector
#include "module_wrapper.h"  // for mwp_vector
#include "biocro_simulation.h"
#include "R_helper_functions.h"

using std::string;

extern "C" {

SEXP R_run_biocro(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mwp_vec,
    SEXP differential_mwp_vec,
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

        mwp_vector direct_mwps = mw_vector_from_list(direct_mwp_vec);
        mwp_vector differential_mwps = mw_vector_from_list(differential_mwp_vec);

        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];
        string solver_type_string = CHAR(STRING_ELT(solver_type, 0));
        double output_step_size = REAL(solver_output_step_size)[0];
        double adaptive_rel_error_tol = REAL(solver_adaptive_rel_error_tol)[0];
        double adaptive_abs_error_tol = REAL(solver_adaptive_abs_error_tol)[0];
        int adaptive_max_steps = (int)REAL(solver_adaptive_max_steps)[0];

        biocro_simulation gro(iv, p, d, direct_mwps, differential_mwps,
                              solver_type_string, output_step_size,
                              adaptive_rel_error_tol, adaptive_abs_error_tol,
                              adaptive_max_steps);
        state_vector_map result = gro.run_simulation();

        if (loquacious) {
            Rprintf(gro.generate_report().c_str());
        }

        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_run_biocro: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_run_biocro.");
    }
}

}  // extern "C"
