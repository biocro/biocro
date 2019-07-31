/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include "Gro.h"

std::unordered_map<std::string, std::vector<double>> Gro(
        std::unordered_map<std::string, double> const &initial_state,
        std::unordered_map<std::string, double> const &invariant_parameters,
        std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
        std::vector<std::string> const &steady_state_module_names,
		std::vector<std::string> const &derivative_module_names)
{
	// Create a system based on the intputs, using the verbose option	
	System sys(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, true);
	
	// Get the current state in the correct format
	std::vector<double> state;
	sys.get_state(state);
	
	// Initialize the time
	int time = 0;
	
	// Make vectors to store the model output
	std::vector<std::vector<double>> state_vec;
	std::vector<int> time_vec;
	
	// Store the initial state and time
	state_vec.push_back(state);
	time_vec.push_back(time);
	
	// Return the output
	return sys.get_results(state_vec, time_vec);
}
