#ifndef AUTO_NUMERICAL_INTEGRATOR_H
#define AUTO_NUMERICAL_INTEGRATOR_H

#include "../numerical_integrator.h"
#include "boost_numerical_integrators.h"
#include "homemade_euler_numerical_integrator.h"
#include "../state_map.h" // for state_vector_map

// A class representing the auto numerical integrator which chooses default
// methods
template <class state_type>
class auto_numerical_integrator : public numerical_integrator
{
   public:
    auto_numerical_integrator(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps)
        : numerical_integrator("auto", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps),
          adaptive_numerical_integrator(
              std::unique_ptr<numerical_integrator>(
                  new boost_rsnbrk_numerical_integrator(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))),
          other_numerical_integrator(
              std::unique_ptr<numerical_integrator>(
                  new homemade_euler_numerical_integrator<state_type>(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))) {}

   private:
    std::unique_ptr<numerical_integrator> adaptive_numerical_integrator;
    std::unique_ptr<numerical_integrator> other_numerical_integrator;

    bool adaptive_numerical_integrator_most_recent;

    state_vector_map
    do_solve(std::shared_ptr<System> sys) override
    {
        // The system is compatible with adaptive step size methods,
        // so use the adaptive numerical_integrator to solve the system:
        adaptive_numerical_integrator_most_recent = true;
        return adaptive_numerical_integrator->solve(sys);
    }

    state_vector_map
    handle_adaptive_incompatibility(std::shared_ptr<System> sys) override
    {
        // The system is not compatible with adaptive step size methods,
        // so use the other numerical_integrator to solve the system
        adaptive_numerical_integrator_most_recent = false;
        return other_numerical_integrator->solve(sys);
    }

    std::string get_param_info() const override
    {
        return std::string("\nThis numerical_integrator chooses between two defaults ") +
               std::string("depending on the type of system it solves") +
               std::string("\nnumerical_integrator used for adaptive-compatible systems:\n") +
               adaptive_numerical_integrator->generate_info_report() +
               std::string("\nnumerical_integrator used for non-adaptive-compatible systems:\n") +
               other_numerical_integrator->generate_info_report();
    }

    std::string get_solution_info() const override
    {
        if (adaptive_numerical_integrator_most_recent) {
            return std::string("The numerical_integrator for adaptive-compatible systems was used.\n") +
                   std::string("It reports the following information:\n") +
                   adaptive_numerical_integrator->generate_solve_report();
        } else {
            return std::string("The numerical_integrator for non-adaptive-compatible systems was used.\n") +
                   std::string("It reports the following information:\n") +
                   other_numerical_integrator->generate_solve_report();
        }
    }
};

#endif
