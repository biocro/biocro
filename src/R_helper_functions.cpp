#include <memory>
#include <stdexcept>
#include <Rinternals.h>
#include "modules.h"
#include "R_helper_functions.h"

using std::string;
using std::vector;

state_map map_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    size_t n = length(list);
    state_map m;
    m.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        m.insert(std::pair<string, double>(CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]));
    }
    return m;
}

state_vector_map map_vector_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    size_t n = length(list);
    state_vector_map m;
    m.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        size_t p = length(VECTOR_ELT(list, i));
        vector<double> temporary;
        temporary.reserve(p);
        for (size_t j = 0; j < p; ++j) {
            temporary.push_back(REAL(VECTOR_ELT(list, i))[j]);
        }
        m.insert(std::pair<string, vector<double>>(CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}

vector<string> make_vector(SEXP const &r_string_vector) {
    vector<string> v;
    size_t n = length(r_string_vector);
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        v.push_back(CHAR(STRING_ELT(r_string_vector, i)));
    }
    return v;
}

SEXP list_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        SET_VECTOR_ELT(list, i, ScalarReal(it->second));
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
    }
    setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP list_from_map(state_vector_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        size_t j = 0;
        auto second = it->second;
        auto p = second.size();
        SEXP values = PROTECT(allocVector(REALSXP, p));
        double* vp = REAL(values);  // It's a litte faster if you get a pointer and reuse it.
        for (auto vit = second.begin(); vit != second.end(); ++vit, ++j) {
            vp[j] = second[j];
        } 
        SET_VECTOR_ELT(list, i, values);
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
        UNPROTECT(1);
    }
    setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP vector_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP vector = PROTECT(allocVector(REALSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    double* vp = REAL(vector);  // It's a little faster if you get a pointer and reuse it.
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        vp[i] = it->second;
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
    }
    setAttrib(vector, R_NamesSymbol, names);
    UNPROTECT(2);
    return vector;
}

void output_map(state_map const &m) {
    if (!m.empty()) {
        size_t i = 0;
        auto it = m.begin();
        Rprintf("The map contains the following items: ");
        for(; std::next(it) != m.end(); ++it) {
            Rprintf("%s, %0.04f; ", it->first.c_str(), it->second);
            if (++i % 5 == 0) Rprintf("\n");
        }
        Rprintf("%s, %0.04f.\n\n", it->first.c_str(), it->second);
    } else {
        Rprintf("The map is empty.\n");
    }
}

void output_list(SEXP const &list) {
    size_t n = length(list);
    SEXP names = PROTECT(allocVector(STRSXP, n));
    names = getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (size_t i = 0; i < n - 1; ++i) {
        Rprintf("%s, %0.04f; ", CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf("%s, %0.04f.\n\n", CHAR(STRING_ELT(names, n - 1)), REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

