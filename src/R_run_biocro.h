#ifndef R_RUN_BIOCRO_H
#define R_RUN_BIOCRO_H

#include <Rinternals.h>  // for SEXP

extern "C" SEXP R_run_biocro(
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
    SEXP verbose);

#endif
