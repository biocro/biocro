#include "numerical_integrator_factory.h"

// Include all the header files that define the numerical_integrators.
#include "boost_numerical_integrators.h"
#include "homemade_euler_numerical_integrator.h"
#include "auto_numerical_integrator.h"

/**
 * @brief A function that returns a unique_ptr to a numerical_integrator object.
 */
template <class numerical_integrator_type>
std::unique_ptr<numerical_integrator> create_numerical_integrator(
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    return std::unique_ptr<numerical_integrator>(new numerical_integrator_type(
        step_size,
        rel_error_tolerance,
        abs_error_tolerance,
        max_steps));
}

numerical_integrator_factory::numerical_integrator_creator_map numerical_integrator_factory::numerical_integrator_creators =
    {
        {"auto",             create_numerical_integrator<auto_numerical_integrator<preferred_state_type>>},
        {"homemade_euler",   create_numerical_integrator<homemade_euler_numerical_integrator<preferred_state_type>>},
        {"boost_euler",      create_numerical_integrator<boost_euler_numerical_integrator<preferred_state_type>>},
        {"boost_rosenbrock", create_numerical_integrator<boost_rsnbrk_numerical_integrator>},
        {"boost_rk4",        create_numerical_integrator<boost_rk4_numerical_integrator<preferred_state_type>>},
        {"boost_rkck54",     create_numerical_integrator<boost_rkck54_numerical_integrator<preferred_state_type>>},
};

std::unique_ptr<numerical_integrator> numerical_integrator_factory::create(
    std::string const& numerical_integrator_name,
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    try {
        return numerical_integrator_factory::numerical_integrator_creators.at(numerical_integrator_name)(
            step_size,
            rel_error_tolerance,
            abs_error_tolerance,
            max_steps);
    } catch (std::out_of_range) {
        std::string message = std::string("\"") + numerical_integrator_name +
                              std::string("\"") +
                              std::string(" was given as a numerical_integrator name, ") +
                              std::string("but no numerical_integrator with that name could be found.\n.");

        throw std::out_of_range(message);
    }
}

string_vector numerical_integrator_factory::get_numerical_integrators()
{
    string_vector numerical_integrator_name_vector;
    for (auto const& x : numerical_integrator_factory::numerical_integrator_creators) {
        numerical_integrator_name_vector.push_back(x.first);
    }
    return numerical_integrator_name_vector;
}
