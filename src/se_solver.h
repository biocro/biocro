#ifndef SE_SOLVER_H
#define SE_SOLVER_H

#include <memory>  // for std::shared_ptr
#include <vector>
#include "simultaneous_equations.h"

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
        double rel_error_tol,
        double abs_error_tol,
        int max_it) : rel_error_tolerance(rel_error_tol),
                      abs_error_tolerance(abs_error_tol),
                      max_iterations(max_it) {}

    // Make the destructor a pure virtual function to indicate that this class should be abstract
    virtual ~se_solver() = 0;

    bool solve(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& initial_guess_for_root,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double>& final_value_for_root);

    std::string generate_info_report() const;
    std::string generate_solve_report() const;

   private:
    const std::string solver_name;

    const double rel_error_tolerance;
    const double abs_error_tolerance;
    const int max_iterations;

    bool solve_method_has_been_called = false;
    bool converged_abs;
    bool converged_rel;
    int num_iterations;
    int num_adjustments;

    virtual std::vector<double> get_next_guess(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& input_guess) = 0;

    virtual std::vector<double> adjust_bad_guess(
        std::vector<double> const& bad_guess,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds);
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline se_solver::~se_solver()
{
}



#endif