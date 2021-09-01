#include "dynamical_system_helper_functions.h"

/**
 * @brief Checks whether the collection of modules requires an Euler ODE solver.
 */
bool check_euler_requirement(module_vector const& modules_to_check)
{
    int num_requiring_euler{0};

    for (auto const& x : modules_to_check) {
        num_requiring_euler += x->requires_euler_ode_solver();
    }

    return num_requiring_euler > 0;
}
