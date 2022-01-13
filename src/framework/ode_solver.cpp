#include "ode_solver.h"

state_vector_map ode_solver::integrate(std::shared_ptr<dynamical_system> sys)
{
    integrate_method_has_been_called = true;

    if (should_check_euler_requirement && sys->requires_euler_ode_solver()) {
        return handle_euler_requirement(sys);
    } else {
        sys->reset_ncalls();
        return do_integrate(sys);
    }
}
