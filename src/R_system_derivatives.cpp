#include <vector>
#include <string>
#include <exception>                       // for std::exception
#include <Rinternals.h>                    // for Rf_error
#include "framework/R_helper_functions.h"  // for map_from_list, map_vector_from_list, mc_vector_from_list, list_from_map
#include "framework/state_map.h"           // for state_map, state_vector_map, string_vector
#include "framework/dynamical_system.h"

using std::string;
using std::vector;

extern "C" {

/**
 *  @brief Creates a `dynamical_system` object from the differential quantities,
 *         parameters, drivers, and modules, and then uses the system object to
 *         determine the derivatives of the differential quantities at the
 *         specified time
 *
 *  @param [in] differential_quantities An R list of named elements
 *              representing the current values of the differential quantities
 *
 *  @param [in] time An R numeric vector with one element specifying the time
 *              index
 *
 *  @param [in] parameters An R list of named elements representing the
 *              parameters
 *
 *  @param [in] drivers An R data frame representing the time series of the
 *              drivers
 *
 *  @param [in] direct_mc_vec An R vector of pointers to module wrapper objects
 *              representing the direct modules
 *
 *  @param [in] differential_module_names An R vector of pointers to module
 *              wrapper objects representing the differential modules
 *
 *  @return An R list of named elements representing the derivatives of the
 *          differential quantities
 */
SEXP R_system_derivatives(
    SEXP differential_quantities,
    SEXP time,
    SEXP parameters,
    SEXP drivers,
    SEXP direct_mc_vec,
    SEXP differential_mc_vec)
{
    try {
        // Convert the inputs into the proper format
        state_map iv = map_from_list(differential_quantities);
        state_map p = map_from_list(parameters);
        state_vector_map d = map_vector_from_list(drivers);

        if (d.begin()->second.size() == 0) {
            return R_NilValue;
        }

        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        mc_vector differential_mcs = mc_vector_from_list(differential_mc_vec);

        double t = REAL(time)[0];

        // Create a dynamical system
        dynamical_system sys(iv, p, d, direct_mcs, differential_mcs);

        // Ask the system object for a vector of the current values of the
        // differential quantities. The system hasn't been modified since its
        // creation, so these will be the values from the input argument
        // `differential_quantities`. This step is necessary to ensure they are
        // arranged in the same order as expected by the system, since it may
        // reorder them.
        vector<double> x;
        sys.get_differential_quantities(x);

        // Get the differential quantity names in the order used by the system,
        // which may in general be different than the original order of the
        // quantities in the `differential_quantities` input.
        string_vector differential_quantity_names =
            sys.get_differential_quantity_names();

        // Make a vector to store the values of the derivatives of the
        // differential quantities
        vector<double> dxdt(x.size());

        // Calculate the derivative (sets dxdt)
        sys.calculate_derivative(x, dxdt, t);

        // Make the output map
        state_map result;
        for (size_t i = 0; i < differential_quantity_names.size(); i++) {
            result[differential_quantity_names[i]] = dxdt[i];
        }

        // Return the resulting derivatives
        return list_from_map(result);

    } catch (std::exception const& e) {
        Rf_error("%s", (string("Caught exception in R_system_derivatives: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_system_derivatives.");
    }
}

}  // extern "C"
