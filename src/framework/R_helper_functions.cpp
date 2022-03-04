#include <Rinternals.h>
#include <vector>
#include "R_helper_functions.h"

using std::string;
using std::vector;

/*
 * !!!!! IMPORTANT NOTE !!!!!:
 * This is a C++ interface to the C library in R. C++ supports exceptions, but C
 * does not. R's C library requires you to explicitly PROTECT() and UNPROTECT()
 * memory for garbage collection. If care is not taken, and an exception is
 * thrown after memory has been PROTECTed, but before it has been UNPROTECTed,
 * UNPROTECT will not be called.
 *
 * Every effort should be made to not use exception-throwing functions between
 * PROTECT and UNPROTECT pairs. If an exception-throwing function is used
 * between pairs, a means of ensuring that UNPROTECT is called when exceptions
 * are thrown is required.
 *
 * The ScopeGuard pattern could be used which would store protect_count
 * internally, and call UNPROTECT(protect_count) in the destructor. It would be
 * used as follows:
 *
 * guard = ProtectGuard();
 * PROTECT(something);
 * guard.increment_protect_count(1);
 *
 * ProtectGuard is an implementation of the ScopeGuard pattern. Its
 * increment_protect_count method should be be called immediately after each set
 * of PROTECT calls using the appropriate number of items to process. The
 * ProtectGuard destructor will be called when the function exists, regardless
 * of whether exceptions were thrown.
 *
 * For reference: the following methods are guaranteed to not throw exceptions:
 * - std::unordered_map::begin()
 * - std::unordered_map::end()
 * - std::unordered_map::size()
 * - std::unordered_map::operator[()
 * - std::vector::begin()
 * - std::vector::end()
 * - std::vector::size()
 * - std::vector::operator[()
 */

state_map map_from_list(SEXP const& list)
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

state_vector_map map_vector_from_list(SEXP const& list)
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
        m.insert(state_vector_map::value_type(
            CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}

string_vector make_vector(SEXP const& r_string_vector)
{
    string_vector v;
    size_t n = Rf_length(r_string_vector);
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        v.emplace_back(CHAR(STRING_ELT(r_string_vector, i)));
    }
    return v;
}

/**
 *  @brief Creates a std::vector of pointers to module_creator objects from
 *  an R vector of R external pointer objects
 *
 *  @param [in] list An R vector of R external pointer objects, typically
 *              created by a call to `R_module_creators()`
 *
 *  @return A std::vector of pointers to module_creator objects
 */
mc_vector mc_vector_from_list(SEXP const& list)
{
    size_t n = Rf_length(list);
    mc_vector v(n);
    for (size_t i = 0; i < n; ++i) {
        v[i] = static_cast<module_creator*>(
            R_ExternalPtrAddr(VECTOR_ELT(list, i)));
    }
    return v;
}

SEXP r_string_vector_from_vector(string_vector const& v)
{
    auto n = v.size();
    SEXP r_string_vector = PROTECT(Rf_allocVector(STRSXP, n));
    for (size_t i = 0; i < n; ++i) {
        SET_STRING_ELT(r_string_vector, i, Rf_mkChar(v[i].c_str()));
    }
    UNPROTECT(1);
    return r_string_vector;
}

