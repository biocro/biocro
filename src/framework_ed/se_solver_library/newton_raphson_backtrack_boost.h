#ifndef NEWTON_RAPHSON_BACKTRACK_BOOST_H
#define NEWTON_RAPHSON_BACKTRACK_BOOST_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include "../../framework/numerical_jacobian.h"
#include "../se_solver.h"
#include "../se_solver_helper_functions.h"  // for f_scalar_from_F_vec
#include "newton_raphson_boost.h"           // for get_newton_raphson_step_boost
#include <cmath>                            // for sqrt
#include <numeric>                          // for std::inner_product
#include <algorithm>                        // for std::transform
#include <Rinternals.h>                     // for debugging
const bool nrb_print = false;               // for debugging

/**
 * @brief Searches along a line for a point that sufficiently decreases the non-negative
 * scalar-valued function f_scalar = 0.5 * |F_vec|^2, where F_vec is the vector-valued
 * function whose root is being found by the calling function. Guesses are determined by
 * x_new = x_old + lambda * direction. Here `direction` is typically the Newton-Raphson
 * step and `lambda` is a number in (0,1]. We begin with the full step (lambda = 1). If
 * this lambda value is rejected, we try smaller values until f_scalar is found to decrease
 * by a sufficient amount. See section 9.7 of Numerical Recipes in C.
 */
