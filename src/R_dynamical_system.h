#ifndef R_DYNAMICAL_SYSTEM_H
#define R_DYNAMICAL_SYSTEM_H

#include <Rinternals.h>  // for SEXP

extern "C" SEXP R_validate_dynamical_system_inputs(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec,
    SEXP verbose);

#endif
