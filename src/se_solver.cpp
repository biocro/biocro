#include <map>
#include <random>  // for random number generation
#include <limits>  // for std::numeric_limits<double>::max()
#include <cmath>   // for std::nextafter
#include <Rinternals.h>
#include "se_solver.h"
#include "se_solver_helper_functions.h"

namespace se_solver_rand
{
// A random_device to be used to obtain a seed for the random number engine
std::random_device rd;

// A standard mersenne_twister_engine seeded with rd()
std::mt19937 eng(rd());

// A uniform real distribution on the closed interval [0,1]
std::uniform_real_distribution<double> dist(0, std::nextafter(1, std::numeric_limits<double>::max()));

// A function that uses these objects to return a real number between 0 and 1
double rand_01()
{
    return dist(eng);
}

}  // namespace se_solver_rand

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
 * @return a new guess
 * 
 * The new guess is determined by modifying all elements of bad_guess that lie outside the bounds.
 * 
 * For each such "bad element," a new value is chosen randomly from the range of acceptable values.
 * 
 * As far as I know, there is no guarantee that this method will help find a good solution. [EBL]
 * 
 */
std::vector<double> se_solver::adjust_bad_guess(
    std::vector<double> const& bad_guess,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds)
{
    std::vector<double> new_guess = bad_guess;

    // Determine which elements lie outside the bounds
    std::vector<bool> oob = is_outside_bounds(bad_guess, lower_bounds, upper_bounds);

    // Adjust the problematic elements
    for (size_t i = 0; i < bad_guess.size(); ++i) {
        if (oob[i]) {
            new_guess[i] = lower_bounds[i] + se_solver_rand::rand_01() * (upper_bounds[i] - lower_bounds[i]);
        }
    }

    return new_guess;
}

/**
 * @brief Returns a string describing the se_solver's basic properties.
 */
std::string se_solver::generate_info_report() const
{
    return std::string("Name: ") + solver_name +
           std::string("\nRelative error tolerance: ") + std::to_string(rel_error_tolerance) +
           std::string("\nAbsolute error tolerance: ") + std::to_string(abs_error_tolerance) + 
           std::string("\nMaximum number of iterations: ") + std::to_string(max_iterations);
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

/**
 * @brief Converts the output from an se_observer_push_back into a format that can easily be passed to R
 */
state_vector_map format_se_solver_results(
    std::vector<std::string> quantity_names,
    std::vector<std::vector<double>> uq_vector,
    std::vector<bool> adjustment_made_vector)
{
    // Set up the output
    state_vector_map result;
    std::vector<double> temp(uq_vector.size());
    for (std::string const& n : quantity_names) {
        result[n] = temp;
    }
    result["adjustment_made"] = temp;
    result["iteration"] = temp;

    // Fill in the results
    std::vector<double> uq;
    for (size_t i = 0; i < uq_vector.size(); ++i) {
        uq = uq_vector[i];
        for (size_t j = 0; j < quantity_names.size(); ++j) {
            result[quantity_names[j]][i] = uq[j];
        }
        result["adjustment_made"][i] = adjustment_made_vector[i];
        result["iteration"][i] = i;
    }

    return result;
}
