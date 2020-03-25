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
    // Get the difference vector evaluated at input_guess
    boost::numeric::ublas::vector<double> difference_vector(input_guess.size());
    se->operator()(input_guess, difference_vector);

    // Get the Jacobian matrix of the difference function evaluated at input_guess
    boost::numeric::ublas::matrix<double> jacobian(input_guess.size(), input_guess.size());
    calculate_jacobian(se, input_guess, jacobian);

    // Determine the inverse of the Jacobian matrix using LU decomposition and back-substitution
    // See e.g. http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion
    boost::numeric::ublas::matrix<double> jacobian_inverse = jacobian;            // create a matrix to store the inverse
    boost::numeric::ublas::permutation_matrix<std::size_t> pm(jacobian.size1());  // create a permutation matrix for the LU-factorization
    int res = boost::numeric::ublas::lu_factorize(jacobian, pm);                  // perform LU-factorization
    if (res != 0) {
        throw std::runtime_error(std::string("Thrown by newton_raphson_boost: LU decomposition of Jacobian matrix failed!"));
    }
    jacobian_inverse.assign(boost::numeric::ublas::identity_matrix<double>(jacobian.size1()));  // create identity matrix
    boost::numeric::ublas::lu_substitute(jacobian, pm, jacobian_inverse);
    
    // Get -dx = J^-1 * F
    boost::numeric::ublas::vector<double> neg_dx = boost::numeric::ublas::prod(jacobian_inverse, difference_vector);
    
    // Subtract -dx from the input guess to get the new guess
    std::vector<double> output_guess = input_guess;
    std::transform(output_guess.begin(), output_guess.end(), neg_dx.begin(),
                   output_guess.begin(), std::minus<double>());
                   

    // Print, for debugging
    std::string msg = std::string("\n\n\nJacobian matrix:\n\n\n");
    for (size_t i = 0; i < jacobian.size1(); ++i) {
        msg += std::string("\n");
        for (size_t j = 0; j < jacobian.size2(); ++j) {
            msg += std::to_string(jacobian(i, j)) + std::string(" ");
        }
    }
    msg += std::string("\n\n\n");

    msg += std::string("\n\n\nJacobian inverse matrix:\n\n\n");
    for (size_t i = 0; i < jacobian_inverse.size1(); ++i) {
        msg += std::string("\n");
        for (size_t j = 0; j < jacobian_inverse.size2(); ++j) {
            msg += std::to_string(jacobian_inverse(i, j)) + std::string(" ");
        }
    }
    msg += std::string("\n\n\n");
    
    msg += std::string("\n\n\nDifference vector:\n\n\n");
    for (size_t i = 0; i < neg_dx.size(); ++i) {
        msg += std::to_string(-neg_dx[i]) + std::string(" ");
    }
    msg += std::string("\n\n\n");

    Rprintf(msg.c_str());
    

    return output_guess;
}

#endif