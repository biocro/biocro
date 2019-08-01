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
		std::vector<std::string> const &derivative_module_names,
		bool verbose)
{
	// Start a timer
	clock_t ct = clock();
	
	// Create a system based on the intputs
	System sys(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose);
	
	// Get the current state in the correct format
	std::vector<double> state;
	sys.get_state(state);
	
	// Make a vector to store the derivative
	std::vector<double> dstatedt = state;
	
	// Get the number of time points
	int ntimes = (int) sys.get_ntimes();
	
	// Get the names of the output parameters and pointers to them
	std::vector<std::string> output_param_vector = sys.get_output_param_names();
	std::vector<const double*> output_ptr_vector = sys.get_output_ptrs();
	
	// Make the result vector
	std::vector<double> temp(ntimes);
	std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);
	
	// Run through all the times
	for(int t = 0; t < ntimes; t++) {
		// Store the current parameter values
		for(size_t i = 0; i < result_vec.size(); i++) (result_vec[i])[t] = *output_ptr_vector[i];
		
		// Calculate the derivative based on the current time and state
		sys(state, dstatedt, t);
		
		// Update the state
		for(size_t j = 0; j < state.size(); j++) state[j] += dstatedt[j];	// The derivative has already been multiplied by the timestep
	}
	
	// Make the result map
	std::unordered_map<std::string, std::vector<double>> results;
	for(size_t i = 0; i < output_param_vector.size(); i++) results[output_param_vector[i]] = result_vec[i];
	
	// End the timer
	ct = clock() - ct;
	Rprintf("\nGro.cpp required %u microseconds to run\n\n", (unsigned int) ct);
	
	// Return the results
	return results;
	
}
