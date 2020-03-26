#ifndef SE_SOLVER_HELPER_FUNCTIONS_H
#define SE_SOLVER_HELPER_FUNCTIONS_H

#include <vector>
#include <cmath>  // for fabs

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
 * @brief Checks whether the guesses have converged using an absolute threshold
 * 
 * @param[in] old_guess
 * 
 * @param[in] new_guess
 * 
 * @param[in] tolerance
 * 
 * @return a vector of bools where `true` indicates that the corresponding element
 *         of the input vectors has not met the convergence criteria.
 */
template <typename vector_type>
std::vector<bool> has_not_converged_abs(
    vector_type const& old_guess,
    vector_type const& new_guess,
    double const& tolerance)
{
    std::vector<bool> result(new_guess.size());

    for (size_t i = 0; i < new_guess.size(); ++i) {
        result[i] = fabs(new_guess[i] - old_guess[i]) > tolerance;
    }

    return result;
}

/**
 * @brief Checks whether the guesses have converged using a relative threshold
 * 
 * @param[in] old_guess
 * 
 * @param[in] new_guess
 * 
 * @param[in] tolerance
 * 
 * @return a vector of bools where `true` indicates that the corresponding element
 *         of the input vectors has not met the convergence criteria.
 */
template <typename vector_type>
std::vector<bool> has_not_converged_rel(
    vector_type const& old_guess,
    vector_type const& new_guess,
    double const& tolerance)
{
    std::vector<bool> result(new_guess.size());

    for (size_t i = 0; i < new_guess.size(); ++i) {
        result[i] = fabs((new_guess[i] - old_guess[i]) / old_guess[i]) > tolerance;
    }

    return result;
}

#endif