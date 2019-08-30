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
	// Create a system based on the inputs and store a smart pointer to it
	std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose));
	
	// Get the number of time points
	int ntimes = (int) sys->get_ntimes();
	
	// Get the names of the output parameters and pointers to them
	std::vector<std::string> output_param_vector = sys->get_output_param_names();
	std::vector<const double*> output_ptr_vector = sys->get_output_ptrs();
	
	// Make the results map
	std::unordered_map<std::string, std::vector<double>> results;
	
	// Check to see if it is compatible with adaptive step size methods
	bool is_adaptive_compatible = sys->is_adaptive_compatible();
	
	// Solve the system
	if(!is_adaptive_compatible) {
		// In this case, we must use the fixed-step Euler method
		
		// Make the result vector
		std::vector<double> temp(ntimes);
		std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);
		
		// Get the current state in the correct format
		std::vector<double> state;
		sys->get_state(state);
		
		// Make a vector to store the derivative
		std::vector<double> dstatedt = state;
		
		// Run through all the times
		for(int t = 0; t < ntimes; t++) {
			// Store the current parameter values
			for(size_t i = 0; i < result_vec.size(); i++) (result_vec[i])[t] = *output_ptr_vector[i];
			
			// Calculate the derivative based on the current time and state
			sys->operator()(state, dstatedt, t);
			
			// Update the state
			for(size_t j = 0; j < state.size(); j++) state[j] += dstatedt[j];	// The derivative has already been multiplied by the timestep
		}
		
		// Make the result map
		for(size_t i = 0; i < output_param_vector.size(); i++) results[output_param_vector[i]] = result_vec[i];
	}
	else {
		// In this case, we can use the Rosenbrock method
		
		// Get the current state in the correct format
		boost::numeric::ublas::vector<double> state;
		sys->get_state(state);
		
		// Make a vector to store the derivative
		boost::numeric::ublas::vector<double> dstatedt = state;
		
		// Make a system caller to pass to odeint
		SystemCaller syscall(sys);
		
		// Set the error tolerance to use for the odeint calculation
		double error_tol = 1.0e-4;
		
		// Set the step size to use for the odeint calculation
		double step_size = 1.0;
		
		// Make vectors to store the observer output
		std::vector<boost::numeric::ublas::vector<double>> state_vec;
		std::vector<double> time_vec;
		
		// Run the calculation and get the results
		double abs_err = error_tol, rel_err = error_tol;
		typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
		try {
			boost::numeric::odeint::integrate_const(
				boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
				std::make_pair(syscall, syscall),
				state,
				0.0,
				(double)ntimes - 1.0,
				step_size,
				push_back_state_and_time_rsnbrk(state_vec, time_vec, (double)ntimes - 1.0, verbose)
			);
		}
		catch (std::exception &e) {
			// Just save whatever progresss we have made
			if(verbose) Rprintf("Error encountered while running ODEINT: %s\n", e.what());
			results = sys->get_results(state_vec, time_vec);
		}
		results = sys->get_results(state_vec, time_vec);
	}
	
	// Make the output look nice
	if(verbose) Rprintf("\n\n");
	
	// Return the results
	return results;
	
}