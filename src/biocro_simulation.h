#ifndef BIOCRO_SIMULATION_H
#define BIOCRO_SIMULATION_H

#include <vector>
#include "state_map.h"
#include "system.h"
#include "system_solver.h"
#include "solver_library/SystemSolverFactory.hpp"

// Class that represents a BioCro simulation
class biocro_simulation
{
   public:
    biocro_simulation(
        std::unordered_map<std::string, double> const& initial_state,
        std::unordered_map<std::string, double> const& invariant_parameters,
        std::unordered_map<std::string, std::vector<double>> const& varying_parameters,
        std::vector<std::string> const& steady_state_module_names,
        std::vector<std::string> const& derivative_module_names,
        std::string solver_name,
        double output_step_size,
        double adaptive_rel_error_tol,
        double adaptive_abs_error_tol,
        int adaptive_max_steps)
    {
        // Create the system
        sys = std::shared_ptr<System>(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names));

        // Create the solver
        solver = system_solver_factory(solver_name, output_step_size, adaptive_rel_error_tol, adaptive_abs_error_tol, adaptive_max_steps);
    }

    std::unordered_map<std::string, std::vector<double>> run_simulation() { return solver->solve(sys); }

    std::string generate_report() const
    {
        return std::string("\nSystem startup information:\n") + sys->generate_startup_report() +
               std::string("\nSolver description:\n") + solver->generate_info_report() +
               std::string("\n\nThe solver reports the following:\n") + solver->generate_solve_report() +
               std::string("\nThe system reports the following:\n") + sys->generate_usage_report() + std::string("\n\n");
    }

   private:
    std::shared_ptr<System> sys;
    std::unique_ptr<system_solver> solver;
};

#endif
