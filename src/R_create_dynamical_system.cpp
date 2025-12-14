#include <string>
#include <exception>  // for std::exception
#include <R.h>
#include <Rinternals.h>                    // for Rf_error and Rprintf
#include "framework/R_helper_functions.h"  // for map_from_list, map_vector_from_list, mc_vector_from_list, list_from_map
#include "framework/state_map.h"           // for state_map, state_vector_map, string_vector
#include "framework/module_creator.h"      // for mc_vector
#include "framework/dynamical_system.h"
#include "R_create_dynamical_system.h"

using std::string;

extern "C" {

void finalize_dynamical_system_handle(SEXP handle)
{
    dynamical_system* ptr = static_cast<dynamical_system*>(R_ExternalPtrAddr(handle));
    if (NULL != ptr) {
        delete ptr;
        R_ClearExternalPtr(handle);
    }
}

SEXP R_create_dynamical_system(
    SEXP initial_values,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec)
{
    dynamical_system* pointer = R_Calloc(1, dynamical_system);
    SEXP handle = PROTECT(R_MakeExternalPtr(pointer, R_NilValue, R_NilValue));
    try {
        R_RegisterCFinalizerEx(handle, finalize_dynamical_system_handle, TRUE);
        state_map iv = map_from_list(initial_values);
        state_map p = map_from_list(parameters);
        state_vector_map d = map_vector_from_list(drivers);

        // if (d.begin()->second.size() == 0) {
        //     return R_NilValue;
        // }

        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        mc_vector differential_mcs = mc_vector_from_list(differential_mc_vec);
        new (pointer) dynamical_system(iv, p, d, direct_mcs, differential_mcs);

    } catch (std::exception const& e) {
        delete pointer;
        R_ClearExternalPtr(handle);
        Rf_error("%s", string(string("Caught exception in R_run_biocro: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_run_biocro.");
    }
    UNPROTECT(1);
    return handle;
}

SEXP R_get_system_modules(const SEXP handle)
{
    dynamical_system* pointer = static_cast<dynamical_system*>(R_ExternalPtrAddr(handle));
    if (NULL != pointer) {
        SEXP dirs = r_string_vector_from_vector(pointer->get_direct_module_names());
        SEXP difs = r_string_vector_from_vector(pointer->get_differential_module_names());

        SEXP list = PROTECT(Rf_allocVector(VECSXP, 2));
        SEXP names = PROTECT(Rf_allocVector(STRSXP, 2));

        SET_VECTOR_ELT(list, 0, dirs);
        SET_STRING_ELT(names, 0, Rf_mkChar("direct_modules"));
        SET_VECTOR_ELT(list, 1, difs);
        SET_STRING_ELT(names, 1, Rf_mkChar("differential_modules"));
        Rf_setAttrib(list, R_NamesSymbol, names);
        UNPROTECT(2);
        return list;
    }
    return R_NilValue;
}

SEXP R_calculate_derivative(SEXP handle, SEXP time, SEXP state)
{
    dynamical_system* pointer = static_cast<dynamical_system*>(R_ExternalPtrAddr(handle));
    if (NULL != pointer) {
        // R to C++
        double t = REAL(time)[0];
        // calculate vector
        std::vector<double> x;
        pointer->get_differential_quantities(x);
        for (size_t i = 0; i < x.size(); ++i)
            x[i] = REAL(state)[i];

        std::vector<double> dxdt(x.size());
        pointer->calculate_derivative(x, dxdt, t);
        // C++ to R
        return r_vector_from_state_vector(dxdt);
    }
    return R_NilValue;
}

SEXP R_get_differential_quantities(const SEXP handle)
{
    dynamical_system* pointer = static_cast<dynamical_system*>(R_ExternalPtrAddr(handle));
    if (NULL != pointer) {
        state_map x = pointer->get_initial_values();

        return vector_from_map(x);
    }
    return R_NilValue;
}

}  // extern "C"