template <typename equation_ptr_type, typename vector_type>
bool newton_raphson_line_search_boost(
    vector_type const& lower_bounds,
    vector_type const& upper_bounds,
    double min_rescale_factor,
    double min_step_factor,
    double f_decrease_factor,
    boost::numeric::ublas::vector<double> direction,
    boost::numeric::ublas::vector<double> const& F_vec_old,
    boost::numeric::ublas::matrix<double> const& jacobian,
    equation_ptr_type const& F_vec,
    vector_type const& x_old,
    vector_type& x_new,
    vector_type& F_vec_new)
{
    std::string message = "Running the backtracking line search:\n";
    char buff[128];

    message += " x_old:";
    for (double const& v : x_old) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    message += " direction:";
    for (double const& v : direction) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    message += " F_vec_old:";
    for (double const& v : F_vec_old) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    message += " jacobian:\n";
    for (size_t i = 0; i < jacobian.size1(); ++i) {
        message += " ";
        for (size_t j = 0; j < jacobian.size2(); ++j) {
            sprintf(buff, " %e", jacobian(i, j));
            message += std::string(buff);
        }
        message += "\n";
    }

    // If the full step (lambda = 1) would produce a guess that lies outside the
    // bounds, rescale it. Here we assume that x_old lies within the bounds. We
    // check each element in the following manner:
    //
    // If an element lies above the upper bound, find an appropriate rescaling
    // factor r such that x_old[i] + direction[i] * r = upper_bounds[i],
    // i.e., r = (upper_bounds[i] - x_old[i]) / direction[i].
    //
    // If an element lies below the lower bound, find an appropriate rescaling
    // factor r such that x_old[i] + direction[i] * r = lower_bounds[i],
    // i.e., r = (lower_bounds[i] - x_old[i]) / direction[i].
    //
    // Then we use the smallest rescale factor as the overall factor and rescale
    // the direction. This should ensure that x_new lies within the bounds.
    double rescale_factor = 1.0;
    for (size_t i = 0; i < x_old.size(); ++i) {
        if (x_old[i] + direction[i] > upper_bounds[i]) {
            rescale_factor = std::min(rescale_factor, (upper_bounds[i] - x_old[i]) / direction[i]);
        } else if (x_old[i] + direction[i] < lower_bounds[i]) {
            rescale_factor = std::min(rescale_factor, (lower_bounds[i] - x_old[i]) / direction[i]);
        }
    }

    sprintf(buff, " recale_factor = %e\n", rescale_factor);
    message += std::string(buff);
    if (nrb_print) {
        Rprintf(message.c_str());  // print the message now in case an error is thrown
    }

    // If the guess is right at the edge of the bounds and the Newton-Raphson direction is
    // pushing it out of bounds, the rescale factor will be very small. In this case, additional
    // steps will probably not lead to a solution, so we report that a problem has occurred by
    // returning a value of `true`.
    if (rescale_factor < min_rescale_factor) {
        if (nrb_print) {
            Rprintf("rescale_factor is too small, so we are abandoning the line search\n");
        }
        return true;
    }

    // The rescale factor should never be greater than 1, so throw an error if this
    // somehow manages to occur. If it's not 1, rescale the direction.
    if (rescale_factor > 1.0) {
        // This shouldn't happen
        throw std::runtime_error("Thrown by newton_raphson_line_search_boost: Rescale factor negative or greater than one.");
    } else if (rescale_factor != 1.0) {
        // Rescale the direction vector
        std::transform(direction.begin(), direction.end(), direction.begin(),
                       [&rescale_factor](double v) -> double { return v * rescale_factor; });
    }

    message = " rescaled direction:";
    for (double const& v : direction) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    // Compute the gradient of f_scalar = 0.5 * |F_vec|^2 at x_old using the
    // Jacobian and F_vec calculated at x_old. As in Equation 9.7.5,
    // grad_f_scalar = F_vec * jacobian
    boost::numeric::ublas::vector<double> grad_f_scalar_old = boost::numeric::ublas::prod(F_vec_old, jacobian);

    message += " grad_f_scalar_old:";
    for (double const& v : grad_f_scalar_old) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    // Compute the "slope", which is grad_f_scalar_old dot direction.
    // This quantity is the initial rate of decrease of f_scal
    // as we move along the line. This value must be negative
    // by definition, so a non-negative value indicates an error.
    const double slope = std::inner_product(direction.begin(), direction.end(),
                                            grad_f_scalar_old.begin(), 0.0);

    sprintf(buff, " slope = %e\n", slope);
    message += std::string(buff);

    if (nrb_print) {
        Rprintf(message.c_str());  // print the message now in case an error is thrown
    }

    if (slope >= 0.0) {
        throw std::runtime_error("Thrown by newton_raphson_line_search: roundoff problem occurred.");
    }

    // The ith component of delta_x = lambda * direction is given by
    // delta_x_i = lambda * direction_i.
    //
    // We want to prevent steps that are too small, i.e.,
    // where delta_x_i is too small. To enforce this, we can set
    // a minimum step size determined by a fraction of x_old_i:
    //
    // delta_x_min_i = min_step_factor * abs(x_old_i).
    //
    // So now each element has its own minimum lambda, determined
    // by the following:
    //
    // lambda_min_i = min_step_factor * abs(x_old_i) / abs(direction_i)
    //
    // We can choose an overall lambda_min by minimizing over the individual
    // values.
    //
    // A problem may arise if any x_old_i are zero or very small, since in that
    // case lambda_min will be zero or very small, defeating the purpose of
    // choosing a minimum. To prevent this, we set a minimum value for abs(x_old_i).
    // In the original Numerical Recipes version, this minimum was 1. However, this
    // can lead to some weird results if a root occurs near a very small value of x_i,
    // since in this case lambda_min might be greater than 1. To avoid this issue,
    // we use a smaller value.
    double const min_x_old_i = 1e-10;
    vector_type lambda_mins = x_old;  // std::transform cannot easily write to an empty vector
    std::transform(direction.begin(), direction.end(), x_old.begin(), lambda_mins.begin(),
                   [&min_step_factor, &min_x_old_i](double d_i, double x_i) -> double { return min_step_factor * std::max(fabs(x_i), min_x_old_i) / fabs(d_i); });

    message = " lambda_mins:";  // reset the message since we just printed it
    for (double const& v : lambda_mins) {
        sprintf(buff, " %e", v);
        message += std::string(buff);
    }
    message += "\n";

    const double lambda_min = *std::min_element(lambda_mins.begin(), lambda_mins.end());

    sprintf(buff, " lambda_min = %e\n", lambda_min);
    message += std::string(buff);

    // Determine the value of f_scalar at x_old
    double const f_scalar_old = f_scalar_from_f_vec(F_vec_old);

    sprintf(buff, " f_scalar_old = %e\n", f_scalar_old);
    message += std::string(buff);

    // Initialize local variables for the loop
    double lambda = 1.0;        // always try the full step first
    double f_scalar_new = 0.0;  // will be recalculated at each step
    double lambda_2 = 0.0;      // will be initialized at the end of the first loop, but not required during the first iteration
    double f_scalar_2 = 0.0;    // will be initialized at the end of the first loop, but not required during the first iteration
    bool found_acceptable_step = false;
    bool found_possible_local_min = false;

    message += " Beginning the loop:\n";

    if (nrb_print) {
        Rprintf(message.c_str());  // print the message now in case an error is thrown
    }

    // Search for a new value for x
    do {
        sprintf(buff, " lambda = %e\n", lambda);
        message = std::string(buff);

        // Get the x_new value corresponding to lambda: x_new = x_old + lambda * direction
        std::transform(x_old.begin(), x_old.end(), direction.begin(), x_new.begin(),
                       [&lambda](double x_old_i, double d_i) -> double { return x_old_i + lambda * d_i; });

        message += "  x_new:";
        for (double const& v : x_new) {
            sprintf(buff, " %e", v);
            message += std::string(buff);
        }
        message += "\n";

        // Evaluate F_vec and f_scalar at the new guess
        (*F_vec)(x_new, F_vec_new);  // modifies F_vec_new
        f_scalar_new = f_scalar_from_f_vec(F_vec_new);

        message += "  F_vec_new:";
        for (double const& v : F_vec_new) {
            sprintf(buff, " %e", v);
            message += std::string(buff);
        }
        message += "\n";

        sprintf(buff, "  f_scalar_new = %e\n", f_scalar_new);
        message += std::string(buff);

        // Check to see if we have found a possible zero or an acceptable step.
        // If not, determine a new value of lambda to try.
        if (lambda < lambda_min) {
            // The step is very small, so we are probably stuck near a local min.
            // In this case, indicate that a problem has occurred.
            found_possible_local_min = true;
            message += "  lambda < lambda_min, so we are abandoning the line search\n";
            if (nrb_print) {
                Rprintf(message.c_str());  // print the message now in case an error is thrown
            }
        } else if (f_scalar_new <= f_scalar_old + f_decrease_factor * lambda * slope) {
            // f_scalar has decreased by a sufficient amount, so we can accept x_new
            found_acceptable_step = true;
            message += "  f_scalar_new <= f_scalar_old + f_decrease_factor * lambda * slope\n";
            if (nrb_print) {
                Rprintf(message.c_str());  // print the message now in case an error is thrown
            }
        } else {
            // We need to choose a new value of lambda to try
            double temporary_lambda;
            if (lambda == 1.0) {
                // The previous attempt was the full Newton step,
                // so this is the first backtrack.
                // Get a new value of lambda using Equation 9.7.11
                temporary_lambda = -slope / (2.0 * (f_scalar_new - f_scalar_old - slope));
                message += "  doing first backtrack\n";
            } else {
                // This the second or a subsequent backtrack
                // First determine the coefficients a and b
                // using Equation 9.7.13
                message += "  doing additional backtrack\n";
                double rhs1 = f_scalar_new - f_scalar_old - lambda * slope;
                double rhs2 = f_scalar_2 - f_scalar_old - lambda_2 * slope;
                double a = (rhs1 / (lambda * lambda) - rhs2 / (lambda_2 * lambda_2)) / (lambda - lambda_2);
                double b = (-lambda_2 * rhs1 / (lambda * lambda) + lambda * rhs2 / (lambda_2 * lambda_2)) / (lambda - lambda_2);

                // Now determine a new value for lambda by locating the minimum
                // of Equation 9.7.12
                if (a == 0.0) {
                    // If `a` is zero, Equation 9.7.12 is quadratic
                    temporary_lambda = -slope / (2.0 * b);
                } else {
                    // Equation 9.7.12 is cubic with a minimum occurring at a
                    // value of lambda given by Equation 9.7.14.
                    // Begin solving by calculating the value under the square root,
                    // called `disc`
                    double disc = b * b - 3.0 * a * slope;
                    if (disc < 0.0) {
                        // Just choose the maximum allowed value for lambda
                        // when the cubic approximation to g has no minimum.
                        temporary_lambda = 0.5 * lambda;
                    } else if (b <= 0.0) {
                        // Use Equation 9.7.14
                        temporary_lambda = (-b + sqrt(disc)) / (3.0 * a);
                    } else {
                        // Not sure why we do this, but it's part of the code
                        // in Numerical Recipes in C.
                        temporary_lambda = -slope / (b + sqrt(disc));
                    }
                }

                // Ensure that the new lambda value is smaller than half of the previous value
                temporary_lambda = std::min(temporary_lambda, 0.5 * lambda);
            }

            // Store some values for later loops
            lambda_2 = lambda;
            f_scalar_2 = f_scalar_new;

            // Ensure that the new lambda value is larger than 10% of the previous value
            lambda = std::max(temporary_lambda, 0.1 * lambda);

            if (nrb_print) {
                Rprintf(message.c_str());  // print the message now in case an error is thrown
            }
        }
    } while (found_acceptable_step == false && found_possible_local_min == false);

    message = "\n";

    if (nrb_print) {
        Rprintf(message.c_str());
    }

    return found_possible_local_min;
}

