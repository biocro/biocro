#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "state_map.h"  // for string_vector
#include "R_helper_functions.h"
#include "module_library/standard_module_library.h"
#include "ode_solver_library/ode_solver_factory.h"
#include "se_solver_library/se_solver_factory.h"

using std::string;
using std::vector;

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

SEXP R_get_all_ode_solvers()
{
    try {
        string_vector result = ode_solver_factory::get_ode_solvers();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_ode_solvers: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_ode_solvers.");
    }
}

SEXP R_get_all_se_solvers()
{
    try {
        string_vector result = se_solver_factory::get_solvers();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((string("Caught exception in R_get_all_se_solvers: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_se_solvers.");
    }
}

}  // extern "C"
