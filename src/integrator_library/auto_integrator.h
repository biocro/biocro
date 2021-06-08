#ifndef AUTO_INTEGRATOR_H
#define AUTO_INTEGRATOR_H

#include "../integrator.h"
#include "boost_integrators.h"
#include "homemade_euler_integrator.h"
#include "../state_map.h" // for state_vector_map

// A class representing the auto integrator which chooses default methods
template <class state_type>
class auto_integrator : public integrator
{
   public:
    auto_integrator(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps)
        : integrator("auto", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps),
          adaptive_integrator(
              std::unique_ptr<integrator>(
                  new boost_rsnbrk_integrator(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))),
          other_integrator(
              std::unique_ptr<integrator>(
                  new homemade_euler_integrator<state_type>(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))) {}

   private:
    std::unique_ptr<integrator> adaptive_integrator;
    std::unique_ptr<integrator> other_integrator;

    bool adaptive_integrator_most_recent;

    state_vector_map
    do_solve(std::shared_ptr<System> sys) override
    {
        // The system is compatible with adaptive step size methods,
        // so use the adaptive integrator to solve the system:
        adaptive_integrator_most_recent = true;
        return adaptive_integrator->solve(sys);
    }

    state_vector_map
    handle_adaptive_incompatibility(std::shared_ptr<System> sys) override
    {
        // The system is not compatible with adaptive step size methods,
        // so use the other integrator to solve the system
        adaptive_integrator_most_recent = false;
        return other_integrator->solve(sys);
    }

    std::string get_param_info() const override
    {
        return std::string("\nThis integrator chooses between two defaults ") +
               std::string("depending on the type of system it solves") +
               std::string("\nintegrator used for adaptive-compatible systems:\n") +
               adaptive_integrator->generate_info_report() +
               std::string("\nintegrator used for non-adaptive-compatible systems:\n") +
               other_integrator->generate_info_report();
    }

    std::string get_solution_info() const override
    {
        if (adaptive_integrator_most_recent) {
            return std::string("The integrator for adaptive-compatible systems was used.\n") +
                   std::string("It reports the following information:\n") +
                   adaptive_integrator->generate_solve_report();
        } else {
            return std::string("The integrator for non-adaptive-compatible systems was used.\n") +
                   std::string("It reports the following information:\n") +
                   other_integrator->generate_solve_report();
        }
    }
};

#endif
