#include "system_solver.h"

// Store some information that will be useful to any type of solver, and then call the private do_solve method
std::unordered_map<std::string, std::vector<double>> system_solver::solve(
    std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps)
{
    // Update the stored objects
    set_solver_parameters(output_step_size, adaptive_error_tol, adaptive_max_steps);

    return solve(sys);
}

std::unordered_map<std::string, std::vector<double>> system_solver::solve(
    std::shared_ptr<System> sys)
{
    // Solve the system and return the results
    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) {
        return handle_adaptive_incompatibility(sys);
    }

    else {
        return do_solve(sys);
    }
}
