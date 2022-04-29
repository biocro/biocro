#ifndef R_SIMULTANEOUS_EQUATIONS_H
#define R_SIMULTANEOUS_EQUATIONS_H

#include <Rinternals.h>  // for SEXP

extern "C" SEXP R_solve_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP lower_bounds,
    SEXP upper_bounds,
    SEXP abs_error_tols,
    SEXP rel_error_tols,
    SEXP direct_mc_vec,
    SEXP solver_type,
    SEXP max_it,
    SEXP silent);

extern "C" SEXP R_validate_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP direct_mc_vec,
    SEXP silent);

extern "C" SEXP R_test_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP direct_mc_vec);

#endif
