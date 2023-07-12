#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error
#include "framework/R_helper_functions.h"  // for r_string_vector_from_vector
#include "framework/state_map.h"           // for string_vector
#include "R_framework_version.h"
#include "framework/framework_version.h"

using std::string;
using biocro_framework::version;

extern "C" {
/**
 *  @brief Returns the BioCro framework version number as a string.
 */
SEXP R_framework_version()
{
    try {
        string_vector result = {version};
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_framework_version: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_framework_version.");
    }
}
}
