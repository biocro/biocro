#ifndef SYSTEM_SOLVER_FACTORY_HPP
#define SYSTEM_SOLVER_FACTORY_HPP

#include <memory>
#include "../system_solver.h"

// Define a function that returns a pointer to a system solver
template <class solver_type>
std::unique_ptr<system_solver> createSystemSolver()
{
    return std::unique_ptr<system_solver>(new solver_type());
}

// We are passing names from R as strings, so we need a way to create objects from strings.
// This is a factory that makes system solvers from strings.
class SystemSolverFactory
{
   public:
    std::unique_ptr<system_solver> operator()(std::string const& system_solver_name) const;

   private:
    using system_solver_creator = std::unique_ptr<system_solver> (*)();              // Define a system_solver_creator to be a pointer to a function that has no arguments and returns a std::unique_ptr<system_solver>
    using system_solver_creator_map = std::map<std::string, system_solver_creator>;  // A map of strings to system_solver_creators
    using preferred_state_type = std::vector<double>;                                // A default value for state type
    static system_solver_creator_map system_solver_creators;
};

extern SystemSolverFactory system_solver_factory;

#endif
