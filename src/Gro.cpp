/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2015 lower and upper temp contributed by Deepak Jaiswal,
 *  nitroparms also contributed by Deepak Jaiswal
 *
 */

#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include "modules.h"
#include "system.h"
#include "Gro.h"
#include "math.h"

using std::vector;
using std::string;
using std::unique_ptr;

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::vector<std::string> const &steady_state_module_names,
		std::vector<std::string> const &derivative_module_names)
{
	// Create a system based on the intputs, using the verbose option	
	System sys(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, true);
	
	// Get the current state in the correct format
	state_type state;
	sys.get_state(state);
	
	// Initialize the time
	time_type time = (time_type) 0;
	
	// Make vectors to store the model output
	std::vector<state_type> state_vec;
	std::vector<time_type> time_vec;
	
	// Store the initial state and time
	state_vec.push_back(state);
	time_vec.push_back(time);
	
	// Return the output
	return sys.get_results(state_vec, time_vec);
}
