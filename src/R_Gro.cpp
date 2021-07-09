#include <Rinternals.h>
#include <vector>
#include <string>
#include <exception>    // for std::exception
#include "state_map.h"  // for state_map, state_vector_map, string_vector
#include "dynamical_system.h"
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
    SEXP differential_module_names)
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
        string_vector differential_names = make_vector(differential_module_names);

        double t = REAL(time)[0];

        // Create a system
        dynamical_system sys(s, p, d, direct_names, differential_names);

        // Get the state in the correct format
        vector<double> x;
        sys.get_differential_quantities(x);

        // Get the state parameter names in the correct order
        string_vector state_param_names = sys.get_differential_quantity_names();

        // Make a vector to store the derivative
        vector<double> dxdt = x;

        // Run the system once
        sys.calculate_derivative(x, dxdt, t);

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

}  // extern "C"
