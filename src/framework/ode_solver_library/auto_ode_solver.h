#ifndef AUTO_ODE_SOLVER_H
#define AUTO_ODE_SOLVER_H

#include "../ode_solver.h"
#include "boost_ode_solvers.h"
#include "homemade_euler_ode_solver.h"
#include "../state_map.h"  // for state_vector_map

// A class representing the auto ode_solver which chooses default methods
template <class state_type>
class auto_ode_solver : public ode_solver
{
   public:
    auto_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps)
        : ode_solver("auto", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps),
          advanced_ode_solver(
              std::unique_ptr<ode_solver>(
                  new boost_rsnbrk_ode_solver(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))),
          euler_ode_solver(
              std::unique_ptr<ode_solver>(
                  new homemade_euler_ode_solver<state_type>(step_size, rel_error_tolerance, abs_error_tolerance, max_steps))) {}

   private:
    std::unique_ptr<ode_solver> advanced_ode_solver;
    std::unique_ptr<ode_solver> euler_ode_solver;

    bool advanced_ode_solver_most_recent;

    state_vector_map
    do_integrate(std::shared_ptr<dynamical_system> sys) override
    {
        // The `dynamical_system` does not require an Euler ode_solver, so use
        // the advanced ode_solver to integrate it
        advanced_ode_solver_most_recent = true;
        return advanced_ode_solver->integrate(sys);
    }

    state_vector_map
    handle_euler_requirement(std::shared_ptr<dynamical_system> sys) override
    {
        // The `dynamical_system` requires an Euler ode_solver, so use the Euler
        // ode_solver to integrate it
        advanced_ode_solver_most_recent = false;
        return euler_ode_solver->integrate(sys);
    }

    std::string get_param_info() const override
    {
        return std::string("\nThis ode_solver chooses between two defaults ") +
               std::string("depending on the type of system it integrates") +
               std::string("\node_solver used for dynamical systems that do ") +
               std::string("not require the Euler method:\n") +
               advanced_ode_solver->generate_info_report() +
               std::string("\node_solver used for dynamical systems that ") +
               std::string("require the Euler method:\n") +
               euler_ode_solver->generate_info_report();
    }

    std::string get_solution_info() const override
    {
        if (advanced_ode_solver_most_recent) {
            return std::string("The ode_solver for dynamical systems that do") +
                   std::string(" not require the Euler method was used.\n") +
                   std::string("It reports the following information:\n") +
                   advanced_ode_solver->generate_integrate_report();
        } else {
            return std::string("The ode_solver for dynamical systems that ") +
                   std::string("require the Euler method was used.\n") +
                   std::string("It reports the following information:\n") +
                   euler_ode_solver->generate_integrate_report();
        }
    }
};

#endif
