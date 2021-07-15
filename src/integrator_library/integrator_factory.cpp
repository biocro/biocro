#include "integrator_factory.h"

// Include all the header files that define the integrators.
#include "boost_integrators.h"
#include "homemade_euler_integrator.h"
#include "auto_integrator.h"

/**
 * @brief A function that returns a unique_ptr to a integrator object.
 */
template <class integrator_type>
std::unique_ptr<integrator> create_integrator(
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    return std::unique_ptr<integrator>(new integrator_type(
        step_size,
        rel_error_tolerance,
        abs_error_tolerance,
        max_steps));
}

integrator_factory::integrator_creator_map integrator_factory::integrator_creators =
    {
        {"auto",             create_integrator<auto_integrator<preferred_state_type>>},
        {"homemade_euler",   create_integrator<homemade_euler_integrator<preferred_state_type>>},
        {"boost_euler",      create_integrator<boost_euler_integrator<preferred_state_type>>},
        {"boost_rosenbrock", create_integrator<boost_rsnbrk_integrator>},
        {"boost_rk4",        create_integrator<boost_rk4_integrator<preferred_state_type>>},
        {"boost_rkck54",     create_integrator<boost_rkck54_integrator<preferred_state_type>>},
};

std::unique_ptr<integrator> integrator_factory::create(
    std::string const& integrator_name,
    double step_size,
    double rel_error_tolerance,
    double abs_error_tolerance,
    int max_steps)
{
    try {
        return integrator_factory::integrator_creators.at(integrator_name)(
            step_size,
            rel_error_tolerance,
            abs_error_tolerance,
            max_steps);
    } catch (std::out_of_range const&) {
        std::string message = std::string("\"") + integrator_name +
                              std::string("\"") +
                              std::string(" was given as a integrator name, ") +
                              std::string("but no integrator with that name could be found.\n.");

        throw std::out_of_range(message);
    }
}

string_vector integrator_factory::get_integrators()
{
    string_vector integrator_name_vector;
    for (auto const& x : integrator_factory::integrator_creators) {
        integrator_name_vector.push_back(x.first);
    }
    return integrator_name_vector;
}
