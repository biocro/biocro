#ifndef GAMMA_OSCILLATOR_H
#define GAMMA_OSCILLATOR_H

#include "../modules.h"

class gamma_oscillator : public DerivModule {
	public:
		gamma_oscillator(const std::unordered_map<std::string, double>* /*input_parameters*/, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("gamma_oscillator"),
			// Get pointers to input parameters
			//  ...but, there aren't any!
			// Get pointers to output parameters
			gamma_op(get_op(output_parameters, "gamma"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		//  ...but, there aren't any!
		// Pointers to output parameters
		double* gamma_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> gamma_oscillator::get_inputs() {
	return {
	};
}

std::vector<std::string> gamma_oscillator::get_outputs() {
	return {
		"gamma"
	};
}

void gamma_oscillator::do_operation() const {
	// Update the output parameter list
	update(gamma_op, 0.0);
}

#endif