#include "system_solver.h"

// Store some information that will be useful to any type of solver, and then call the private do_solve method
std::unordered_map<std::string, std::vector<double>> system_solver::solve(
    std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps)
{
    // Update the stored objects
    this->output_step_size = output_step_size;
    this->adaptive_error_tol = adaptive_error_tol;
    this->adaptive_max_steps = adaptive_max_steps;
    this->ntimes = sys->get_ntimes();

    // Solve the system and return the results
    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) {
        return this->handle_adaptive_incompatibility(sys);
    }

    else {
        return this->do_solve(sys);
    }
}