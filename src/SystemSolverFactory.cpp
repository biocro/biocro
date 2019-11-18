#include "SystemSolverFactory.hpp"
#include "Gro.h"


SystemSolverFactory::system_solver_map SystemSolverFactory::system_solvers = {
    { "Gro",                Gro },
    { "Gro_euler",          Gro_euler },
    { "Gro_euler_odeint",   Gro_euler_odeint },
    { "Gro_rsnbrk",         Gro_rsnbrk },
    { "Gro_rk4",            Gro_rk4 },
    { "Gro_rkck54",         Gro_rkck54 },
};

SystemSolverFactory::SYSTEM_SOLVER SystemSolverFactory::operator()(std::string const &system_solver_name) const {
    try {
        return this->system_solvers.at(system_solver_name);
    } catch (std::out_of_range const &oor) {
        throw std::out_of_range(std::string("\"") + system_solver_name + std::string("\"") + std::string(" was given as a SYSTEM_SOLVER name, but no SYSTEM_SOLVER with that name could be found.\n."));
    }
}

SystemSolverFactory system_solver_factory;
