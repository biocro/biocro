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
    auto_solver() : system_solver("auto", true) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys) override
    {
        // The system is compatible with adaptive step size methods, so use a rosenbrock solver to solve the system
        boost_rsnbrk_system_solver solver;
        return solver.solve(sys, this->output_step_size, this->adaptive_error_tol, this->adaptive_max_steps);
    }

    std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(std::shared_ptr<System> sys) override
    {
        // The system is not compatible with adaptive step size methods, so use an euler solver to solve the system
        homemade_euler_solver<state_type> solver;
        return solver.solve(sys, this->output_step_size, this->adaptive_error_tol, this->adaptive_max_steps);
    }
};

#endif
