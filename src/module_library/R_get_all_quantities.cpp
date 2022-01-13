#include <string>
#include <unordered_map>
#include <exception>                // for std::exception
#include <Rinternals.h>             // for Rf_error
#include "../state_map.h"           // for string_vector
#include "../R_helper_functions.h"  // for list_from_map
#include "standard_module_library.h"

using std::string;

extern "C" {

SEXP R_get_all_quantities()
{
    try {
        std::unordered_map<string, string_vector> all_quantities =
            module_library<standard_module_library>::get_all_quantities();
        return list_from_map(all_quantities);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_param: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_param.");
    }
}

}  // extern "C"
