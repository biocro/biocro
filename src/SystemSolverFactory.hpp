#ifndef SYSTEM_SOLVER_FACTORY_HPP
#define SYSTEM_SOLVER_FACTORY_HPP

#include <memory>
#include "system.h"

class SystemSolverFactory {
    private:
        typedef state_vector_map (*SYSTEM_SOLVER)(state_map const &, state_map const &, state_vector_map const &, std::vector<std::string> const &, std::vector<std::string> const &, bool, void (*)(char const*, ...));  // Typedef the function signature for a SystemSolver to the name SYSTEM_SOLVER.
        typedef std::map<std::string, SYSTEM_SOLVER> system_solver_map;  // A map of strings to SYSTEM_SOLVERs.
        static system_solver_map system_solvers;

    public:
        SYSTEM_SOLVER operator()(std::string const &system_solver_name) const;
};

extern SystemSolverFactory system_solver_factory;

#endif
