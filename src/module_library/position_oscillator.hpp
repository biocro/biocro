#ifndef POSITION_OSCILLATOR_H
#define POSITION_OSCILLATOR_H

#include "../modules.h"

class position_oscillator : public DerivModule {
	public:
		position_oscillator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("position_oscillator"),
			// Get pointers to input parameters
			velocity_ip(get_ip(input_parameters, "velocity")),
			// Get pointers to output parameters
			position_op(get_op(output_parameters, "position"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* velocity_ip;
		// Pointers to output parameters
		double* position_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> position_oscillator::get_inputs() {
	return {
		"velocity"
	};
}

std::vector<std::string> position_oscillator::get_outputs() {
	return {
		"position"
	};
}

void position_oscillator::do_operation() const {
	// Update the output parameter list
	update(position_op, *velocity_ip);
}

#endif