#include <stdexcept>
#include <sstream>
#include <Rinternals.h>
#include <cmath>  // For isnan
#include "module_library/BioCro.h"
#include "modules.h"
#include "biocro_simulation.h"
#include "module_library/module_wrapper_factory.h"
#include "R_helper_functions.h"
#include "standalone_ss.h"
#include "system_solver_library/system_solver_factory.h"
#include "validate_system.h"
#include "numerical_jacobian.h"

using std::string;
using std::unique_ptr;
using std::vector;

extern "C" {

SEXP R_Gro_solver(
    SEXP initial_state,
    SEXP parameters,
    SEXP varying_parameters,
    SEXP steady_state_module_names,
    SEXP derivative_module_names,
    SEXP solver_type,
    SEXP solver_output_step_size,
    SEXP solver_adaptive_rel_error_tol,
    SEXP solver_adaptive_abs_error_tol,
    SEXP solver_adaptive_max_steps,
    SEXP verbose)
{
    try {
        state_map s = map_from_list(initial_state);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(varying_parameters);

        if (vp.begin()->second.size() == 0) {
            return R_NilValue;
        }

        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);

        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];
        string solver_type_string = CHAR(STRING_ELT(solver_type, 0));
        double output_step_size = REAL(solver_output_step_size)[0];
        double adaptive_rel_error_tol = REAL(solver_adaptive_rel_error_tol)[0];
        double adaptive_abs_error_tol = REAL(solver_adaptive_abs_error_tol)[0];
        int adaptive_max_steps = (int)REAL(solver_adaptive_max_steps)[0];

        biocro_simulation gro(s, ip, vp, ss_names, deriv_names,
                              solver_type_string, output_step_size,
                              adaptive_rel_error_tol, adaptive_abs_error_tol,
                              adaptive_max_steps);
        state_vector_map result = gro.run_simulation();

        if (loquacious) {
            Rprintf(gro.generate_report().c_str());
        }

        return list_from_map(result);
    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_solver: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_solver.");
    }
}

SEXP R_Gro_deriv(
    SEXP state,
    SEXP time,
    SEXP parameters,
    SEXP varying_parameters,
    SEXP steady_state_module_names,
    SEXP derivative_module_names)
{
    try {
        // Convert the inputs into the proper format
        state_map s = map_from_list(state);
        state_map ip = map_from_list(parameters);
        state_vector_map vp = map_vector_from_list(varying_parameters);

        if (vp.begin()->second.size() == 0) {
            return R_NilValue;
        }

        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);

        double t = REAL(time)[0];

        // Create a system
        System sys(s, ip, vp, ss_names, deriv_names);

        // Get the state in the correct format
        std::vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        std::vector<std::string> state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        std::vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, t);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) result[state_param_names[i]] = dxdt[i];

        // Return the resulting derivatives
        return list_from_map(result);

    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_deriv: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_deriv.");
    }
}

SEXP R_Gro_ode(
    SEXP state,
    SEXP steady_state_module_names,
    SEXP derivative_module_names)
{
    try {
        // Get the input state
        state_map s = map_from_list(state);

        // There are a few special parameters that a system requires at startup,
        //  so make sure they are properly defined

        // Form the list of invariant parameters, making sure it includes the timestep
        state_map ip;
        if (s.find("timestep") == s.end()) {
            // The timestep is not defined in the input state, so assume it is 1
            ip["timestep"] = 1.0;
        } else {
            // The timestep is defined in the input state, so copy its value into the invariant parameters
            //  and remove it from the state
            ip["timestep"] = s["timestep"];
            s.erase("timestep");
        }

        // Form the list of varying parameters, making sure it includes doy and hour
        state_vector_map vp;
        if (s.find("doy") == s.end()) {
            // The doy is not defined in the input state, so assume it is 0
            std::vector<double> temp_vec;
            temp_vec.push_back(0.0);
            vp["doy"] = temp_vec;
        } else {
            // The doy is defined in the input state, so copy its value into the varying parameters
            std::vector<double> temp_vec;
            temp_vec.push_back(s["doy"]);
            vp["doy"] = temp_vec;
            s.erase("doy");
        }
        if (s.find("hour") == s.end()) {
            // The hour is not defined in the input state, so assume it is 0
            std::vector<double> temp_vec;
            temp_vec.push_back(0.0);
            vp["hour"] = temp_vec;
        } else {
            // The hour is defined in the input state, so copy its value into the varying parameters
            std::vector<double> temp_vec;
            temp_vec.push_back(s["hour"]);
            vp["hour"] = temp_vec;
            s.erase("hour");
        }

        // Get the module names
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        std::vector<std::string> deriv_names = make_vector(derivative_module_names);

        // Make the system
        System sys(s, ip, vp, ss_names, deriv_names);

        // Get the current state in the correct format
        std::vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        std::vector<std::string> state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        std::vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, 0);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) result[state_param_names[i]] = dxdt[i];

        // Return the resulting derivatives
        return list_from_map(result);

    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_ode: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_ode.");
    }
}

}  // extern "C"
