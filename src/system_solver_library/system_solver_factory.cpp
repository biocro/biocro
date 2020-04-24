#include "system_solver_factory.h"

// Include all the header files that define the system_solvers.
#include "boost_solvers.h"
#include "homemade_euler.h"
#include "auto_solver.h"

/**
 * @brief A function that returns a unique_ptr to a system_solver object.
 */
template <class solver_type>
std::unique_ptr<system_solver> create_system_solver(
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    return std::unique_ptr<system_solver>(new solver_type(step_size, rel_error_tolerance, abs_error_tolerance, max_steps));
}

system_solver_factory::system_solver_creator_map system_solver_factory::system_solver_creators =
{
    { "Gro",                create_system_solver<auto_solver<preferred_state_type>> },
    { "Gro_euler",          create_system_solver<homemade_euler_solver<preferred_state_type>> },
    { "Gro_euler_odeint",   create_system_solver<boost_euler_system_solver<preferred_state_type>> },
    { "Gro_rsnbrk",         create_system_solver<boost_rsnbrk_system_solver> },
    { "Gro_rk4",            create_system_solver<boost_rk4_system_solver<preferred_state_type>> },
    { "Gro_rkck54",         create_system_solver<boost_rkck54_system_solver<preferred_state_type>> },
};

std::unique_ptr<system_solver> system_solver_factory::create(
    std::string const& system_solver_name,
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    try {
        return system_solver_factory::system_solver_creators.at(system_solver_name)(step_size, rel_error_tolerance, abs_error_tolerance, max_steps);
    } catch (std::out_of_range) {
        std::string message = std::string("\"") + system_solver_name +
                       std::string("\"") +
                       std::string(" was given as a system_solver name, ") +
                       std::string("but no system_solver with that name could be found.\n.");

        throw std::out_of_range(message);
    }
}

std::vector<std::string> system_solver_factory::get_solvers()
{
    std::vector<std::string> solver_name_vector;
    for (auto const& x : system_solver_creators) {
        solver_name_vector.push_back(x.first);
    }
    return solver_name_vector;
}
