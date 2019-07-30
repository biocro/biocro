#ifndef CIRCADIAN_CLOCK2_H
#define CIRCADIAN_CLOCK2_H

#include "../module.hpp"

class circadian_clock2 : public DerivModule {
	public:
		circadian_clock2(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("circadian_clock2"),
			// Get pointers to input parameters
			omega_ip(get_ip(input_parameters, "omega")),
			// Get pointers to output parameters
			phi_op(get_op(output_parameters, "phi"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* omega_ip;
		// Pointers to output parameters
		double* phi_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> circadian_clock2::get_inputs() {
	return {
		"omega"
	};
}

std::vector<std::string> circadian_clock2::get_outputs() {
	return {
		"phi"
	};
}

void circadian_clock2::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get omega
	double omega = *omega_ip;
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	update(phi_op, omega);
}

#endif
