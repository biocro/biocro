#ifndef R_CREATE_DYNAMICAL_SYSTEM_H
#define R_CREATE_DYNAMICAL_SYSTEM_H

#include <Rinternals.h>  // for SEXP

extern "C" SEXP R_create_dynamical_system(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec);

extern "C" SEXP R_get_system_modules(const SEXP handle);
extern "C" SEXP R_calculate_derivative(SEXP handle, SEXP time, SEXP state);

#endif
