#ifndef MODULE_GRAPH_TEST_H
#define MODULE_GRAPH_TEST_H

#include "../modules.h"

class Module_1 : public SteadyModule {
	public:
		Module_1(const std::unordered_map<std::string, double>* /*input_parameters*/, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("Module_1"),
			// Get pointers to input parameters
			// None
			// Get pointers to output parameters
			A_op(get_op(output_parameters, "A")),
			B_op(get_op(output_parameters, "B"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		// Nothing here
		// Pointers to output parameters
		double* A_op;
		double* B_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> Module_1::get_inputs() {
	return {
		// None
	};
}

std::vector<std::string> Module_1::get_outputs() {
	return {
		"A",
		"B"
	};
}

void Module_1::do_operation() const {
	// Collect inputs and make calculations
	// Nothing here
	
	// Update the output parameter list
	update(A_op, 0.0);
	update(B_op, 0.0);
}

class Module_2 : public SteadyModule {
	public:
		Module_2(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("Module_2"),
			// Get pointers to input parameters
			B_ip(get_ip(input_parameters, "B")),
			// Get pointers to output parameters
			C_op(get_op(output_parameters, "C")),
			D_op(get_op(output_parameters, "D"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* B_ip;
		// Pointers to output parameters
		double* C_op;
		double* D_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> Module_2::get_inputs() {
	return {
		"B"
	};
}

std::vector<std::string> Module_2::get_outputs() {
	return {
		"C",
		"D"
	};
}

void Module_2::do_operation() const {
	// Collect inputs and make calculations
	// Nothing here
	
	// Update the output parameter list
	update(C_op, 0.0);
	update(D_op, 0.0);
}

class Module_3 : public SteadyModule {
	public:
		Module_3(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("Module_3"),
			// Get pointers to input parameters
			A_ip(get_ip(input_parameters, "A")),
			C_ip(get_ip(input_parameters, "C")),
			// Get pointers to output parameters
			E_op(get_op(output_parameters, "E"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* A_ip;
		const double* C_ip;
		// Pointers to output parameters
		double* E_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> Module_3::get_inputs() {
	return {
		"A",
		"C"
	};
}

std::vector<std::string> Module_3::get_outputs() {
	return {
		"E"
	};
}

void Module_3::do_operation() const {
	// Collect inputs and make calculations
	// Nothing here
	
	// Update the output parameter list
	update(E_op, 0.0);
}

#endif