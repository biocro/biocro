#ifndef SYSTEM_SOLVER_H
#define SYSTEM_SOLVER_H

#include <vector>
#include <boost/numeric/odeint.hpp>  // For use with ODEINT
#include "state_map.h"
#include "system.h"

// An abstract class for a generic system solver
// Its solve function provides a uniform interface for all derived solvers,
//  and its constructor requires inputs that are common to all solvers
class system_solver
{
   public:
    system_solver(
        std::string solver_name,
        bool check_adaptive_compatible,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : solver_name(solver_name),
                         check_adaptive_compatible(check_adaptive_compatible),
                         output_step_size(step_size),
                         adaptive_rel_error_tol(rel_error_tolerance),
                         adaptive_abs_error_tol(abs_error_tolerance),
                         adaptive_max_steps(max_steps) {}

    virtual ~system_solver() {}

    state_vector_map solve(std::shared_ptr<System> sys);

    std::string generate_info_report() const { return std::string("Name: ") + solver_name + get_param_info(); }

    std::string generate_solve_report() const
    {
        if (!solve_method_has_been_called) {
            return std::string("The solver has not been called yet");
        } else {
            return get_solution_info();
        }
    }

   protected:
    double get_output_step_size() const { return output_step_size; }
    double get_adaptive_rel_error_tol() const { return adaptive_rel_error_tol; }
    double get_adaptive_abs_error_tol() const { return adaptive_abs_error_tol; }
    int get_adaptive_max_steps() const { return adaptive_max_steps; }

   private:
    const std::string solver_name;
    const bool check_adaptive_compatible;

    double output_step_size;
    double adaptive_rel_error_tol;
    double adaptive_abs_error_tol;
    int adaptive_max_steps;

    bool solve_method_has_been_called = false;

    virtual state_vector_map do_solve(std::shared_ptr<System> sys) = 0;
    virtual state_vector_map handle_adaptive_incompatibility(std::shared_ptr<System> sys);
    virtual std::string get_param_info() const = 0;
    virtual std::string get_solution_info() const = 0;
};

// Define the standard response to a problem with adaptive compatibility
inline state_vector_map
    system_solver::handle_adaptive_incompatibility(std::shared_ptr<System> /*sys*/)
{
    throw std::logic_error(
        std::string("system_solver '") + solver_name +
        std::string("' is not compatible with the input system.\n"));
}

#endif
