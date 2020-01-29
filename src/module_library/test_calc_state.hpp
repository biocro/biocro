#ifndef TEST_CALC_STATE_H
#define TEST_CALC_STATE_H

#include "../modules.h"

class test_calc_state : public SteadyModule {
	public:
		test_calc_state(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("test_calc_state"),
			// Get pointers to input parameters
			LeafArea_ip(get_ip(input_parameters, "LeafArea")),
			parameter_ip(get_ip(input_parameters, "parameter")),
			// Get pointers to output parameters
			useless_parameter_op(get_op(output_parameters, "useless_parameter"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* LeafArea_ip;
		const double* parameter_ip;
		// Pointers to output parameters
		double* useless_parameter_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> test_calc_state::get_inputs() {
	return {
		"LeafArea",
		"parameter"
	};
}

std::vector<std::string> test_calc_state::get_outputs() {
	return {
		"useless_parameter"
	};
}

void test_calc_state::do_operation() const {
	// Update the output parameter list
	update(useless_parameter_op, (*LeafArea_ip) * (*parameter_ip));
}

#endif