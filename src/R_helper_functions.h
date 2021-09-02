#ifndef R_HELPER_FUNCTIONS_H
#define R_HELPER_FUNCTIONS_H

#include <Rinternals.h>
#include <vector>
#include <string>
#include "state_map.h"

state_map map_from_list(SEXP const& list);

state_vector_map map_vector_from_list(SEXP const& list);

string_vector make_vector(SEXP const& r_string_vector);

SEXP list_from_map(state_map const& m);

SEXP list_from_map(state_vector_map const& m);

SEXP list_from_map(std::unordered_map<std::string, string_vector> const& m);

SEXP list_from_module_info(
    std::string const& module_name,
    state_map const& module_inputs,
    state_map const& module_outputs,
    bool const& is_differential,
    bool const& requires_euler_ode_solver,
    std::string const& creation_error_message);

SEXP vector_from_map(state_map const& m);

SEXP r_string_vector_from_vector(string_vector const& v);

SEXP r_logical_from_boolean(bool b);

void output_map(state_map const& m);

void output_list(SEXP const& list);

#endif
