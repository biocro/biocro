#ifndef SE_SOLVER_HELPER_FUNCTIONS_H
#define SE_SOLVER_HELPER_FUNCTIONS_H

#include <vector>
#include <cmath>                     // for fabs
#include <numeric>                   // for inner_product
#include "../framework/constants.h"  // for calculation_constants::eps_zero
#include <Rinternals.h>              // for debugging
const bool seshf_print = false;      // for debugging

// Helping functions for adjust_bad_guess_random and generate_guess_list
namespace se_solver_rand
{
double rand_01();
double normal_rand();
std::vector<double> random_vector_on_n_dimensional_sphere(size_t n);
}  // namespace se_solver_rand

/**
 * @brief Determines a new guess from a bad one by randomly replacing bad elements
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
template <typename equation_ptr_type, typename vector_type>
bool adjust_bad_guess_random(
    equation_ptr_type const& se,
    vector_type const& lower_bounds,
    vector_type const& upper_bounds,
    vector_type& bad_guess,
    vector_type& difference_vector_at_bad_guess)
{
    // Determine which elements lie outside the bounds
    std::vector<bool> oob = is_outside_bounds(bad_guess, lower_bounds, upper_bounds);

    // Adjust the problematic elements
    int num_adjusted = 0;
    for (size_t i = 0; i < bad_guess.size(); ++i) {
        if (oob[i]) {
            bad_guess[i] = lower_bounds[i] + se_solver_rand::rand_01() * (upper_bounds[i] - lower_bounds[i]);
            ++num_adjusted;
        }
    }

    // Evaluate the difference vector at the new guess if an adjustment was made
    if (num_adjusted > 0) {
        (*se)(bad_guess, difference_vector_at_bad_guess);  // modifies difference_vector_at_bad_guess
    }

    return num_adjusted > 0;
}

/**
 * @brief Determines a new guess from a bad one by replacing bad elements with the nearest bound.
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
 * Any value below its lower bound will be replaced by the lower bound, and any value above its
 * upper bound will be replaced by the upper bound.
 * 
 * As far as I know, there is no guarantee that this method will help find a good solution. [EBL]
 * 
 */
template <typename equation_ptr_type, typename vector_type>
bool adjust_bad_guess_limits(
    equation_ptr_type const& se,
    vector_type const& lower_bounds,
    vector_type const& upper_bounds,
    vector_type& bad_guess,
    vector_type& difference_vector_at_bad_guess)
{
    // Adjust the problematic elements
    int num_adjusted = 0;
    for (size_t i = 0; i < bad_guess.size(); ++i) {
        if (bad_guess[i] < lower_bounds[i]) {
            bad_guess[i] = lower_bounds[i];
            ++num_adjusted;
        } else if (bad_guess[i] > upper_bounds[i]) {
            bad_guess[i] = upper_bounds[i];
            ++num_adjusted;
        }
    }

    // Evaluate the difference vector at the new guess if an adjustment was made
    if (num_adjusted > 0) {
        (*se)(bad_guess, difference_vector_at_bad_guess);  // modifies difference_vector_at_bad_guess
    }

    return num_adjusted > 0;
}

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

    if (seshf_print) {
        Rprintf(message.c_str());  // print the message now in case an error is thrown
    }

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

    if (seshf_print) {
        Rprintf(message.c_str());  // print the message now in case an error is thrown
    }

    return result;
}

/**
 * @brief Calculates f_scalar = 0.5 * |F_vec|^2 from
 * a vector F_vec.
 */
template <typename vector_type>
double f_scalar_from_f_vec(vector_type F_vec)
{
    return 0.5 * std::inner_product(F_vec.begin(), F_vec.end(), F_vec.begin(), 0.0);
}

/**
 * @brief Calculates f_scalar_norm = |F_vec/F_norm|^2 from
 * a vector F_vec and a normalization vector F_norm.
 */
template <typename vector_type>
double f_scalar_norm_from_f_vec(vector_type F_vec, vector_type F_vec_norm)
{
    double f_scalar_norm = 0.0;
    for (size_t i = 0; i < F_vec.size(); ++i) {
        if (F_vec_norm[i] < calculation_constants::eps_zero) {
            f_scalar_norm += F_vec[i] * F_vec[i];
        } else {
            f_scalar_norm += F_vec[i] * F_vec[i] / (F_vec_norm[i] * F_vec_norm[i]);
        }
    }
    return f_scalar_norm;
}

std::vector<std::vector<double>> generate_guess_list(
    std::vector<double> const& central_guess,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double> const& scale_factors,
    int num_guesses);

#endif
