#ifndef CLOCK_TESTING_H
#define CLOCK_TESTING_H

#include "../module.hpp"

class clock_testing : public DerivModule {
	public:
		clock_testing(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("clock_testing"),
			// Get pointers to input parameters
			gamma_ip(get_ip(input_parameters, "gamma")),
			x_ip(get_ip(input_parameters, "x")),
			v_ip(get_ip(input_parameters, "v")),
			theta_ip(get_ip(input_parameters, "theta")),
			E_ip(get_ip(input_parameters, "E")),
			F_ip(get_ip(input_parameters, "F")),
			// Get pointers to output parameters
			x_op(get_op(output_parameters, "x")),
			v_op(get_op(output_parameters, "v")),
			theta_op(get_op(output_parameters, "theta")),
			E_op(get_op(output_parameters, "E"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* gamma_ip;
		const double* x_ip;
		const double* v_ip;
		const double* theta_ip;
		const double* E_ip;
		const double* F_ip;
		// Pointers to output parameters
		double* x_op;
		double* v_op;
		double* theta_op;
		double* E_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> clock_testing::get_inputs() {
	return {
		"gamma",
		"x",
		"v",
		"theta",
		"E",
		"F"
	};
}

std::vector<std::string> clock_testing::get_outputs() {
	return {
		"x",
		"v",
		"theta",
		"E"
	};
}

void clock_testing::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get the damping parameter
	double gamma = *gamma_ip;
	
	// Get the current oscillator state
	double x = *x_ip;
	double v = *v_ip;
	
	// Get the current oscillator state in phase coordinates
	double theta = *theta_ip;
	double E = *E_ip;
	
	// Get the force value
	double F = *F_ip;
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	/*
	// This version is the "magical mass on a spring"
	update(x_op, v);
	update(v_op, -x - gamma * v * (x * x + v * v - 1.0) + F);
	
	update(E_op, 2.0 * gamma * E * (1.0 - E) * sin(theta) * sin(theta));
	update(theta_op, gamma * (1.0 - E) * sin(theta) * cos(theta) - 1.0);
	*/
	
	/*
	// This version is a Poincare oscillator
	update(x_op, -0.5 * gamma * x * (x * x + v * v - 1.0) + v);
	update(v_op, -0.5 * gamma * v * (x * x + v * v - 1.0) - x + F);
	
	update(E_op, gamma * E * (1.0 - E));
	update(theta_op, -1.0);
	*/
	
	// This version is a different rectangular projection of a Poincare oscillator
	update(x_op, - gamma * x * (sqrt(x * x + v * v) - 1.0) + v);
	update(v_op, - gamma * v * (sqrt(x * x + v * v) - 1.0) - x + F);
	
	update(E_op, gamma * E * (1.0 - E));
	update(theta_op, -1.0);
}

#endif
