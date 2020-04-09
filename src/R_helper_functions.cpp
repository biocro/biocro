#include <memory>
#include <stdexcept>
#include <Rinternals.h>
#include "modules.h"
#include "R_helper_functions.h"

using std::string;
using std::vector;

/*
 * !!!!! IMPORTANT NOTE !!!!!:
 * This is a C++ interface to the C library in R. C++ supports exceptions, but C does not.
 * R's C library requires you to explicitly PROTECT() and UNPROTECT() memory for garbage collection.
 * If care is not taken, and an exception is thrown after memory has been PROTECTed, but before it has been UNPROTECTed, UNPROTECT will not be called.
 *
 * Every effort should be made to not use exception-throwing functions between PROTECT and UNPROTECT pairs.
 * If an exception-throwing function is used between pairs, a means of ensuring that UNPROTECT is called when exceptions are thrown is required.
 *
 * The ScopeGuard pattern could be used which would store protect_count internally, and call UNPROTECT(protect_count) in the destructor.
 * It would be used as follows:
 *
 * guard = ProtectGuard();  // ProtectGuard is an implementation of the ScopeGuard pattern.
 * PROTECT(something);
 * guard.increment_protect_count(1);  //  Call this immediately after each set of PROTECT calls using the appropriate number of items to protect.
 *
 * The ProtectGuard destructor will be called when the function exists, regardless whether exceptions were thrown.
 *
 * Reference:
 * For std::unordered_map and std::vector, begin(), end(), size() and operator[() are guaranteed to not throw exceptions.
 *
 */

state_map map_from_list(SEXP const &list)
{
    SEXP names = Rf_getAttrib(list, R_NamesSymbol);
    size_t n = Rf_length(list);
    state_map m;
    m.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        m.emplace(CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    return m;
}

state_vector_map map_vector_from_list(SEXP const &list)
{
    SEXP names = Rf_getAttrib(list, R_NamesSymbol);
    size_t n = Rf_length(list);
    state_vector_map m;
    m.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        size_t p = Rf_length(VECTOR_ELT(list, i));
        vector<double> temporary;
        temporary.reserve(p);
        for (size_t j = 0; j < p; ++j) {
            temporary.emplace_back(REAL(VECTOR_ELT(list, i))[j]);
        }
        m.insert(state_vector_map::value_type(CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}

vector<string> make_vector(SEXP const &r_string_vector) {
    vector<string> v;
    size_t n = Rf_length(r_string_vector);
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        v.emplace_back(CHAR(STRING_ELT(r_string_vector, i)));
    }
    return v;
}

SEXP r_string_vector_from_vector(vector<string> const &v) {
    auto n = v.size();
    SEXP r_string_vector = PROTECT(Rf_allocVector(STRSXP, n));
    for (size_t i = 0; i < n; ++i) {
        SET_STRING_ELT(r_string_vector, i, Rf_mkChar(v[i].c_str()));
    }
    UNPROTECT(1);
    return r_string_vector;
}

SEXP list_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(Rf_allocVector(VECSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        SET_VECTOR_ELT(list, i, Rf_ScalarReal(it->second));
        SET_STRING_ELT(names, i, Rf_mkChar(it->first.c_str()));
    }
    Rf_setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP list_from_map(state_vector_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(Rf_allocVector(VECSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        size_t j = 0;
        auto second = it->second;
        auto p = second.size();
        SEXP values = PROTECT(Rf_allocVector(REALSXP, p));
        double* vp = REAL(values);  // It's a litte faster if you get a pointer and reuse it.
        for (auto vit = second.begin(); vit != second.end(); ++vit, ++j) {
            vp[j] = second[j];
        } 
        SET_VECTOR_ELT(list, i, values);
        SET_STRING_ELT(names, i, Rf_mkChar(it->first.c_str()));
        UNPROTECT(1);
    }
    Rf_setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP list_from_map(std::unordered_map<std::string, std::vector<std::string>> const &m)
{
	auto n = m.size();
    SEXP list =  PROTECT(Rf_allocVector(VECSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        size_t j = 0;
        auto second = it->second;
        auto p = second.size();
        SEXP values = PROTECT(Rf_allocVector(STRSXP, p));
        for (auto vit = second.begin(); vit != second.end(); ++vit, ++j) {
			SET_STRING_ELT(values, j, Rf_mkChar((second[j]).c_str()));
        } 
        SET_VECTOR_ELT(list, i, values);
        SET_STRING_ELT(names, i, Rf_mkChar(it->first.c_str()));
        UNPROTECT(1);
    }
    Rf_setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP vector_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP vector = PROTECT(Rf_allocVector(REALSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    double* vp = REAL(vector);  // It's a little faster if you get a pointer and reuse it.
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        vp[i] = it->second;
        SET_STRING_ELT(names, i, Rf_mkChar(it->first.c_str()));
    }
    Rf_setAttrib(vector, R_NamesSymbol, names);
    UNPROTECT(2);
    return vector;
}

SEXP r_logical_from_boolean(bool b)
{
    SEXP logical_output = PROTECT(Rf_allocVector(LGLSXP, 1));
    LOGICAL(logical_output)[0] = b;
    UNPROTECT(1);
    return logical_output;
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
    size_t n = Rf_length(list);
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    names = Rf_getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (size_t i = 0; i < n - 1; ++i) {
        Rprintf("%s, %0.04f; ", CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf("%s, %0.04f.\n\n", CHAR(STRING_ELT(names, n - 1)), REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

