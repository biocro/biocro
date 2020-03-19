#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include "R_helper_functions.h"
#include "state_map.h"
#include "simultaneous_equations.h"
#include "numerical_jacobian.h"

extern "C" {

SEXP R_validate_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP steady_state_module_names,
    SEXP silent)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        std::vector<std::string> uq = make_vector(unknown_quantities);
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);
        bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];

        std::string msg;
        bool valid = validate_simultaneous_equations_inputs(msg, kq, uq, ss_names);

        if (!be_quiet) {
            Rprintf("\nChecking the validity of the simultaneous_equations inputs:\n");

            Rprintf(msg.c_str());

            if (valid) {
                Rprintf("\nSimultaneous_equations inputs are valid\n");
            } else {
                Rprintf("\nSimultaneous_equations inputs are not valid\n");
            }

            Rprintf("\nPrinting additional information about the simultaneous_equations inputs:\n");

            msg = analyze_simultaneous_equations_inputs(ss_names);
            Rprintf(msg.c_str());

            // Print a space to improve readability
            Rprintf("\n");
        }

        return r_logical_from_boolean(valid);

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_validate_simultaneous_equations: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_validate_simultaneous_equations.");
    }
}

SEXP R_test_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP steady_state_module_names)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        state_map uq = map_from_list(unknown_quantities);
        std::vector<std::string> ss_names = make_vector(steady_state_module_names);

        // Split uq into two vectors
        std::vector<std::string> uq_names = keys(uq);
        std::vector<double> uq_values(uq_names.size());
        for (size_t i = 0; i < uq_names.size(); i++) {
            uq_values[i] = uq[uq_names[i]];
        }

        std::shared_ptr<simultaneous_equations> se(new simultaneous_equations(kq, uq_names, ss_names));

        // Calculate a Jacobian matrix and print its values to the R console
        boost::numeric::ublas::matrix<double> jacobian(uq.size(), uq.size());
        calculate_jacobian(se, uq_values, jacobian);

        std::string msg = std::string("\n\n\nJacobian matrix:\n\n\n");
        for (size_t i = 0; i < jacobian.size1(); ++i) {
            msg += std::string("\n");
            for (size_t j = 0; j < jacobian.size2(); ++j) {
                msg += std::to_string(jacobian(i, j)) + std::string(" ");
            }
        }
        msg += std::string("\n\n\n");
        Rprintf(msg.c_str());

        // Calculate a difference vector
        std::vector<double> output_vector(uq.size());
        se->operator()(uq_values, output_vector);

        // Return the difference vector in a nice format
        state_map result;
        for (size_t i = 0; i < uq.size(); i++) {
            result[uq_names[i]] = output_vector[i];
        }

        return list_from_map(result);

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_test_simultaneous_equations: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_simultaneous_equations.");
    }
}

}  // extern "C"