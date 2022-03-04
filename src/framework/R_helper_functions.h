#ifndef R_HELPER_FUNCTIONS_H
#define R_HELPER_FUNCTIONS_H

#include <Rinternals.h>
#include <string>
#include "module_creator.h"  // for module_creator, mc_vector
#include "state_map.h"       // for state_map, string_vector

state_map map_from_list(SEXP const& list);

state_vector_map map_vector_from_list(SEXP const& list);

string_vector make_vector(SEXP const& r_string_vector);

mc_vector mc_vector_from_list(SEXP const& list);

SEXP list_from_map(state_map const& m);

SEXP list_from_map(state_vector_map const& m);

SEXP list_from_map(std::unordered_map<std::string, string_vector> const& m);

SEXP list_from_module_info(
    std::string const& module_name,
    string_vector const& module_inputs,
    string_vector const& module_outputs,
    bool const& is_differential,
    bool const& requires_euler_ode_solver,
    std::string const& creation_error_message);

SEXP vector_from_map(state_map const& m);

SEXP r_string_vector_from_vector(string_vector const& v);

SEXP r_logical_from_boolean(bool b);

void output_map(state_map const& m);

void output_list(SEXP const& list);

void finalize_module_creator(SEXP mw_ptr);

#endif
