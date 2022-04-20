#ifndef R_MODULES_H
#define R_MODULES_H

#include <Rinternals.h> // for SEXP

extern "C" SEXP R_module_info(SEXP mw_ptr_vec, SEXP verbose);
extern "C" SEXP R_evaluate_module(SEXP mw_ptr_vec, SEXP input_quantities);

#endif
