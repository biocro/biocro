#ifndef SYSTEM_SOLVER_FACTORY_HPP
#define SYSTEM_SOLVER_FACTORY_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../system_solver.h"

class system_solver_factory
{
   public:
    static std::unique_ptr<system_solver> create(
        std::string const& system_solver_name,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps);

    static std::vector<std::string> get_solvers();

   private:
    using system_solver_creator = std::unique_ptr<system_solver> (*)(double, double, double, int);  // Define a system_solver_creator to be a pointer to a function that has no arguments and returns a std::unique_ptr<system_solver>
    using system_solver_creator_map = std::map<std::string, system_solver_creator>;                 // A map of strings to system_solver_creators
    using preferred_state_type = std::vector<double>;                                               // A default value for state type
    static system_solver_creator_map system_solver_creators;
};

#endif
