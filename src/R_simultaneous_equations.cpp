#include <Rinternals.h>  // for Rf_error and Rprintf
#include <vector>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include "framework/R_helper_functions.h"
#include "framework/state_map.h"
#include "framework/numerical_jacobian.h"
#include "framework_ed/simultaneous_equations.h"
#include "framework_ed/se_solver.h"
#include "framework_ed/se_solver_library/se_solver_factory.h"

extern "C" {

SEXP R_solve_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP lower_bounds,
    SEXP upper_bounds,
    SEXP abs_error_tols,
    SEXP rel_error_tols,
    SEXP direct_mc_vec,
    SEXP solver_type,
    SEXP max_it,
    SEXP silent)
{
    try {
        // Convert format
        const state_map kq = map_from_list(known_quantities);
        const state_map uq = map_from_list(unknown_quantities);
        const state_map lb = map_from_list(lower_bounds);
        const state_map ub = map_from_list(upper_bounds);
        const state_map abs_t = map_from_list(abs_error_tols);
        const state_map rel_t = map_from_list(rel_error_tols);
        const mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        const std::string solver_type_string = CHAR(STRING_ELT(solver_type, 0));
        const int max_iterations = REAL(max_it)[0];
        const bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];

        // Split uq into two vectors
        const std::vector<std::string> uq_names = keys(uq);
        std::vector<double> uq_values(uq_names.size());
        for (size_t i = 0; i < uq_names.size(); i++) {
            uq_values[i] = uq.at(uq_names[i]);
        }
        std::vector<double> uq_final = uq_values;

        // Get the upper and lower bounds, along with
        // the error tolerances
        std::vector<double> lb_vector;
        std::vector<double> ub_vector;
        std::vector<double> abs_t_vector;
        std::vector<double> rel_t_vector;
        for (std::string const& name : uq_names) {
            lb_vector.push_back(lb.at(name));
            ub_vector.push_back(ub.at(name));
            abs_t_vector.push_back(abs_t.at(name));
            rel_t_vector.push_back(rel_t.at(name));
        }

        // Make a vector to store the sequence of guesses
        std::vector<std::vector<double>> guess_vector;

        // Solve
        std::unique_ptr<simultaneous_equations> se(new simultaneous_equations(kq,
                                                                              uq_names,
                                                                              direct_mcs));

        std::unique_ptr<se_solver> solver(se_solver_factory::create(solver_type_string, max_iterations));

        bool success = solver->solve(se, uq_values,
                                     lb_vector, ub_vector,
                                     abs_t_vector, rel_t_vector,
                                     uq_final,
                                     se_observer_push_back(guess_vector));

        // Print info if desired
        if (!be_quiet) {
            std::string message = std::string("\n\nThe simultaneous_equations object reports the following upon construction:\n");
            message += se->generate_startup_report();

            message += std::string("\nThe se_solver reports the following upon construction:\n");
            message += solver->generate_info_report();

            message += std::string("\n\nThe se_solver reports the following after solving:\n");
            message += solver->generate_solve_report();

            message += std::string("\n\nThe simultaneous_equations object reports the following after solving:\n");
            message += se->generate_usage_report();

            message += std::string("\n\n");

            Rprintf(message.c_str());
        }

        // Report a warning if the method was not successful
        if (!success) {
            Rf_warning(std::string("A solution was not successfully found!").c_str());
        }

        return list_from_map(format_se_solver_results(uq_names, guess_vector));

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_solve_simultaneous_equations: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_solve_simultaneous_equations.");
    }
}

SEXP R_validate_simultaneous_equations(
    SEXP known_quantities,
    SEXP unknown_quantities,
    SEXP direct_mc_vec,
    SEXP silent)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        std::vector<std::string> uq = make_vector(unknown_quantities);
        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);
        bool be_quiet = LOGICAL(VECTOR_ELT(silent, 0))[0];

        std::string msg;
        bool valid = validate_simultaneous_equations_inputs(msg, kq, uq, direct_mcs);

        if (!be_quiet) {
            Rprintf("\nChecking the validity of the simultaneous_equations inputs:\n");

            Rprintf(msg.c_str());

            if (valid) {
                Rprintf("\nSimultaneous_equations inputs are valid\n");
            } else {
                Rprintf("\nSimultaneous_equations inputs are not valid\n");
            }

            Rprintf("\nPrinting additional information about the simultaneous_equations inputs:\n");

            msg = analyze_simultaneous_equations_inputs(direct_mcs);
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
    SEXP direct_mc_vec)
{
    try {
        // Convert format
        state_map kq = map_from_list(known_quantities);
        state_map uq = map_from_list(unknown_quantities);
        mc_vector direct_mcs = mc_vector_from_list(direct_mc_vec);

        // Split uq into two vectors
        std::vector<std::string> uq_names = keys(uq);
        std::vector<double> uq_values(uq_names.size());
        for (size_t i = 0; i < uq_names.size(); i++) {
            uq_values[i] = uq[uq_names[i]];
        }

        std::unique_ptr<simultaneous_equations> se(new simultaneous_equations(kq, uq_names, direct_mcs));

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
