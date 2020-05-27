#ifndef SE_SOLVER_H
#define SE_SOLVER_H

#include <memory>   // for std::unique_ptr
#include <numeric>  // for std::accumulate
#include <vector>
#include <string>
#include "se_solver_helper_functions.h"
#include "simultaneous_equations.h"
#include "state_map.h"

/**
 * @class se_observer_null
 * 
 * @brief An observer to be used when the user doesn't specify one. This observer doesn't do anything.
 */
struct se_observer_null {
    se_observer_null() {}
    void operator()(std::vector<double> const& /*uq*/, bool /*adjustment_made*/) {}
};

/**
 * @class se_solver
 * 
 * @brief An abstract class for a generic simultaneous equation solver. Its `solve` member function provides
 * a uniform interface for all derived solvers, and its constructor defines inputs that are common to all
 * solvers.
 */
class se_solver
{
   public:
    se_solver(
        std::string solver_name,
        int max_it) : solver_name(solver_name),
                      max_iterations(max_it) {}

    // Make the destructor a pure virtual function to indicate that this class should be abstract
    virtual ~se_solver() = 0;

    template <typename observer_type>
    bool solve(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& initial_guess_for_root,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double> const& absolute_error_tolerances,
        std::vector<double> const& relative_error_tolerances,
        std::vector<double>& final_value_for_root,
        observer_type observer);

    bool solve(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& initial_guess_for_root,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double> const& absolute_error_tolerances,
        std::vector<double> const& relative_error_tolerances,
        std::vector<double>& final_value_for_root)
    {
        return solve(se, initial_guess_for_root,
                     lower_bounds, upper_bounds,
                     absolute_error_tolerances, relative_error_tolerances,
                     final_value_for_root, se_observer_null());
    }

    std::string generate_info_report() const;
    std::string generate_solve_report() const;

   private:
    const std::string solver_name;

    const int max_iterations;

    bool solve_method_has_been_called = false;
    bool converged_abs;
    bool converged_rel;
    int num_iterations;
    int num_adjustments;

    virtual bool get_next_guess(
        std::unique_ptr<simultaneous_equations> const& se,
        std::vector<double> const& input_guess,
        std::vector<double> const& difference_vector_at_input_guess,
        std::vector<double>& output_guess) = 0;

