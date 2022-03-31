#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include "../se_solver.h"
#include "../se_solver_helper_functions.h"

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
    fixed_point(int max_it) : se_solver(std::string("fixed_point"), max_it) {}

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

bool fixed_point::get_next_guess(
    std::unique_ptr<simultaneous_equations> const& se,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double> const& input_guess,
    std::vector<double> const& difference_vector_at_input_guess,
    std::vector<double>& output_guess,
    std::vector<double>& difference_vector_at_output_guess)
{
    // Add the original input values to the difference vector to get the final values
    std::transform(difference_vector_at_input_guess.begin(), difference_vector_at_input_guess.end(), input_guess.begin(),
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
