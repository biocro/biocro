#ifndef BIOMASS_LEAF_N_LIMITATION_H
#define BIOMASS_LEAF_N_LIMITATION_H

#include "../modules.h"

class biomass_leaf_n_limitation : public SteadyModule {
	public:
		biomass_leaf_n_limitation(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("biomass_leaf_n_limitation"),
			// Get pointers to input parameters
			LeafN_0_ip(get_ip(input_parameters, "LeafN_0")),
			Leaf_ip(get_ip(input_parameters, "Leaf")),
			Stem_ip(get_ip(input_parameters, "Stem")),
			kln_ip(get_ip(input_parameters, "kln")),
			// Get pointers to output parameters
			LeafN_op(get_op(output_parameters, "LeafN"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* LeafN_0_ip;
		const double* Leaf_ip;
		const double* Stem_ip;
		const double* kln_ip;
		// Pointers to output parameters
		double* LeafN_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> biomass_leaf_n_limitation::get_inputs() {
	return {
		"LeafN_0",
		"Leaf",
		"Stem",
		"kln"
	};
}

std::vector<std::string> biomass_leaf_n_limitation::get_outputs() {
	return {
		"LeafN"
	};
}

void biomass_leaf_n_limitation::do_operation() const {
	// Collect inputs and make calculations
	double leaf_n;
	if(fabs((*Leaf_ip) + (*Stem_ip)) < eps) leaf_n = *LeafN_0_ip;
	else leaf_n = (*LeafN_0_ip) * pow((*Leaf_ip) + (*Stem_ip), -1.0*(*kln_ip));
	
	// Update the output parameter list
    update(LeafN_op, std::min(*LeafN_0_ip, leaf_n));
}

#endif
