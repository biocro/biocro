#ifndef NEWTON_RAPHSON_BOOST_H
#define NEWTON_RAPHSON_BOOST_H

#include <Rinternals.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../se_solver.h"
#include "../numerical_jacobian.h"

/**
 * @class newton_raphson_boost
 * 
 * @brief This class implements the fixed-point method for solving simultaneous equations,
 * where y = f(x) is the new guess for an input vector x. The simultaneous_equations class
 * actually returns the difference delta = f(x) - x, so we determine y by y = x + delta.
 */
class newton_raphson_boost : public se_solver
{
   public:
    newton_raphson_boost(
        double rel_error_tol,
        double abs_error_tol,
        int max_it) : se_solver(std::string("newton_raphson_boost"), rel_error_tol, abs_error_tol, max_it) {}

   private:
    std::vector<double> get_next_guess(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& input_guess) override;
};

std::vector<double> newton_raphson_boost::get_next_guess(
    std::shared_ptr<simultaneous_equations> const& se,
    std::vector<double> const& input_guess)
{
    std::string msg = std::string("\n\n\nInput vector:\n");
    for (size_t i = 0; i < input_guess.size(); ++i) {
        msg += std::to_string(input_guess[i]) + std::string(" ");
    }
    
    // Evaluate the function at input_guess: F(x_0)
    boost::numeric::ublas::vector<double> difference_vector(input_guess.size());
    se->operator()(input_guess, difference_vector);
    
    msg += std::string("\n\nDifference vector:\n");
    for (size_t i = 0; i < difference_vector.size(); ++i) {
        msg += std::to_string(difference_vector[i]) + std::string(" ");
    }

    // Evaluate the Jacobian matrix of the function at input_guess: J(x_0)
    boost::numeric::ublas::matrix<double> jacobian(input_guess.size(), input_guess.size());
    calculate_jacobian_nt(se, input_guess, difference_vector, jacobian);
    
    msg += std::string("\n\nJacobian matrix:\n");
    for (size_t i = 0; i < jacobian.size1(); ++i) {
        msg += std::string("\n");
        for (size_t j = 0; j < jacobian.size2(); ++j) {
            msg += std::to_string(jacobian(i, j)) + std::string(" ");
        }
    }

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
    
    msg += std::string("\n\nJacobian inverse matrix:\n");
    for (size_t i = 0; i < jacobian_inverse.size1(); ++i) {
        msg += std::string("\n");
        for (size_t j = 0; j < jacobian_inverse.size2(); ++j) {
            msg += std::to_string(jacobian_inverse(i, j)) + std::string(" ");
        }
    }

    // Calculate the change in x according to the Newton-Raphson formula: dx = J^(-1) * F
    boost::numeric::ublas::vector<double> dx = boost::numeric::ublas::prod(jacobian_inverse, difference_vector);
    
    msg += std::string("\n\nChange in input vector:\n");
    for (size_t i = 0; i < dx.size(); ++i) {
        msg += std::to_string(dx[i]) + std::string(" ");
    }

    // Determine the new guess according to x_new = x_0 - dx
    std::vector<double> output_guess = input_guess;
    std::transform(output_guess.begin(), output_guess.end(), dx.begin(),
                   output_guess.begin(), std::minus<double>());

    msg += std::string("\n\nNew guess:\n");
    for (size_t i = 0; i < output_guess.size(); ++i) {
        msg += std::to_string(output_guess[i]) + std::string(" ");
    }

    Rprintf(msg.c_str());

    return output_guess;
}

#endif