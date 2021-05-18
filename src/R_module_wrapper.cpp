#include <string>
#include <Rinternals.h>          // for Rprintf
#include <memory>                // for std::unique_ptr
#include "R_helper_functions.h"  // for make_vector
#include "module_library/module_wrapper_factory.h"
#include "module_wrapper.h"

/**
 *  @brief Deletes a module_wrapper_base object that is pointed to by an "R
 *  external pointer" object.
 *
 *  See http://www.hep.by/gnu/r-patched/r-exts/R-exts_122.html for more details.
 *
 *  @param [in] m_ptr an "R external pointer" object that points to a
 *                    module_wrapper_base object
 */
void finalize_module_wrapper_pointer(SEXP mw_ptr)
{
    module_wrapper_base* w =
        static_cast<module_wrapper_base*>(R_ExternalPtrAddr(mw_ptr));

    delete w;
}

extern "C" {

/**
 *  @brief Creates an "R external pointer" object that points to a
 *  module_wrapper_base object.
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
 *  module_wrapper_base object (in the sense that it will ensure that the
 *  module_wrapper_base object is deleted when the pointer is destroyed), so it
 *  is critical for the std::unique_ptr to relinquish control via
 *  `std::unique_ptr::release()` rather than `std::unique_ptr::get()`. Memory
 *  access problems will occur otherwise, causing R to suddenly crash.
 *
 *  @param [in] module_name_input The name of the module for which a
 *                                module_wrapper_base object should be created
 *
 *  @return An "R external pointer" object
 */
SEXP R_module_wrapper_pointer(SEXP module_name_input)
{
    try {
        std::string module_name = make_vector(module_name_input)[0];

        std::unique_ptr<module_wrapper_base> mw =
            module_wrapper_factory::create(module_name);

        SEXP mw_ptr =
            PROTECT(R_MakeExternalPtr(mw.release(), R_NilValue, R_NilValue));

        R_RegisterCFinalizerEx(
            mw_ptr,
            (R_CFinalizer_t)finalize_module_wrapper_pointer,
            TRUE);

        UNPROTECT(1);

        return mw_ptr;
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_module_wrapper_pointer: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_module_wrapper_pointer.");
    }
}
}
