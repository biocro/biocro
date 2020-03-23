#include <map>
#include "se_solver.h"
#include "se_solver_helper_functions.h"

/**
 * @brief Tries to find a root, starting at the initial guess and staying within the bounds.
 * 
 * @param[in] se
 * 
 * @param[in] initial_guess_for_root
 * 
 * @param[in] lower_bounds
 * 
 * @param[in] upper_bounds
 * 
 * @param[out] final_value_for_root
 * 
 * @return indicates whether or not the procedure was successful in finding a root.
 * 
 * The solver proceeds by steps, in which a new guess for the root is determined from the previous guess
 * using the `get_next_guess` method, which must be implemented by derived classes. When a new guess is
 * found, it is first checked to see if it lies within the acceptable bounds. If it lies outside the bounds,
 * it is adjusted with the `adjust_bad_guess` method, which has a default behavior but can be overridden by
 * derived classes. If the new guess doesn't need an adjustment, it is compared to the previous guess to
 * check for convergence using either an absolute or relative threshold. The process ends when convergence
 * occurs or when the number of iterations exceeds a threshold value.
 */
bool se_solver::solve(
    std::shared_ptr<simultaneous_equations> const& se,
    std::vector<double> const& initial_guess_for_root,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double>& final_value_for_root)
{
    // Reset all the status indicators
    solve_method_has_been_called = true;
    converged_abs = false;
    converged_rel = false;
    num_iterations = 0;
    num_adjustments = 0;

    // Initialize temporary variables for the loop
    std::vector<double> previous_guess = initial_guess_for_root;
    std::vector<double> next_guess = initial_guess_for_root;

    // Define a lambda to simplify error checking
    auto errors_occurred = [](std::vector<bool> error_vector) -> bool {
        int num_errors = std::accumulate(error_vector.begin(), error_vector.end(), 0);
        return num_errors > 0;
    };

    // Define a lambda to simplify convergence checking
    auto check_convergence = [&errors_occurred](std::vector<bool> convergence_error_vector, bool* convergence_achieved) -> void {
        if (!errors_occurred(convergence_error_vector)) {
            *convergence_achieved = true;
        }
    };

    // Run the loop
    do {
        ++num_iterations;

        next_guess = get_next_guess(se, previous_guess);

        // Check to see if the new guess is acceptable.
        // If it is, check to see if we have reached convergence.
        if (errors_occurred(is_outside_bounds(next_guess, lower_bounds, upper_bounds))) {
            ++num_adjustments;
            next_guess = adjust_bad_guess(next_guess, lower_bounds, upper_bounds);
        } else {
            std::map<std::vector<bool>, bool*> convergence_checks = {
                {has_not_converged_abs(previous_guess, next_guess, abs_error_tolerance),    &converged_abs},
                {has_not_converged_rel(previous_guess, next_guess, rel_error_tolerance),    &converged_rel}
            };

            for (auto const& x : convergence_checks) {
                check_convergence(x.first, x.second);
            }
        }
        
        previous_guess = next_guess;
        
    } while (num_iterations < max_iterations && !converged_abs && !converged_rel);

    final_value_for_root = next_guess;

    return converged_abs || converged_rel;
}

/**
 * @brief Sets the default behavior for determining a new guess from a bad one
 * 
 * @param[in] bad_guess a vector that produces a bad output when used as an input to get_next_guess,
 *                      i.e., the next guess based on this one lies outside the acceptable bounds
 * 
 * @param[in] lower_bounds a vector indicating the lower bound for each unknown quantity
 * 
 * @param[in] upper_bounds a vector indicating the upper bound for each unknown quantity
 * 
 * @return a new guess determined by ???
 */
std::vector<double> se_solver::adjust_bad_guess(
    std::vector<double> const& bad_guess,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds)
{
    std::vector<double> new_guess = bad_guess;
    // Algorithm not implemented yet; just return the same guess
    return new_guess;
}

/**
 * @brief Returns a string describing the se_solver's basic properties.
 */
std::string se_solver::generate_info_report() const
{
    return std::string("Name: ") + solver_name +
           std::string("\nRelative error tolerance: ") + std::to_string(rel_error_tolerance) +
           std::string("\nAbsolute error tolerance: ") + std::to_string(abs_error_tolerance);
}

/**
 * @brief Returns a string describing what happened during the most recent
 * attempt to find a root.
 */
std::string se_solver::generate_solve_report() const
{
    if (!solve_method_has_been_called) {
        return std::string("The solver has not been called yet");
    } else {
        auto bool_to_string = [](bool b) -> std::string {
            return b ? std::string("Yes") : std::string("No");
        };

        return std::string("Details about the most recent calculation:") +
               std::string("\n Converged according to absolute threshold: ") + bool_to_string(converged_abs) +
               std::string("\n Converged according to relative threshold: ") + bool_to_string(converged_rel) +
               std::string("\n Number of iterations: ") + std::to_string(num_iterations) +
               std::string("\n Number of adjustments: ") + std::to_string(num_adjustments);
    }
}