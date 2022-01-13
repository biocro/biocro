#include <string>
#include <Rinternals.h>          // for Rf_error
#include "state_map.h"           // for string_vector
#include "R_helper_functions.h"  // for make_vector
#include "module_library/standard_module_library.h"
#include "module_creator.h"

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
                module_library<standard_module_library>::retrieve(names[i]);

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
}
