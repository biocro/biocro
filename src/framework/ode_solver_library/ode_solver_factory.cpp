#include "ode_solver_factory.h"

// Include all the header files that define the ode_solvers.
#include "boost_ode_solvers.h"
#include "homemade_euler_ode_solver.h"
#include "auto_ode_solver.h"

/**
 * @brief A function that returns a unique_ptr to a ode_solver object.
 */
template <class ode_solver_type>
ode_solver* create_ode_solver(
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    return new ode_solver_type(
        step_size,
        rel_error_tolerance,
        abs_error_tolerance,
        max_steps);
}

ode_solver_factory::ode_solver_creator_map ode_solver_factory::ode_solver_creators =
    {
        {"auto",             create_ode_solver<auto_ode_solver<preferred_state_type>>},
        {"homemade_euler",   create_ode_solver<homemade_euler_ode_solver<preferred_state_type>>},
        {"boost_euler",      create_ode_solver<boost_euler_ode_solver<preferred_state_type>>},
        {"boost_rosenbrock", create_ode_solver<boost_rsnbrk_ode_solver>},
        {"boost_rk4",        create_ode_solver<boost_rk4_ode_solver<preferred_state_type>>},
        {"boost_rkck54",     create_ode_solver<boost_rkck54_ode_solver<preferred_state_type>>},
};

ode_solver* ode_solver_factory::create(
    std::string const& ode_solver_name,
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    try {
        return ode_solver_factory::ode_solver_creators.at(ode_solver_name)(
            step_size,
            rel_error_tolerance,
            abs_error_tolerance,
            max_steps);
    } catch (std::out_of_range const&) {
        std::string message = std::string("\"") + ode_solver_name +
                              std::string("\"") +
                              std::string(" was given as a ode_solver name, ") +
                              std::string("but no ode_solver with that name could be found.\n.");

        throw std::out_of_range(message);
    }
}

string_vector ode_solver_factory::get_ode_solvers()
{
    string_vector ode_solver_name_vector;
    for (auto const& x : ode_solver_factory::ode_solver_creators) {
        ode_solver_name_vector.push_back(x.first);
    }
    return ode_solver_name_vector;
}
