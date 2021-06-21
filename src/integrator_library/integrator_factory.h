#ifndef INTEGRATOR_FACTORY_H
#define INTEGRATOR_FACTORY_H

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../integrator.h"
#include "../state_map.h" // for string_vector

class integrator_factory
{
   public:
    static std::unique_ptr<integrator> create(
        std::string const& integrator_name,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps);

    static string_vector get_integrators();

   private:
    // Define a integrator_creator to be a pointer to a function that
    // has no arguments and returns a std::unique_ptr<integrator>
    using integrator_creator =
        std::unique_ptr<integrator> (*)(double, double, double, int);

    // A map of strings to integrator_creators
    using integrator_creator_map =
        std::map<std::string, integrator_creator>;

    // A default value for state type
    using preferred_state_type = std::vector<double>;

    static integrator_creator_map integrator_creators;
};

#endif
