#include "integrator.h"

state_vector_map integrator::integrate(std::shared_ptr<dynamical_system> sys)
{
    integrate_method_has_been_called = true;

    if (should_check_euler_requirement && sys->requires_euler_integrator()) {
        return handle_euler_requirement(sys);
    } else {
        sys->reset_ncalls();
        return do_integrate(sys);
    }
}
