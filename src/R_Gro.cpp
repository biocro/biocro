#include <Rinternals.h>
#include <vector>
#include <string>
#include <exception>    // for std::exception
#include "state_map.h"  // for state_map, state_vector_map, string_vector
#include "system.h"
#include "R_helper_functions.h"

using std::string;
using std::vector;

extern "C" {

SEXP R_Gro_deriv(
    SEXP state,
    SEXP time,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_module_names,
    SEXP derivative_module_names)
{
    try {
        // Convert the inputs into the proper format
        state_map s = map_from_list(state);
        state_map p = map_from_list(parameters);
        state_vector_map d = map_vector_from_list(drivers);

        if (d.begin()->second.size() == 0) {
            return R_NilValue;
        }

        string_vector direct_names = make_vector(direct_module_names);
        string_vector deriv_names = make_vector(derivative_module_names);

        double t = REAL(time)[0];

        // Create a system
        System sys(s, p, d, direct_names, deriv_names);

        // Get the state in the correct format
        vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        string_vector state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, t);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) {
            result[state_param_names[i]] = dxdt[i];
        }

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
    SEXP direct_module_names,
    SEXP derivative_module_names)
{
    try {
        // Get the input state
        state_map s = map_from_list(state);

        // There are a few special parameters that a system requires at startup,
        //  so make sure they are properly defined

        // Form the list of parameters, making sure it includes the timestep
        state_map p;
        if (s.find("timestep") == s.end()) {
            // The timestep is not defined in the input state, so assume it is 1
            p["timestep"] = 1.0;
        } else {
            // The timestep is defined in the input state, so copy its value into the parameters
            //  and remove it from the state
            p["timestep"] = s["timestep"];
            s.erase("timestep");
        }

        // Form the list of drivers, making sure it includes doy and hour
        state_vector_map d;
        if (s.find("doy") == s.end()) {
            // The doy is not defined in the input state, so assume it is 0
            vector<double> temp_vec;
            temp_vec.push_back(0.0);
            d["doy"] = temp_vec;
        } else {
            // The doy is defined in the input state, so copy its value into the drivers
            vector<double> temp_vec;
            temp_vec.push_back(s["doy"]);
            d["doy"] = temp_vec;
            s.erase("doy");
        }
        if (s.find("hour") == s.end()) {
            // The hour is not defined in the input state, so assume it is 0
            vector<double> temp_vec;
            temp_vec.push_back(0.0);
            d["hour"] = temp_vec;
        } else {
            // The hour is defined in the input state, so copy its value into the drivers
            vector<double> temp_vec;
            temp_vec.push_back(s["hour"]);
            d["hour"] = temp_vec;
            s.erase("hour");
        }

        // Get the module names
        string_vector direct_names = make_vector(direct_module_names);
        string_vector deriv_names = make_vector(derivative_module_names);

        // Make the system
        System sys(s, p, d, direct_names, deriv_names);

        // Get the current state in the correct format
        vector<double> x;
        sys.get_state(x);

        // Get the state parameter names in the correct order
        string_vector state_param_names = sys.get_state_parameter_names();

        // Make a vector to store the derivative
        vector<double> dxdt = x;

        // Run the system once
        sys(x, dxdt, 0);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < state_param_names.size(); i++) {
            result[state_param_names[i]] = dxdt[i];
        }

        // Return the resulting derivatives
        return list_from_map(result);

    } catch (std::exception const& e) {
        Rf_error(string(string("Caught exception in R_Gro_ode: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_Gro_ode.");
    }
}

}  // extern "C"
