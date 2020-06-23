#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include "../se_solver.h"

/**
 * @class fixed_point
 * 
 * @brief This class implements the fixed-point method for solving simultaneous equations,
 * where y = f(x) is the new guess for an input vector x. The simultaneous_equations class
 * actually returns the difference delta = f(x) - x, so we determine y by y = x + delta.
 */
class fixed_point : public se_solver
{
   public:
    fixed_point(
        double rel_error_tol,
        double abs_error_tol,
        int max_it) : se_solver(std::string("fixed_point"), rel_error_tol, abs_error_tol, max_it) {}

   private:
    std::vector<double> get_next_guess(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& input_guess) override;
};

std::vector<double> fixed_point::get_next_guess(
    std::unique_ptr<simultaneous_equations> const& se,
    std::vector<double> const& input_guess)
{
    std::vector<double> output_guess(input_guess.size());

    // Get the difference vector
    se->operator()(input_guess, output_guess);

    // Add the original input values to the difference vector to get the final values
    std::transform(output_guess.begin(), output_guess.end(), input_guess.begin(),
                   output_guess.begin(), std::plus<double>());

    return output_guess;
}

#endif
