#include "Gro.h"
#include "system_solver.h"
#include "solver_library/SystemSolverFactory.hpp"

// General Gro function that uses an arbitrary solver
std::unordered_map<std::string, std::vector<double>> Gro_solve(
    std::unordered_map<std::string, double> const& initial_state,
    std::unordered_map<std::string, double> const& invariant_parameters,
    std::unordered_map<std::string, std::vector<double>> const& varying_parameters,
    std::vector<std::string> const& steady_state_module_names,
    std::vector<std::string> const& derivative_module_names,
    std::string solver_name,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    bool verbose,
    void (*print_msg)(char const* format, ...))
{
    // Create a system based on the inputs and store a smart pointer to it
    std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose, print_msg));

    // Create a solver
    std::unique_ptr<system_solver> solver = system_solver_factory(solver_name);

    // Solve the system and return the result
    return solver->solve(sys, output_step_size, adaptive_error_tol, adaptive_max_steps);
}
