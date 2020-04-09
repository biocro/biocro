#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "../modules.h"

class test_module : public SteadyModule {
	public:
		test_module(const std::unordered_map<std::string, double>* /*input_parameters*/, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("test_module"),
			// Get pointers to input parameters
			// ...there are none!
			// Get pointers to output parameters
			first_op(get_op(output_parameters, "first"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		// ...there are none!
		// Pointers to output parameters
		double* first_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> test_module::get_inputs() {
	return {
		// Nothing here
	};
}

std::vector<std::string> test_module::get_outputs() {
	return {
		"first"
	};
}

void test_module::do_operation() const {
	// Update the output parameter list
	update(first_op, 19.5);
}

#endif