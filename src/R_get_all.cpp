#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "module_library/module_wrapper_factory.h"
#include "numerical_integrator_library/numerical_integrator_factory.h"
#include "se_solver_library/se_solver_factory.h"

extern "C" {

SEXP R_get_all_modules()
{
    try {
        std::vector<std::string> result = module_wrapper_factory::get_modules();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_all_modules: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_modules.");
    }
}

SEXP R_get_all_quantities()
{
    try {
        std::unordered_map<std::string, std::vector<std::string>> all_quantities = module_wrapper_factory::get_all_quantities();
        return list_from_map(all_quantities);
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_all_param: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_param.");
    }
}

SEXP R_get_all_numerical_integrators()
{
    try {
        std::vector<std::string> result = numerical_integrator_factory::get_numerical_integrators();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_all_numerical_integrators: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_numerical_integrators.");
    }
}

SEXP R_get_all_se_solvers()
{
    try {
        std::vector<std::string> result = se_solver_factory::get_solvers();
        return r_string_vector_from_vector(result);
    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_all_se_solvers: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_all_se_solvers.");
    }
}

}  // extern "C"
