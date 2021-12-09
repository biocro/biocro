#ifndef FVCB_H
#define FVCB_H

#include "../modules.h"
#include "../state_map.h"

class FvCB : public direct_module {
	public:
		FvCB(state_map const& input_quantities, state_map* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			direct_module("FvCB"),
			// Get pointers to input quantities
			Ci_ip(get_ip(input_quantities, "Ci")),
			Gstar_ip(get_ip(input_quantities, "Gstar")),
			Vcmax_ip(get_ip(input_quantities, "Vcmax")),
			Kc_ip(get_ip(input_quantities, "Kc")),
			O2_ip(get_ip(input_quantities, "O2")),
			Ko_ip(get_ip(input_quantities, "Ko")),
			J_ip(get_ip(input_quantities, "J")),
			electrons_per_carboxylation_ip(get_ip(input_quantities, "electrons_per_carboxylation")),
			electrons_per_oxygenation_ip(get_ip(input_quantities, "electrons_per_oxygenation")),
			maximum_tpu_rate_ip(get_ip(input_quantities, "maximum_tpu_rate")),
			Rd_ip(get_ip(input_quantities, "Rd")),
			Ca_ip(get_ip(input_quantities, "Ca")),
			atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
			leaf_stomatal_conductance_ip(get_ip(input_quantities, "leaf_stomatal_conductance")),
			// Get pointers to output quantities
			carboxylation_rate_op(get_op(output_quantities, "carboxylation_rate")),
			net_assimilation_rate_op(get_op(output_quantities, "net_assimilation_rate")),
			Ci_op(get_op(output_quantities, "Ci"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
		const double* Ci_ip;
		const double* Gstar_ip;
		const double* Vcmax_ip;
		const double* Kc_ip;
		const double* O2_ip;
		const double* Ko_ip;
		const double* J_ip;
		const double* electrons_per_carboxylation_ip;
		const double* electrons_per_oxygenation_ip;
		const double* maximum_tpu_rate_ip;
		const double* Rd_ip;
		const double* Ca_ip;
		const double* atmospheric_pressure_ip;
		const double* leaf_stomatal_conductance_ip;
		// Pointers to output quantities
		double* carboxylation_rate_op;
		double* net_assimilation_rate_op;
		double* Ci_op;
		// Main operation
		void do_operation() const;
};

string_vector FvCB::get_inputs() {
	return {
		"Ci",
		"Gstar",
		"Vcmax",
		"Kc",
		"O2",
		"Ko",
		"J",
		"electrons_per_carboxylation",
		"electrons_per_oxygenation",
		"maximum_tpu_rate",
		"Rd",
		"Ca",
		"atmospheric_pressure",
		"leaf_stomatal_conductance"
	};
}

string_vector FvCB::get_outputs() {
	return {
		"carboxylation_rate",
		"net_assimilation_rate",
		"Ci"
	};
}

void FvCB::do_operation() const {
	// Collect input quantities and make calculations

	double Ci = *Ci_ip;
	double Gstar = *Gstar_ip;
	double Vcmax = *Vcmax_ip;
	double Kc = *Kc_ip;
	double O2 = *O2_ip;
	double Ko = *Ko_ip;
	double J = *J_ip;
	double electrons_per_carboxylation = *electrons_per_carboxylation_ip;
	double electrons_per_oxygenation = *electrons_per_oxygenation_ip;
	double maximum_tpu_rate = *maximum_tpu_rate_ip;
	double Rd = *Rd_ip;
	double Ca = *Ca_ip;
	double atmospheric_pressure = *atmospheric_pressure_ip;
	double leaf_stomatal_conductance = *leaf_stomatal_conductance_ip;

	double rubisco_limited = Vcmax * (Ci - Gstar) / (Ci + Kc * (1.0 + O2 / Ko));

	double rubp_limited = J * (Ci - Gstar) / (electrons_per_carboxylation * Ci + 2.0 * electrons_per_oxygenation * Gstar);
	rubp_limited = std::max(rubp_limited, 0.0);		// This rate can't be negative.

	double tpu_limited = 3.0 * maximum_tpu_rate / (1.0 - Gstar / Ci);

	double carboxylation_rate = std::min(rubisco_limited, std::min(rubp_limited, tpu_limited));		// The overall carboxylation rate is the rate of the slowest process.
	double net_assimilation_rate = carboxylation_rate - Rd;

	Ci = Ca - net_assimilation_rate * 1.6 * atmospheric_pressure / leaf_stomatal_conductance;

	// Update the output quantity list
	update(carboxylation_rate_op, carboxylation_rate);
	update(net_assimilation_rate_op, net_assimilation_rate);
	update(Ci_op, Ci);

}

#endif