    virtual std::vector<double> adjust_bad_guess(
        std::vector<double> const& bad_guess,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds);
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline se_solver::~se_solver()
{
}

/**
 * @brief Tries to find a root, starting at the initial guess and staying within the bounds.
 * 
 * @param[in] se
 * 
 * @param[in] initial_guess_for_root
 * 
 * @param[in] lower_bounds
 * 
 * @param[in] upper_bounds
 * 
 * @param[in] absolute_error_tolerances
 * 
 * @param[in] relative_error_tolerances
 * 
 * @param[out] final_value_for_root
 * 
 * @param[in] observer an object with an operator()(std::vector<double> const& uq, bool adjustment_made)
 *                     method. This method will be called at each step of the solution process.
 * 
 * @return indicates whether or not the procedure was successful in finding a root.
 * 
 * The solver proceeds by steps, in which the current guess is checked to determine whether it is a root.
 * If the guess is not sufficiently close to a root, a new guess is determined using the `get_next_guess`
 * method, which must be implemented by derived classes. When a new guess is found, it is first checked
 * to see if it lies within the acceptable bounds. If it lies outside the bounds, it is adjusted with the
 * `adjust_bad_guess` method, which has a default behavior but can be overridden by derived classes.
 * 
 * Each guess is checked according to absolute and relative criteria, and a root is found when both tests
 * are passed.
 * 
 * If the number of iterations exceeds a threshold value without finding a root, the loop ends and a failure
 * is indicated.
 */
template <typename observer_type>
bool se_solver::solve(
    std::unique_ptr<simultaneous_equations> const& se,
    std::vector<double> const& initial_guess_for_root,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double> const& absolute_error_tolerances,
    std::vector<double> const& relative_error_tolerances,
    std::vector<double>& final_value_for_root,
    observer_type observer)
{
    // Reset all the status indicators
    solve_method_has_been_called = true;
    converged_abs = false;
    converged_rel = false;
    num_iterations = 0;
    num_adjustments = 0;

    // Define a lambda to simplify error checking. If all entries in `convergence_error_vector`
    // are `false`, `convergence_achieved` is set to true.
    auto no_errors_occurred = [](std::vector<bool> convergence_error_vector) -> bool {
        int num_errors = std::accumulate(convergence_error_vector.begin(), convergence_error_vector.end(), 0);
        if (num_errors < 1) {
            return true;
        } else {
            return false;
        }
    };

    // Initialize local variables for the loop
    std::vector<double> prev_guess = initial_guess_for_root;
    std::vector<double> next_guess = prev_guess;
    std::vector<double> difference_vector(initial_guess_for_root.size());
    bool problem_occurred_while_getting_guess = false;
    bool need_to_make_adjustment = false;
    bool zero_found = false;

    // Run the loop until convergence is achieved or the max number of iterations is exceeded
    do {
        // Pass the previous guess to the observer
        observer(prev_guess, need_to_make_adjustment);

        // Evaluate the simultaneous equations at the previous guess to get the
        // corresponding difference vector
        (*se)(prev_guess, difference_vector);  // modifies difference_vector

        // Check to see if the previous guess is a zero
        converged_abs = no_errors_occurred(has_not_converged_abs(difference_vector, absolute_error_tolerances));
        converged_rel = no_errors_occurred(has_not_converged_rel(difference_vector, prev_guess, relative_error_tolerances));
        zero_found = converged_abs && converged_rel;

        // If a zero was found or a problem occurred while getting the guess, break out of the loop
        // Otherwise, get a new guess and try again.
        if (zero_found || problem_occurred_while_getting_guess) {
            break;
        } else {
            // Get the next guess
            problem_occurred_while_getting_guess = get_next_guess(se, prev_guess, difference_vector, next_guess);

            // Adjust the guess if it lies outside the acceptable bounds
            need_to_make_adjustment = !no_errors_occurred(is_outside_bounds(next_guess, lower_bounds, upper_bounds));
            if (need_to_make_adjustment) {
                ++num_adjustments;
                next_guess = adjust_bad_guess(next_guess, lower_bounds, upper_bounds);
            }

            // Increment the iteration counter
            ++num_iterations;

            // Update the guess
            prev_guess = next_guess;
        }

    } while (num_iterations < max_iterations);

    final_value_for_root = prev_guess;

    return zero_found;
}

/**
 * @class se_observer_push_back
 * 
 * @brief An observer to be passed to an se_solver via its `solve` method.
 * Its purpose is to store the sequence of guesses for the values of the
 * unknown quantities, along with additional information, e.g., whether
 * each new guess was made using the `get_next_guess` (adjustment_made == false)
 * or `adjust_bad_guess` (adjustment_made == true) methods.
 */
struct se_observer_push_back {
    // Data members
    std::vector<std::vector<double>>& uq_vec;
    std::vector<bool>& adjustment_made_vec;

    // Constructor
    se_observer_push_back(
        std::vector<std::vector<double>>& uq_vec,
        std::vector<bool>& adjustment_made_vec) : uq_vec(uq_vec),
                                                  adjustment_made_vec(adjustment_made_vec) {}

    // Operation
    void operator()(
        std::vector<double> const& uq,
        bool adjustment_made)
    {
        uq_vec.push_back(uq);
        adjustment_made_vec.push_back(adjustment_made);
    }
};

state_vector_map format_se_solver_results(
    std::vector<std::string> quantity_names,
    std::vector<std::vector<double>> uq_vector,
    std::vector<bool> adjustment_made_vector);

#endif
