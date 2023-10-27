#ifndef R_SYSTEM_DERIVATIVES_H
#define R_SYSTEM_DERIVATIVES_H

#include <Rinternals.h>  // for SEXP

extern "C" SEXP R_system_derivatives(
    SEXP differential_quantities,
    SEXP time,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec);

#endif
