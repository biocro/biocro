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
		bool verbose,
		void (*print_msg) (char const *format, ...))
{
	// Create a system based on the inputs and store a smart pointer to it
	std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose, print_msg));
	
	// Check to see if it is compatible with adaptive step size methods
	bool is_adaptive_compatible = sys->is_adaptive_compatible();
	
	// Update the user and solve the system
	if(is_adaptive_compatible) {
		if(verbose) print_msg("Solving the system using the adaptive step size Rosenbrock method:\n\n");
		return Gro_rsnbrk_solve(sys, verbose, print_msg);
	}
	else {
		if(verbose) print_msg("Solving the system using the fixed step size Euler method:\n\n");
		return Gro_euler_solve(sys, verbose, print_msg);
	}
}

std::unordered_map<std::string, std::vector<double>> Gro_euler(
		std::unordered_map<std::string, double> const &initial_state,
		std::unordered_map<std::string, double> const &invariant_parameters,
		std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
		std::vector<std::string> const &steady_state_module_names,
		std::vector<std::string> const &derivative_module_names,
		bool verbose,
		void (*print_msg) (char const *format, ...))
{
	// Create a system based on the inputs and store a smart pointer to it
	std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose, print_msg));
	
	// Give the user an update if necessary
	if(verbose) print_msg("Solving the system using the fixed step size Euler method:\n\n");
	
	// Pass it to the Euler solver and return the result
	return Gro_euler_solve(sys, verbose, print_msg);
}

std::unordered_map<std::string, std::vector<double>> Gro_euler_solve(
		std::shared_ptr<System> sys,
		bool verbose,
		void (*print_msg) (char const *format, ...))
{
	// Solve the system with our own Euler implementation
	
	// Get the number of time points
	int ntimes = (int) sys->get_ntimes();
	
	// Get the names of the output parameters and pointers to them
	std::vector<std::string> output_param_vector = sys->get_output_param_names();
	std::vector<const double*> output_ptr_vector = sys->get_output_ptrs();
	
	// Make the results map
	std::unordered_map<std::string, std::vector<double>> results;
	
	// Make the result vector
	std::vector<double> temp(ntimes);
	std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);
	
	// Get the current state in the correct format
	std::vector<double> state;
	sys->get_state(state);
	
	// Make a vector to store the derivative
	std::vector<double> dstatedt = state;
	
	// Run through all the times
	double max_time = ntimes - 1.0;
	double threshold = 0.0;
	for(int t = 0; t < ntimes; t++) {
		// Give the user an update, if required
		if(verbose) {
			if(t >= max_time) print_msg("Timestep = %i (%f%% done) at clock = %u microseconds\n", t, t/max_time*100.0, (unsigned int) clock());
			else if(t/max_time >= threshold) {
				print_msg("Timestep = %i (%f%% done) at clock = %u microseconds\n", t, t/max_time*100.0, (unsigned int) clock());
				threshold += 0.02;
			}
		}
		
		// Update all the parameters and calculate the derivative based on the current time and state
		sys->operator()(state, dstatedt, t);
		
		// Store the current parameter values
		for(size_t i = 0; i < result_vec.size(); i++) (result_vec[i])[t] = *output_ptr_vector[i];
		
		// Update the state for the next step
		for(size_t j = 0; j < state.size(); j++) state[j] += dstatedt[j];	// The derivative has already been multiplied by the timestep
	}
	
	if(verbose) print_msg("\n");	// Make the output look nice
	
	// Fill in the result map and return it
	for(size_t i = 0; i < output_param_vector.size(); i++) results[output_param_vector[i]] = result_vec[i];
	return results;
}

std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk(
		std::unordered_map<std::string, double> const &initial_state,
		std::unordered_map<std::string, double> const &invariant_parameters,
		std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
		std::vector<std::string> const &steady_state_module_names,
		std::vector<std::string> const &derivative_module_names,
		bool verbose,
		void (*print_msg) (char const *format, ...))
{
	// Create a system based on the inputs and store a smart pointer to it
	std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose, print_msg));
	
	// Check to make sure the system can be solved using adaptive step size methods
	if(!sys->is_adaptive_compatible()) throw std::logic_error("Thrown by Gro_rsnbrk: the system is not compatible with adaptive step size integration methods.\n");
	
	// Give the user an update if necessary
	if(verbose) print_msg("Solving the system using the adaptive step size Rosenbrock method:\n\n");
	
	// Pass it to the Rosenbrock solver and return the result
	return Gro_rsnbrk_solve(sys, verbose, print_msg);
}

std::unordered_map<std::string, std::vector<double>> Gro_rsnbrk_solve(
	std::shared_ptr<System> sys,
	bool verbose,
	void (*print_msg) (char const *format, ...))
{
	// Solve the system using the ODEINT Rosenbrock stepper
	
	// Check to make sure the system can be solved using adaptive step size methods
	if(!sys->is_adaptive_compatible()) throw std::logic_error("Thrown by Gro_rsnbrk: the system is not compatible with adaptive step size integration methods.\n");
	
	// Get the number of time points
	int ntimes = (int) sys->get_ntimes();
	
	// Get the names of the output parameters and pointers to them
	std::vector<std::string> output_param_vector = sys->get_output_param_names();
	std::vector<const double*> output_ptr_vector = sys->get_output_ptrs();
	
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
			push_back_state_and_time_rsnbrk(state_vec, time_vec, (double)ntimes - 1.0, verbose, print_msg)
		);
	}
	catch (std::exception &e) {
		// Just return whatever progresss we have made
		if(verbose) print_msg("\n");	// Make the output look nice
		print_msg("Error encountered while running the ODEINT Rosenbrock stepper... returning a partial result.\n\nODEINT reports the following: %s\n\n", e.what());
		return sys->get_results(state_vec, time_vec);
	}
	
	if(verbose) print_msg("\n");	// Make the output look nice
	
	// Return the results
	return sys->get_results(state_vec, time_vec);
}