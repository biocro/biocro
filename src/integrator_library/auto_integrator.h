#ifndef AUTO_INTEGRATOR_H
#define AUTO_INTEGRATOR_H

#include "../integrator.h"
#include "boost_integrators.h"
#include "homemade_euler_integrator.h"
#include "../state_map.h"  // for state_vector_map

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
          advanced_integrator(
              std::unique_ptr<integrator>(
                  new boost_rsnbrk_integrator(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))),
          euler_integrator(
              std::unique_ptr<integrator>(
                  new homemade_euler_integrator<state_type>(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))) {}

   private:
    std::unique_ptr<integrator> advanced_integrator;
    std::unique_ptr<integrator> euler_integrator;

    bool advanced_integrator_most_recent;

    state_vector_map
    do_integrate(std::shared_ptr<dynamical_system> sys) override
    {
        // The `dynamical_system` does not require an Euler integrator, so use
        // the advanced integrator to integrate it
        advanced_integrator_most_recent = true;
        return advanced_integrator->integrate(sys);
    }

    state_vector_map
    handle_euler_requirement(std::shared_ptr<dynamical_system> sys) override
    {
        // The `dynamical_system` requires an Euler integrator, so use the Euler
        // integrator to integrate it
        advanced_integrator_most_recent = false;
        return euler_integrator->integrate(sys);
    }

    std::string get_param_info() const override
    {
        return std::string("\nThis integrator chooses between two defaults ") +
               std::string("depending on the type of system it integrates") +
               std::string("\nintegrator used for dynamical systems that do ") +
               std::string("not require the Euler method:\n") +
               advanced_integrator->generate_info_report() +
               std::string("\nintegrator used for dynamical systems that ") +
               std::string("require the Euler method:\n") +
               euler_integrator->generate_info_report();
    }

    std::string get_solution_info() const override
    {
        if (advanced_integrator_most_recent) {
            return std::string("The integrator for dynamical systems that do") +
                   std::string(" not require the Euler method was used.\n") +
                   std::string("It reports the following information:\n") +
                   advanced_integrator->generate_integrate_report();
        } else {
            return std::string("The integrator for dynamical systems that ") +
                   std::string("require the Euler method was used.\n") +
                   std::string("It reports the following information:\n") +
                   euler_integrator->generate_integrate_report();
        }
    }
};

#endif
