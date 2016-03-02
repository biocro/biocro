#include <memory>
#include <stdexcept>
#include <Rinternals.h>
#include "modules.h"
#include "R_helper_functions.h"

state_map map_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    state_map m;
    int n = length(list);
    for (int i = 0; i < n; ++i) {
        m.insert(std::pair<string, double>(CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]));
    }
    return m;
}

state_vector_map map_vector_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    int n = length(list);
    state_vector_map m;
    for (int i = 0; i < n; ++i) {
        int p = length(VECTOR_ELT(list, i));
        vector<double> temporary;
        temporary.reserve(p);
        for (int j = 0; j < p; ++j) {
            temporary.push_back(REAL(VECTOR_ELT(list, i))[j]);
        }
        m.insert(std::pair<string, vector<double>>(CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}


SEXP list_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    int i = 0;
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
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        int j = 0;
        auto p = it->second.size();
        SEXP values = PROTECT(allocVector(REALSXP, p));
        for (auto vit = it->second.begin(); vit != it->second.end(); ++vit, ++j) {
            REAL(values)[j] = it->second[j];
        } 
        SET_VECTOR_ELT(list, i, values);
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
        UNPROTECT(1);
    }
    setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

void output_map(state_map const &m) {
    Rprintf("The map contains the following items: ");
    int i = 0;
    auto it = m.begin();
    for(; std::next(it) != m.end(); ++it) {
        Rprintf("%s, %0.04f; ", it->first.c_str(), it->second);
        if (++i % 5 == 0) Rprintf("\n");
    }
    Rprintf("%s, %0.04f.\n\n", it->first.c_str(), it->second);
}

void output_list(SEXP const &list) {
    int n;
    n = length(list);
    SEXP names = PROTECT(allocVector(STRSXP, n));
    names = getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (int i = 0; i < n - 1; ++i) {
        Rprintf("%s, %0.04f; ", CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf("%s, %0.04f.\n\n", CHAR(STRING_ELT(names, n - 1)), REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