SEXP list_from_map(state_map const& m)
{
    auto n = m.size();
    SEXP list = PROTECT(Rf_allocVector(VECSXP, n));
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

SEXP list_from_map(state_vector_map const& m)
{
    auto n = m.size();
    SEXP list = PROTECT(Rf_allocVector(VECSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    size_t i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        size_t j = 0;
        auto second = it->second;
        auto p = second.size();
        SEXP values = PROTECT(Rf_allocVector(REALSXP, p));
        double* vp = REAL(values);  // It's a litte faster to reuse a pointer
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

SEXP list_from_map(std::unordered_map<string, string_vector> const& m)
{
    auto n = m.size();
    SEXP list = PROTECT(Rf_allocVector(VECSXP, n));
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

SEXP list_from_module_info(
    std::string const& module_name,
    string_vector const& module_inputs,
    string_vector const& module_outputs,
    bool const& is_differential,
    bool const& requires_euler_ode_solver,
    std::string const& creation_error_message)
{
    // Module inputs and outputs (these do not require an UNPROTECT here)
    SEXP input_vec = PROTECT(r_string_vector_from_vector(module_inputs));
    SEXP output_vec = PROTECT(r_string_vector_from_vector(module_outputs));

    // Module name
    SEXP name = PROTECT(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(name, 0, Rf_mkChar(module_name.c_str()));

    // Module type
    SEXP type = PROTECT(Rf_allocVector(STRSXP, 1));
    if (is_differential) {
        SET_STRING_ELT(type, 0, Rf_mkChar("differential"));
    } else {
        SET_STRING_ELT(type, 0, Rf_mkChar("direct"));
    }

    // Euler requirement
    SEXP requires_euler = PROTECT(Rf_allocVector(STRSXP, 1));
    string euler_msg;
    if (requires_euler_ode_solver) {
        euler_msg = "requires a fixed-step Euler ode_solver";
    } else {
        euler_msg = "does not require a fixed-step Euler ode_solver";
    }
    SET_STRING_ELT(requires_euler, 0, Rf_mkChar(euler_msg.c_str()));

    // Error message from module creation
    SEXP error_message = PROTECT(Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(error_message, 0, Rf_mkChar(creation_error_message.c_str()));

    // The module info list
    SEXP info_names = PROTECT(Rf_allocVector(STRSXP, 6));
    SET_STRING_ELT(info_names, 0, Rf_mkChar("module_name"));
    SET_STRING_ELT(info_names, 1, Rf_mkChar("inputs"));
    SET_STRING_ELT(info_names, 2, Rf_mkChar("outputs"));
    SET_STRING_ELT(info_names, 3, Rf_mkChar("type"));
    SET_STRING_ELT(info_names, 4, Rf_mkChar("euler_requirement"));
    SET_STRING_ELT(info_names, 5, Rf_mkChar("creation_error_message"));

    SEXP info_list = PROTECT(Rf_allocVector(VECSXP, 6));
    SET_VECTOR_ELT(info_list, 0, name);
    SET_VECTOR_ELT(info_list, 1, input_vec);
    SET_VECTOR_ELT(info_list, 2, output_vec);
    SET_VECTOR_ELT(info_list, 3, type);
    SET_VECTOR_ELT(info_list, 4, requires_euler);
    SET_VECTOR_ELT(info_list, 5, error_message);

    Rf_setAttrib(info_list, R_NamesSymbol, info_names);

    UNPROTECT(8);

    return info_list;
}

SEXP vector_from_map(state_map const& m)
{
    auto n = m.size();
    SEXP vector = PROTECT(Rf_allocVector(REALSXP, n));
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    double* vp = REAL(vector);  // It's a little faster to reuse a pointer
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
    *LOGICAL(logical_output) = b;
    UNPROTECT(1);
    return logical_output;
}

void output_map(state_map const& m)
{
    if (!m.empty()) {
        size_t i = 0;
        auto it = m.begin();
        Rprintf("The map contains the following items: ");
        for (; std::next(it) != m.end(); ++it) {
            Rprintf("%s, %0.04f; ", it->first.c_str(), it->second);
            if (++i % 5 == 0) Rprintf("\n");
        }
        Rprintf("%s, %0.04f.\n\n", it->first.c_str(), it->second);
    } else {
        Rprintf("The map is empty.\n");
    }
}

void output_list(SEXP const& list)
{
    size_t n = Rf_length(list);
    SEXP names = PROTECT(Rf_allocVector(STRSXP, n));
    names = Rf_getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (size_t i = 0; i < n - 1; ++i) {
        Rprintf(
            "%s, %0.04f; ",
            CHAR(STRING_ELT(names, i)),
            REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf(
        "%s, %0.04f.\n\n",
        CHAR(STRING_ELT(names, n - 1)),
        REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

/**
 *  @brief Deletes a `module_creator` object that is pointed to by an "R
 *  external pointer" object.
 *
 *  See http://www.hep.by/gnu/r-patched/r-exts/R-exts_122.html for more details.
 *
 *  @param [in] m_ptr an "R external pointer" object that points to a
 *              module_creator object
 */
void finalize_module_creator(SEXP mw_ptr)
{
    module_creator* w =
        static_cast<module_creator*>(R_ExternalPtrAddr(mw_ptr));

    delete w;
}
