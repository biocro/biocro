#include <string>
#include <unordered_map>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error
#include "framework/state_map.h"           // for string_vector
#include "framework/R_helper_functions.h"  // for make_vector, r_string_vector_from_vector, list_from_map
#include "framework/module_creator.h"      // for module_creator
#include "framework/module_factory.h"
#include "module_library/module_library.h"
#include "R_module_library.h"

// When creating a new module library R package, it will be necessary to modify
// the namespace in this file to match the one defined in
// `module_library/module_library.h`. See that file for more details. None of
// the other code in this file should require any modifications.

using std::string;
using library = standardBML::module_library;

extern "C" {

/**
 *  @brief Creates a vector of "R external pointer" objects that each point to a
 *  `module_creator` object; this function provides a route for creating C++
 *  objects from R strings
 *
 *  See http://www.hep.by/gnu/r-patched/r-exts/R-exts_122.html for more details
 *  about R external pointers.
 *
 *  When creating an R external pointer using `R_MakeExternalPtr()`, it is
 *  possible to specify `tag` and `prot` arguments for identifying the pointer
 *  and protecting its memory. However, it is common to bypass both of these
 *  arguments by setting them to `R_NilValue` (the R null pointer). We follow
 *  that route here.
 *
 *  Once an R external pointer has been created, it is also possible to specify
 *  a "finalizer" function via `R_RegisterFinalizerEx()`. This function will be
 *  called when garbage is collected and should clear the pointer. By setting
 *  the `onexit` argument of `R_RegisterFinalizerEx()` to be TRUE, we ensure
 *  that the pointer will be cleared when its R session closes.
 *
 *  After its creation, the R external pointer takes ownership of the
 *  module_creator object, so there is no need to use smart pointers here.
 *
 *  The output from this function can be converted into a std::vector of
 *  `module_creator pointers` (in other words, an `mc_vector`) for use in C/C++
 *  code using the `mc_vector_from_list()` function.
 *
 *  @param [in] module_names The names of the modules for which
 *              `module_creator` objects should be created
 *
 *  @return A vector of "R external pointer" objects
 */
SEXP R_module_creators(SEXP module_names)
{
    try {
        string_vector names = make_vector(module_names);
        size_t n = names.size();
        SEXP mw_ptr_vec = PROTECT(Rf_allocVector(VECSXP, n));

        for (size_t i = 0; i < n; ++i) {
            module_creator* w =
                module_factory<library>::retrieve(names[i]);

            SEXP mw_ptr =
                PROTECT(R_MakeExternalPtr(w, R_NilValue, R_NilValue));

            R_RegisterCFinalizerEx(
                mw_ptr,
                (R_CFinalizer_t)finalize_module_creator,
                TRUE);

            SET_VECTOR_ELT(mw_ptr_vec, i, mw_ptr);
            UNPROTECT(1);  // UNPROTECT mw_ptr
        }

        UNPROTECT(1);  // UNPROTECT mw_ptr_vec
        return mw_ptr_vec;

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_module_creators: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_module_creators.");
    }
}

/**
 *  @brief A wrapper for `module_factory::get_all_modules()` that
 *  returns the output as an R vector of R strings.
 */
SEXP R_get_all_modules()
{
    try {
        string_vector result =
            module_factory<library>::get_all_modules();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_modules: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_modules.");
    }
}

/**
 *  @brief A wrapper for `module_factory::get_all_quantities()` that
 *  returns the output as an R list of named R strings.
 */
SEXP R_get_all_quantities()
{
    try {
        std::unordered_map<string, string_vector> all_quantities =
            module_factory<library>::get_all_quantities();
        return list_from_map(all_quantities);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_quantities: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_quantities.");
    }
}
}
