#include "se_solver_factory.h"

// Include all the header files that define the se_solvers.
#include "fixed_point.h"
#include "newton_raphson_boost.h"

/**
 * @brief A function that returns a unique_ptr to an se_solver object.
 */
template <class solver_type>
std::unique_ptr<se_solver> create_se_solver(
    double rel_error_tol,
    double abs_error_tol,
    int max_it)
{
    return std::unique_ptr<se_solver>(new solver_type(rel_error_tol, abs_error_tol, max_it));
}

se_solver_factory::se_solver_creator_map se_solver_factory::se_solver_creators =
{
    { "fixed_point",            create_se_solver<fixed_point> },
    { "newton_raphson_boost",   create_se_solver<newton_raphson_boost> }
};

std::unique_ptr<se_solver> se_solver_factory::create(
    std::string const& se_solver_name,
    double rel_error_tol,
    double abs_error_tol,
    int max_it)
{
    try {
        return se_solver_factory::se_solver_creators.at(se_solver_name)(rel_error_tol, abs_error_tol, max_it);
    } catch (std::out_of_range) {
        std::string message = std::string("\"") + se_solver_name +
                       std::string("\"") +
                       std::string(" was given as an se_solver name, ") +
                       std::string("but no se_solver with that name could be found.\n.");

        throw std::out_of_range(message);
    }
}

std::vector<std::string> se_solver_factory::get_solvers()
{
    std::vector<std::string> solver_name_vector;
    for (auto const& x : se_solver_creators) {
        solver_name_vector.push_back(x.first);
    }
    return solver_name_vector;
}