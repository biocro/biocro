#ifndef NR_EX_H
#define NR_EX_H

#include "../modules.h"

// This module is an example in Chapter 16 of Numerical Recipes in C
// The analytical solution is:
//  u = 2 * exp(-x) - exp(-1000x)
//  v = -exp(-x) + exp(-1000x)
// For u(0) = 1, v(0) = 0

class nr_ex : public DerivModule {
	public:
		nr_ex(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) : 
			DerivModule("nr_ex"),
			// Get input pointers
			u_ip(get_ip(input_parameters, "u")),
			v_ip(get_ip(input_parameters, "v")),
			// Get output pointers for time derivatives
			u_op(get_op(output_parameters, "u")),
			v_op(get_op(output_parameters, "v"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Input pointers
		const double* u_ip;
		const double* v_ip;
		
		// Output pointers for time derivatives
		double* u_op;
		double* v_op;
		
		// Main operation
		void do_operation() const;
};

std::vector<std::string> nr_ex::get_inputs() {
	return {"u", "v"};
}

std::vector<std::string> nr_ex::get_outputs() {
	return {"u", "v"};
}

void nr_ex::do_operation() const {
	// Calculate the time derivatives and modify the module output map
	update(u_op, +998.0 * (*u_ip) + 1998.0 * (*v_ip));
	update(v_op, -999.0 * (*u_ip) - 1999.0 * (*v_ip));
}

#endif
