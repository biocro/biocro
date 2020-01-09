#ifndef GRO_H
#define GRO_H

#include <vector>
#include "state_map.h"
#include "system.h"
#include "solver_library/SystemSolverFactory.hpp"
#include "system_solver.h"

// General Gro function that uses an arbitrary solver
std::unordered_map<std::string, std::vector<double>> Gro_solve(
    std::unordered_map<std::string, double> const& initial_state,
    std::unordered_map<std::string, double> const& invariant_parameters,
    std::unordered_map<std::string, std::vector<double>> const& varying_parameters,
    std::vector<std::string> const& steady_state_module_names,
    std::vector<std::string> const& derivative_module_names,
    std::string solver_name,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    bool verbose,
    void (*print_msg)(char const* format, ...) = void_printf);

#endif
