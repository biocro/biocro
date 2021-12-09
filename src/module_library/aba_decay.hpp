#ifndef ABA_DECAY_H
#define ABA_DECAY_H

#include "../modules.h"
#include "../state_map.h"

class aba_decay : public differential_module {
	public:
		aba_decay(state_map const& input_quantities, state_map* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			differential_module("aba_decay"),
			// Get pointers to input quantities
			soil_aba_concentration_ip(get_ip(input_quantities, "soil_aba_concentration")),
			aba_decay_constant_ip(get_ip(input_quantities, "aba_decay_constant")),
			// Get pointers to output quantities
			soil_aba_concentration_op(get_op(output_quantities, "soil_aba_concentration"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
		const double* soil_aba_concentration_ip;
		const double* aba_decay_constant_ip;
		// Pointers to output quantities
		double* soil_aba_concentration_op;
		// Main operation
		void do_operation() const;
};

string_vector aba_decay::get_inputs() {
	return {
		"aba_decay_constant",
		"soil_aba_concentration"
	};
}

string_vector aba_decay::get_outputs() {
	return {
		"soil_aba_concentration"
	};
}

void aba_decay::do_operation() const {
	// Collect input quantities and make calculations
	double aba_decay_constant = *aba_decay_constant_ip;
	double soil_aba_concentration = *soil_aba_concentration_ip;

	double decay_rate = - aba_decay_constant * soil_aba_concentration;

	// Update the output quantity list
	update(soil_aba_concentration_op, decay_rate);
}

#endif
