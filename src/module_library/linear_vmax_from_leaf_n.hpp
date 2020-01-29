#ifndef LINEAR_VMAX_FROM_LEAF_N_H
#define LINEAR_VMAX_FROM_LEAF_N_H

#include "../modules.h"

class linear_vmax_from_leaf_n : public SteadyModule {
	public:
		linear_vmax_from_leaf_n(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("linear_vmax_from_leaf_n"),
			// Get pointers to input parameters
			LeafN_0_ip(get_ip(input_parameters, "LeafN_0")),
			LeafN_ip(get_ip(input_parameters, "LeafN")),
			vmax_n_intercept_ip(get_ip(input_parameters, "vmax_n_intercept")),
			vmax1_ip(get_ip(input_parameters, "vmax1")),
			// Get pointers to output parameters
			vmax_op(get_op(output_parameters, "vmax"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* LeafN_0_ip;
		const double* LeafN_ip;
		const double* vmax_n_intercept_ip;
		const double* vmax1_ip;
		// Pointers to output parameters
		double* vmax_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> linear_vmax_from_leaf_n::get_inputs() {
	return {
		"LeafN_0",
		"LeafN",
		"vmax_n_intercept",
		"vmax1"
	};
}

std::vector<std::string> linear_vmax_from_leaf_n::get_outputs() {
	return {
		"vmax"
	};
}

void linear_vmax_from_leaf_n::do_operation() const {
	// Collect inputs and make calculations
	double LeafN_0 = *LeafN_0_ip;
	double LeafN = *LeafN_ip;
	double vmax_n_intercept = *vmax_n_intercept_ip;
	double vmax1 = *vmax1_ip;
	
	// Update the output parameter list
	update(vmax_op, (LeafN_0 - LeafN) * vmax_n_intercept + vmax1);
}

#endif