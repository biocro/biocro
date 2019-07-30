#ifndef REACTION_H
#define REACTION_H

#include "../modules.h"

class reaction : public DerivModule {
	public:
		reaction(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) : 
			DerivModule("reaction"),
			// Get input pointers
			k1_ip(get_ip(input_parameters, "rate_coefficient_1")),
			k2_ip(get_ip(input_parameters, "rate_coefficient_2")),
			s1_ip(get_ip(input_parameters, "s_1")),
			s2_ip(get_ip(input_parameters, "s_2")),
			// Get output pointers for time derivatives
			s1_op(get_op(output_parameters, "s_1")),
			s2_op(get_op(output_parameters, "s_2"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Input pointers
		const double* k1_ip;
		const double* k2_ip;
		const double* s1_ip;
		const double* s2_ip;
		
		// Output pointers for time derivatives
		double* s1_op;
		double* s2_op;
		
		// Output pointers for Jacobian
		double* dfs1ds1_op;
		double* dfs1ds2_op;
		double* dfs2ds1_op;
		
		// Main operation
		void do_operation() const;
		void do_operation_jacobian() const;
};

std::vector<std::string> reaction::get_inputs() {
	return {"rate_coefficient_1", "rate_coefficient_2", "s_1", "s_2"};
}

std::vector<std::string> reaction::get_outputs() {
	return {"s_1", "s_2"};
}

void reaction::do_operation() const {
	// Calculate the time derivatives and modify the module output map
	update(s1_op, -1.0 * (*k1_ip) * (*s1_ip) - 1.0 * (*k2_ip) * (*s2_ip));
	update(s2_op, *s1_ip);
}

void reaction::do_operation_jacobian() const {
	// Calculate the partial derivatives and modify the Jacobian matrix
	update(dfs1ds1_op, -1.0 * (*k1_ip));
	update(dfs1ds2_op, -1.0 * (*k2_ip));
	update(dfs2ds1_op, 1.0);
}

#endif
