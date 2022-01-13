#include <string>
#include <exception>             // for std::exception
#include <Rinternals.h>          // for Rf_error
#include "state_map.h"           // for string_vector
#include "R_helper_functions.h"  // for r_string_vector_from_vector
#include "module_library/standard_module_library.h"

using std::string;

extern "C" {

SEXP R_get_all_modules()
{
    try {
        string_vector result =
            module_library<standard_module_library>::get_all_modules();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_modules: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_modules.");
    }
}

}  // extern "C"
