#ifndef GRO_H
#define GRO_H

#include <vector>
#include "state_map.h"
#include "system.h"

std::unordered_map<std::string, std::vector<double>> Gro(
        std::unordered_map<std::string, double> const &initial_state,
        std::unordered_map<std::string, double> const &invariant_parameters,
        std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
        std::vector<std::string> const &steady_state_module_names,
		std::vector<std::string> const &derivative_module_names);

#endif
