#ifndef NUMERICAL_INTEGRATOR_FACTORY_H
#define NUMERICAL_INTEGRATOR_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../numerical_integrator.h"
#include "../state_map.h" // for string_vector

class numerical_integrator_factory
{
   public:
    static std::unique_ptr<numerical_integrator> create(
        std::string const& numerical_integrator_name,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps);

    static string_vector get_numerical_integrators();

   private:
    // Define a numerical_integrator_creator to be a pointer to a function that
    // has no arguments and returns a std::unique_ptr<numerical_integrator>
    using numerical_integrator_creator =
        std::unique_ptr<numerical_integrator> (*)(double, double, double, int);

    // A map of strings to numerical_integrator_creators
    using numerical_integrator_creator_map =
        std::map<std::string, numerical_integrator_creator>;

    // A default value for state type
    using preferred_state_type = std::vector<double>;

    static numerical_integrator_creator_map numerical_integrator_creators;
};

#endif
