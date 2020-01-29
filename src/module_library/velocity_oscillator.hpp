#ifndef VELOCITY_OSCILLATOR_H
#define VELOCITY_OSCILLATOR_H

#include "../modules.h"

class velocity_oscillator : public DerivModule {
	public:
		velocity_oscillator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("velocity_oscillator"),
			// Get pointers to input parameters
			position_ip(get_ip(input_parameters, "position")),
			// Get pointers to output parameters
			velocity_op(get_op(output_parameters, "velocity"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* position_ip;
		// Pointers to output parameters
		double* velocity_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> velocity_oscillator::get_inputs() {
	return {
		"position"
	};
}

std::vector<std::string> velocity_oscillator::get_outputs() {
	return {
		"velocity"
	};
}

void velocity_oscillator::do_operation() const {
	// Update the output parameter list
	update(velocity_op, -(*position_ip));
}

#endif