#ifndef ABA_DECAY_H
#define ABA_DECAY_H

#include "../modules.h"

class aba_decay : public DerivModule {
	public:
		aba_decay(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("aba_decay"),
			// Get pointers to input parameters
			soil_aba_concentration_ip(get_ip(input_parameters, "soil_aba_concentration")),
			aba_decay_constant_ip(get_ip(input_parameters, "aba_decay_constant")),
			// Get pointers to output parameters
			soil_aba_concentration_op(get_op(output_parameters, "soil_aba_concentration"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_aba_concentration_ip;
		const double* aba_decay_constant_ip;
		// Pointers to output parameters
		double* soil_aba_concentration_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> aba_decay::get_inputs() {
	return {
		"aba_decay_constant",
		"soil_aba_concentration"
	};
}

std::vector<std::string> aba_decay::get_outputs() {
	return {
		"soil_aba_concentration"
	};
}

void aba_decay::do_operation() const {
	// Collect input parameters and make calculations
	double aba_decay_constant = *aba_decay_constant_ip;
	double soil_aba_concentration = *soil_aba_concentration_ip;
	
	double decay_rate = - aba_decay_constant * soil_aba_concentration;
	
	// Update the output parameter list
	update(soil_aba_concentration_op, decay_rate);
}

#endif
