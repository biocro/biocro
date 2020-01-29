#include "system_solver.h"

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
