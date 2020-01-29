#ifndef AUTO_SOLVER_H
#define AUTO_SOLVER_H

#include "../system_solver.h"
#include "boost_solvers.h"
#include "homemade_euler.h"

// A class representing the auto solver which chooses default methods
template <class state_type>
class auto_solver : public system_solver
{
   public:
    auto_solver() : system_solver("auto", true),
                    adaptive_solver(std::unique_ptr<system_solver>(new boost_rsnbrk_system_solver())),
                    other_solver(std::unique_ptr<system_solver>(new homemade_euler_solver<state_type>())) {}

   private:
    std::unique_ptr<system_solver> adaptive_solver;
    std::unique_ptr<system_solver> other_solver;

    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys) override
    {
        // The system is compatible with adaptive step size methods, so use the adaptive solver to solve the system
        adaptive_solver->set_solver_parameters(get_output_step_size(), get_adaptive_error_tol(), get_adaptive_max_steps());
        return adaptive_solver->solve(sys);
    }

    std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(std::shared_ptr<System> sys) override
    {
        // The system is not compatible with adaptive step size methods, so use the other solver to solve the system
        other_solver->set_solver_parameters(get_output_step_size(), get_adaptive_error_tol(), get_adaptive_max_steps());
        return other_solver->solve(sys);
    }

    std::string get_param_info() const override { return std::string("Solver used for adaptive systems: ") + adaptive_solver->generate_info_report() + std::string("\nSolver used for other systems: ") + other_solver->generate_info_report(); }
};

#endif
