#include "system_solver.h"

state_vector_map system_solver::solve(std::shared_ptr<System> sys)
{
    solve_method_has_been_called = true;

    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) {
        return handle_adaptive_incompatibility(sys);
    } else {
        sys->reset_ncalls();
        return do_solve(sys);
    }
}
