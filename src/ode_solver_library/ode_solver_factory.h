#ifndef ODE_SOLVER_FACTORY_H
#define ODE_SOLVER_FACTORY_H

#include <string>
#include <vector>
#include <map>
#include "../ode_solver.h"
#include "../state_map.h"  // for string_vector

class ode_solver_factory
{
   public:
    static ode_solver* create(
        std::string const& ode_solver_name,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps);

    static string_vector get_ode_solvers();

   private:
    // Define a ode_solver_creator to be a pointer to a function that
    // has no arguments and returns a pointer to an ode_solver
    using ode_solver_creator =
        ode_solver* (*)(double, double, double, int);

    // A map of strings to ode_solver_creators
    using ode_solver_creator_map =
        std::map<std::string, ode_solver_creator>;

    // A default value for state type
    using preferred_state_type = std::vector<double>;

    static ode_solver_creator_map ode_solver_creators;
};

#endif
