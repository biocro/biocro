#ifndef BALL_BERRY_MODULE_H
#define BALL_BERRY_MODULE_H

#include "../modules.h"
#include "../state_map.h"
#include "ball_berry.hpp"

class ball_berry_module : public direct_module {
	public:
		ball_berry_module(state_map const& input_quantities, state_map* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			direct_module("ball_berry_module"),
			// Get pointers to input quantities
			net_assimilation_rate_ip(get_ip(input_quantities, "net_assimilation_rate")),
			atmospheric_co2_concentration_ip(get_ip(input_quantities, "atmospheric_co2_concentration")),
			rh_ip(get_ip(input_quantities, "rh")),
			b0_ip(get_ip(input_quantities, "b0")),
			b1_ip(get_ip(input_quantities, "b1")),
			// Get pointers to output quantities
			leaf_stomatal_conductance_op(get_op(output_quantities, "leaf_stomatal_conductance"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
		const double* net_assimilation_rate_ip;
		const double* atmospheric_co2_concentration_ip;
		const double* rh_ip;
		const double* b0_ip;
		const double* b1_ip;
		// Pointers to output quantities
		double* leaf_stomatal_conductance_op;
		// Main operation
		void do_operation() const;
};

string_vector ball_berry_module::get_inputs() {
	return {
		"net_assimilation_rate",
		"atmospheric_co2_concentration",
		"rh",
		"b0",
		"b1"
	};
}

string_vector ball_berry_module::get_outputs() {
	return {
		"leaf_stomatal_conductance"
	};
}

void ball_berry_module::do_operation() const {
	// Collect input quantities and make calculations
	double net_assimilation_rate = *net_assimilation_rate_ip;
	double atmospheric_co2_concentration = *atmospheric_co2_concentration_ip;
	double rh = *rh_ip;
	double b0 = *b0_ip;
	double b1 = *b1_ip;

	double stomatal_conductance = ball_berry(net_assimilation_rate, atmospheric_co2_concentration, rh, b0, b1);

	// Update the output quantity list
	update(leaf_stomatal_conductance_op, stomatal_conductance);
}

#endif
