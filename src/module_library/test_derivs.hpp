#ifndef TEST_DERIVS_H
#define TEST_DERIVS_H

#include "../modules.h"

class test_derivs : public DerivModule {
	public:
		test_derivs(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("test_derivs"),
			// Get pointers to input parameters
			LeafArea_ip(get_ip(input_parameters, "LeafArea")),
			PAR_ip(get_ip(input_parameters, "PAR")),
			// Get pointers to output parameters
			CarbonAvailableForGrowth_op(get_op(output_parameters, "CarbonAvailableForGrowth"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* LeafArea_ip;
		const double* PAR_ip;
		// Pointers to output parameters
		double* CarbonAvailableForGrowth_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> test_derivs::get_inputs() {
	return {
		"LeafArea",
		"PAR"
	};
}

std::vector<std::string> test_derivs::get_outputs() {
	return {
		"CarbonAvailableForGrowth"
	};
}

void test_derivs::do_operation() const {
	// Update the output parameter list
	update(CarbonAvailableForGrowth_op, (*LeafArea_ip) * (*PAR_ip));
}

#endif