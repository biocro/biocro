#include "se_solver_helper_functions.h"
#include "../framework/constants.h"  // for eps_zero
#include <random>                    // for random number generation
#include <limits>                    // for std::numeric_limits<double>::max()
#include <cmath>                     // for std::nextafter

// Helping functions for adjust_bad_guess_random
namespace se_solver_rand
{
// A random_device to be used to obtain a seed for the random number engine
std::random_device rd;

// A standard mersenne_twister_engine seeded with rd()
std::mt19937 eng(rd());

// A uniform real distribution on the closed interval [0,1]
std::uniform_real_distribution<double> uniform_real_dist(0, std::nextafter(1, std::numeric_limits<double>::max()));

// A normal real distribution with a mean of 0 and a standard deviation of 1
std::normal_distribution<double> normal_real_dist(0.0, 1.0);

/**
 * @brief A function that returns a double between 0 and 1 chosen from a uniform distribution
 */
double rand_01()
{
    return uniform_real_dist(eng);
}

// A function that uses these objects to return a normally distributed real number
/**
 * @brief A function that returns a double chosen from a normal distribution with
 * a mean of 0 and a standard deviation of 1
 */
double normal_rand()
{
    return normal_real_dist(eng);
}

/**
 * @brief A function that returns a random vector on the surface of an n-dimensional sphere.
 * Here each component is chosen from a normal distribution, and then the vector is normalized
 * to the unit sphere. See section 4 of "Three Different Algorithms for Generating Uniformly
 * Distributed Random Points on the N-Sphere" by Jan Poland (Oct 24, 2000). May be available
 * at https://www-alg.ist.hokudai.ac.jp/~jan/randsphere.pdf.
 */
std::vector<double> random_vector_on_n_dimensional_sphere(size_t n)
{
    // Create a vector and populate it with normally distributed values
    std::vector<double> vec(n);
    for (size_t i = 0; i < n; ++i) {
        vec[i] = normal_rand();
    }

    // Find its norm
    double vec_norm = sqrt(std::inner_product(vec.begin(), vec.end(), vec.begin(), 0.0));

    // Check for the rare case that we have somehow chosen the zero vector
    // In this case, just set all elements to 1 and recalculate the norm
    if (vec_norm < calculation_constants::eps_zero) {
        for (size_t i = 0; i < n; ++i) {
            vec[i] = 1.0;
        }
        vec_norm = sqrt(std::inner_product(vec.begin(), vec.end(), vec.begin(), 0.0));
    }

    // Normalize the vector
    std::transform(vec.begin(), vec.end(), vec.begin(),
                   [&vec_norm](double v) -> double { return v / vec_norm; });

    return vec;
}

}  // namespace se_solver_rand

/**
 * @brief Determines a set of multiple initial guesses distributed around a central guess. The
 * resulting output will include the central guess and an additional `num_guesses` new guesses.
 * 
 * For an N-dimensional central guess, each new guess will be determined by first generating
 * a random vector on the N-dimensional unit sphere: X.
 * Then new_guess[i] = central_guess[i] + X[i] * radius[i], where radius[i] is set by the minimum
 * of scale_factors[i], 0.95 * |central_guess[i] - upper_bounds[i]|, and
 * 0.95 * |central_guess[i] -  lower_bounds[i]|. This should ensure that each guess lies within the
 * range of acceptable values.
 */
std::vector<std::vector<double>> generate_guess_list(
    std::vector<double> const& central_guess,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double> const& scale_factors,
    int num_guesses)
{
    // Determine the radii to use
    std::vector<double> radii(scale_factors.size());
    double const reduction_factor = 0.95;
    for (size_t i = 0; i < radii.size(); ++i) {
        radii[i] = std::min({scale_factors[i],
                             reduction_factor * fabs(central_guess[i] - upper_bounds[i]),
                             reduction_factor * fabs(central_guess[i] - lower_bounds[i])});
    }

    // Store the central guess
    std::vector<std::vector<double>> guesses;
    guesses.push_back(central_guess);

    // Get the other guesses
    std::vector<double> temp_guess(central_guess.size());
    for (int i = 0; i < num_guesses; ++i) {
        std::vector<double> unit_vector = se_solver_rand::random_vector_on_n_dimensional_sphere(central_guess.size());
        for (size_t j = 0; j < central_guess.size(); ++j) {
            temp_guess[j] = central_guess[j] + radii[j] * unit_vector[j];
        }
        guesses.push_back(temp_guess);
    }

    return (guesses);
}