/**
 * @class newton_raphson_backtrack_boost
 *
 * @brief This class implements the backtracking Newton-Raphson method for
 * solving simultaneous equations, as described in section 9.7 of Numerical
 * Recipes in C. Matrix operations are accomplished using the boost ublas
 * library.
 */
class newton_raphson_backtrack_boost : public se_solver
{
   public:
    newton_raphson_backtrack_boost(int max_it) : se_solver(std::string("newton_raphson_backtrack_boost"), max_it) {}

   private:
    double const min_rescale_factor = 1.0e-7;
    double const min_step_factor = 1.0e-7;    // value taken from Numerical Recipes in C (TOLX)
    double const f_decrease_factor = 1.0e-4;  // value taken from Numerical Recipes in C (ALF)
    bool get_next_guess(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double> const& input_guess,
        std::vector<double> const& difference_vector_at_input_guess,
        std::vector<double>& output_guess,
        std::vector<double>& difference_vector_at_output_guess) override;
};

bool newton_raphson_backtrack_boost::get_next_guess(
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
    // (required for get_newton_raphson_step_boost)
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

    // Use the backtracking line search algorithm to determine the next guess,
    // rather than automatically taking the full Newton-Raphson step.
    // The line search will return a value of "true" if a problem occurred
    // (either the search is heading out of bounds or has gotten stuck near
    // a local min). We should pass this as the return value to indicate that
    // a problem occurred.
    output_guess = input_guess;                                            // make sure output_guess is the right size
    difference_vector_at_output_guess = difference_vector_at_input_guess;  // make sure difference_vector_at_output_guess is the right size
    return newton_raphson_line_search_boost(
        lower_bounds,
        upper_bounds,
        min_rescale_factor,
        min_step_factor,
        f_decrease_factor,
        dx,
        function_value,
        jacobian,
        se,
        input_guess,
        output_guess,
        difference_vector_at_output_guess);  // modifies output_guess and difference_vector_at_output_guess
}

#endif
