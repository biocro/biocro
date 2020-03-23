#include <map>
#include "se_solver.h"

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