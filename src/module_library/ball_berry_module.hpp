#ifndef BALL_BERRY_MODULE_H
#define BALL_BERRY_MODULE_H

#include "../modules.h"
#include "ball_berry.hpp"

class ball_berry_module : public SteadyModule {
	public:
		ball_berry_module(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("ball_berry_module"),
			// Get pointers to input parameters
			net_assimilation_rate_ip(get_ip(input_parameters, "net_assimilation_rate")),
			atmospheric_co2_concentration_ip(get_ip(input_parameters, "atmospheric_co2_concentration")),
			rh_ip(get_ip(input_parameters, "rh")),
			b0_ip(get_ip(input_parameters, "b0")),
			b1_ip(get_ip(input_parameters, "b1")),
			// Get pointers to output parameters
			leaf_stomatal_conductance_op(get_op(output_parameters, "leaf_stomatal_conductance"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* net_assimilation_rate_ip;
		const double* atmospheric_co2_concentration_ip;
		const double* rh_ip;
		const double* b0_ip;
		const double* b1_ip;
		// Pointers to output parameters
		double* leaf_stomatal_conductance_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> ball_berry_module::get_inputs() {
	return {
		"net_assimilation_rate",
		"atmospheric_co2_concentration",
		"rh",
		"b0",
		"b1"
	};
}

std::vector<std::string> ball_berry_module::get_outputs() {
	return {
		"leaf_stomatal_conductance"
	};
}

void ball_berry_module::do_operation() const {
	// Collect input parameters and make calculations
	double net_assimilation_rate = *net_assimilation_rate_ip;
	double atmospheric_co2_concentration = *atmospheric_co2_concentration_ip;
	double rh = *rh_ip;
	double b0 = *b0_ip;
	double b1 = *b1_ip;
	
	double stomatal_conductance = ball_berry(net_assimilation_rate, atmospheric_co2_concentration, rh, b0, b1);
	
	// Update the output parameter list
	update(leaf_stomatal_conductance_op, stomatal_conductance);
}

#endif
