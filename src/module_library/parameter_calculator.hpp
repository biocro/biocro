#ifndef PARAMETER_CALCULATOR_H
#define PARAMETER_CALCULATOR_H

#include "../modules.h"

class parameter_calculator : public SteadyModule {
	public:
		parameter_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("parameter_calculator"),
			// Get pointers to input parameters
			iSp_ip(get_ip(input_parameters, "iSp")),
			TTc_ip(get_ip(input_parameters, "TTc")),
			Sp_thermal_time_decay_ip(get_ip(input_parameters, "Sp_thermal_time_decay")),
			Leaf_ip(get_ip(input_parameters, "Leaf")),
			LeafN_0_ip(get_ip(input_parameters, "LeafN_0")),
			LeafN_ip(get_ip(input_parameters, "LeafN")),
			vmax_n_intercept_ip(get_ip(input_parameters, "vmax_n_intercept")),
			vmax1_ip(get_ip(input_parameters, "vmax1")),
			alphab1_ip(get_ip(input_parameters, "alphab1")),
			alpha1_ip(get_ip(input_parameters, "alpha1")),
			// Get pointers to output parameters
	        Sp_op(get_op(output_parameters, "Sp")),
	        lai_op(get_op(output_parameters, "lai")),
	        vmax_op(get_op(output_parameters, "vmax")),
	        alpha_op(get_op(output_parameters, "alpha"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* iSp_ip;
		const double* TTc_ip;
		const double* Sp_thermal_time_decay_ip;
		const double* Leaf_ip;
		const double* LeafN_0_ip;
		const double* LeafN_ip;
		const double* vmax_n_intercept_ip;
		const double* vmax1_ip;
		const double* alphab1_ip;
		const double* alpha1_ip;
		// Pointers to output parameters
        double* Sp_op;
        double* lai_op;
        double* vmax_op;
        double* alpha_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> parameter_calculator::get_inputs() {
	return {
		"iSp",
		"TTc",
		"Sp_thermal_time_decay",
		"Leaf",
		"LeafN_0",
		"LeafN",
		"vmax_n_intercept",
		"vmax1",
		"alphab1",
		"alpha1"
	};
}

std::vector<std::string> parameter_calculator::get_outputs() {
	return {
        "Sp",
        "lai",
        "vmax",
        "alpha"
	};
}

void parameter_calculator::do_operation() const {
	// Collect inputs and make calculations
	double Sp = (*iSp_ip) - (*TTc_ip) * (*Sp_thermal_time_decay_ip);
	
	// Update the output parameter list
	update(Sp_op, Sp);
	update(lai_op, (*Leaf_ip) * Sp);
	update(vmax_op, ((*LeafN_0_ip) - (*LeafN_ip)) * (*vmax_n_intercept_ip) + (*vmax1_ip));
	update(alpha_op, ((*LeafN_0_ip) - (*LeafN_ip)) * (*alphab1_ip) + (*alpha1_ip));
}

#endif


