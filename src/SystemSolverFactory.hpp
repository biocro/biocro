#ifndef SYSTEM_SOLVER_FACTORY_HPP
#define SYSTEM_SOLVER_FACTORY_HPP

#include <memory>
#include "system.h"
#include "system_solver.h"
 
 // Define a function that returns a pointer to a system solver that needs the following as inputs to its constructor:
 //  - output step size
 template <class solver_type, class state_type>
 inline std::unique_ptr<system_solver<state_type>> createSystemSolver(double output_step_size) {
     return std::unique_ptr<system_solver<state_type>>(new solver_type(output_step_size));
 }
 
 // Define a function that returns a pointer to a system solver that needs the following as inputs to its constructor:
 //  - output step size
 //  - adaptive error tolerance
 //  - adaptive max steps
 template <class solver_type, class state_type>
 inline std::unique_ptr<system_solver<state_type>> createSystemSolver(double output_step_size, double adaptive_error_tol, int adaptive_max_steps) {
     return std::unique_ptr<system_solver<state_type>>(new solver_type(output_step_size, adaptive_error_tol, adaptive_max_steps));
 }
 
 /*
 * We are passing names from R as strings, so we need a way to create objects from strings.
 * This is a factory that makes system solvers from strings.
 */

class SystemSolverFactory {
    public:
        typedef state_vector_map (*SYSTEM_SOLVER)(state_map const &, state_map const &, state_vector_map const &, std::vector<std::string> const &, std::vector<std::string> const &, double, double, int, bool, void (*)(char const*, ...));  // Typedef the function signature for a SystemSolver to the name SYSTEM_SOLVER.
        SYSTEM_SOLVER operator()(std::string const &system_solver_name) const;

    private:
        typedef std::map<std::string, SYSTEM_SOLVER> system_solver_map;  // A map of strings to SYSTEM_SOLVERs.
        static system_solver_map system_solvers;
};

extern SystemSolverFactory system_solver_factory;

#endif
