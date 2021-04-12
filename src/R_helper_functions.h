#ifndef R_HELPER_FUNCTIONS_H
#define R_HELPER_FUNCTIONS_H

#include <Rinternals.h>
#include <vector>
#include <string>
#include "state_map.h"

state_map map_from_list(SEXP const &list);

state_vector_map map_vector_from_list(SEXP const &list);

std::vector<std::string> make_vector(SEXP const &r_string_vector);

SEXP list_from_map(state_map const &m);

SEXP list_from_map(state_vector_map const &m);

SEXP list_from_map(std::unordered_map<std::string, std::vector<std::string>> const &m);

SEXP vector_from_map(state_map const &m);

SEXP r_string_vector_from_vector(std::vector<std::string> const &v);

SEXP r_logical_from_boolean(bool b);

void output_map(state_map const &m);

void output_list(SEXP const &list);

#endif

