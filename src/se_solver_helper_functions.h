#ifndef SE_SOLVER_HELPER_FUNCTIONS_H
#define SE_SOLVER_HELPER_FUNCTIONS_H

#include <vector>
#include <cmath>         // for fabs
#include "constants.h"   // for calculation_constants::eps_zero
#include <Rinternals.h>  // for debugging

/**
 * @brief Checks whether the elements of vec_to_test lie outside the bounds
 * 
 * @param[in] vec_to_test
 * 
 * @param[in] lower_bounds
 * 
 * @param[in] upper_bounds
 * 
 * @return a vector of bools where `true` indicates that the corresponding element
 *         of vec_to_test lies outside the range set by the corresponding elements
 *         of lower_bounds and upper_bounds.
 */
template <typename vector_type>
std::vector<bool> is_outside_bounds(
    vector_type const& vec_to_test,
    vector_type const& lower_bounds,
    vector_type const& upper_bounds)
{
    std::vector<bool> result(vec_to_test.size());

    for (size_t i = 0; i < vec_to_test.size(); ++i) {
        result[i] = (vec_to_test[i] < lower_bounds[i] || vec_to_test[i] > upper_bounds[i]);
    }

    return result;
}

/**
 * @brief Checks whether the difference vector elements are close to zero using an absolute threshold
 * 
 * @param[in] difference_vector
 * 
 * @param[in] tolerances
 * 
 * @return a vector of bools where `true` indicates that the corresponding element
 *         of the input vectors has not met the convergence criteria.
 */
template <typename vector_type>
std::vector<bool> has_not_converged_abs(
    vector_type const& difference_vector,
    vector_type const& tolerances)
{
    std::vector<bool> result(difference_vector.size());

    std::string message = "Checking absolute convergence:\n";
    char buff[128];

    for (size_t i = 0; i < difference_vector.size(); ++i) {
        result[i] = fabs(difference_vector[i]) > tolerances[i];
        sprintf(buff, " difference = %e, tolerance = %e, result = %i\n", difference_vector[i], tolerances[i], (int)result[i]);
        message += std::string(buff);
    }

    message += "\n";
    Rprintf(message.c_str());

    return result;
}

/**
 * @brief Checks whether the difference vector elements are close to zero using a relative threshold.
 * 
 * @param[in] difference_vector
 * 
 * @param[in] guess
 * 
 * @param[in] tolerances
 * 
 * @return a vector of bools where `true` indicates that the corresponding element
 *         of the input vectors has not met the convergence criteria.
 */
template <typename vector_type>
std::vector<bool> has_not_converged_rel(
    vector_type const& difference_vector,
    vector_type const& guess,
    vector_type const& tolerances)
{
    std::vector<bool> result(difference_vector.size());

    std::string message = "Checking relative convergence:\n";
    char buff[128];

    for (size_t i = 0; i < difference_vector.size(); ++i) {
        double threshold = fabs(guess[i]) * tolerances[i];
        if (threshold < calculation_constants::eps_zero) {
            // If guess[i] is zero, a relative comparison doesn't make sense.
            // Treat `tolerance` as an absolute threshold in this case.
            threshold = tolerances[i];
        }
        result[i] = fabs(difference_vector[i]) > threshold;
        sprintf(buff, " difference = %e, guess = %e, tolerance = %e, threshold = %e, result = %i\n", difference_vector[i], guess[i], tolerances[i], threshold, (int)result[i]);
        message += std::string(buff);
    }

    message += "\n";
    Rprintf(message.c_str());

    return result;
}

#endif
