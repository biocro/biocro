#ifndef NEWTON_RAPHSON_BOOST_H
#define NEWTON_RAPHSON_BOOST_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../../framework/numerical_jacobian.h"
#include "../se_solver.h"
#include "../se_solver_helper_functions.h"
#include <algorithm>  // for std::transform

/**
 * @brief Determines the Newton-Raphson step using the boost library
 */
boost::numeric::ublas::vector<double> get_newton_raphson_step_boost(
    boost::numeric::ublas::vector<double> F_x_0,
    boost::numeric::ublas::matrix<double> jacobian)
{
    // Determine the inverse of the Jacobian matrix: J^(-1)
    // See e.g. http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion
    boost::numeric::ublas::permutation_matrix<std::size_t> pm(jacobian.size1());  // create a permutation matrix for the LU-factorization
    int res = boost::numeric::ublas::lu_factorize(jacobian, pm);                  // perform the LU-factorization
    if (res != 0) {
        throw std::runtime_error(std::string("Thrown by newton_raphson_boost: LU factorization of Jacobian matrix failed!"));
    }
    boost::numeric::ublas::matrix<double> jacobian_inverse =
        boost::numeric::ublas::identity_matrix<double>(jacobian.size1());  // initialize the inverse to be an identity matrix
    boost::numeric::ublas::lu_substitute(jacobian, pm, jacobian_inverse);  // find the inverse by back-substitution

    // Calculate the change in x according to the Newton-Raphson formula: dx = -J^(-1) * F
    return -1.0 * boost::numeric::ublas::prod(jacobian_inverse, F_x_0);
}

/**
 * @class newton_raphson_boost
 *
 * @brief This class implements the Newton-Raphson method for solving simultaneous equations.
 * Matrix operations are accomplished using the boost ublas library.
 */
class newton_raphson_boost : public se_solver
{
   public:
    newton_raphson_boost(int max_it) : se_solver(std::string("newton_raphson_boost"), max_it) {}

   private:
    bool get_next_guess(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double> const& input_guess,
        std::vector<double> const& difference_vector_at_input_guess,
        std::vector<double>& output_guess,
        std::vector<double>& difference_vector_at_output_guess) override;
};

bool newton_raphson_boost::get_next_guess(
    std::unique_ptr<simultaneous_equations> const& se,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double> const& input_guess,
    std::vector<double> const& difference_vector_at_input_guess,
    std::vector<double>& output_guess,
    std::vector<double>& difference_vector_at_output_guess)
{
    // Evaluate the Jacobian matrix of the function at input_guess
    boost::numeric::ublas::matrix<double> jacobian(input_guess.size(), input_guess.size());
    calculate_jacobian_nt(se, input_guess, difference_vector_at_input_guess, jacobian);  // modifies Jacobian

    // Convert difference_vector_at_input_guess to a boost vector
    boost::numeric::ublas::vector<double> function_value(difference_vector_at_input_guess.size());
    for (size_t i = 0; i < difference_vector_at_input_guess.size(); ++i) {
        function_value[i] = difference_vector_at_input_guess[i];
    }

    // Determine the Newton-Raphson step
    boost::numeric::ublas::vector<double> dx;
    try {
        dx = get_newton_raphson_step_boost(function_value, jacobian);
    } catch (std::runtime_error const& e) {
        // The LU decomposition has failed, so indicate that an error occurred
        return true;
    }

    // Determine the new guess by taking the full step, i.e., calculating x_new = x_0 + dx
    output_guess = input_guess;
    std::transform(output_guess.begin(), output_guess.end(), dx.begin(),
                   output_guess.begin(), std::plus<double>());  // modifies output_guess

    // Evaluate the difference vector at the new guess
    (*se)(output_guess, difference_vector_at_output_guess);  // modifies difference_vector_at_output_guess

    // Adjust the new guess if it lies outside the bounds
    // adjust_bad_guess_limits modifies output_guess and difference_vector_at_output_guess if output_guess
    // lies outside the bounds
    adjust_bad_guess_limits(se, lower_bounds, upper_bounds, output_guess, difference_vector_at_output_guess);

    // This algorithm doesn't need to check for any additional problems,
    // so just return false
    return false;
}

#endif
