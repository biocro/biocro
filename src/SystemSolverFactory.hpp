#ifndef SYSTEM_SOLVER_FACTORY_HPP
#define SYSTEM_SOLVER_FACTORY_HPP

#include <memory>
#include "system.h"

/*
 * We are passing names from R as strings, so we need a way to create objects from strings.
 * This is a factory that makes system solvers from strings.
 * For this context, a system solver is any function with the following signature:
 * state_vector_map f(state_map const &,
 *                    state_map const &,
 *                    state_vector_map const &,
 *                    vector<string> const &,
 *                    vector<string> const &,
 *                    bool,
 *                    void (*)(char const*, ...))
 */

class SystemSolverFactory {
    public:
        typedef state_vector_map (*SYSTEM_SOLVER)(state_map const &, state_map const &, state_vector_map const &, std::vector<std::string> const &, std::vector<std::string> const &, bool, void (*)(char const*, ...));  // Typedef the function signature for a SystemSolver to the name SYSTEM_SOLVER.
        SYSTEM_SOLVER operator()(std::string const &system_solver_name) const;

    private:
        typedef std::map<std::string, SYSTEM_SOLVER> system_solver_map;  // A map of strings to SYSTEM_SOLVERs.
        static system_solver_map system_solvers;
};

extern SystemSolverFactory system_solver_factory;

#endif
