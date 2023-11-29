#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error
#include "framework/R_helper_functions.h"  // for r_string_vector_from_vector
#include "framework/state_map.h"           // for string_vector
#include "framework/ode_solver_library/ode_solver_factory.h"
#include "R_get_all_ode_solvers.h"

using std::string;

extern "C" {

SEXP R_get_all_ode_solvers()
{
    try {
        string_vector result = ode_solver_factory::get_ode_solvers();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error("%s", (string("Caught exception in R_get_all_ode_solvers: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_ode_solvers.");
    }
}

}  // extern "C"
