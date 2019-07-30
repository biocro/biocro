#ifndef THERMAL_TIME_ACCUMULATOR_H
#define THERMAL_TIME_ACCUMULATOR_H

#include "../module.hpp"

class thermal_time_accumulator : public DerivModule {
	public:
		thermal_time_accumulator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("thermal_time_accumulator"),
			// Get pointers to input parameters
			temp_ip(get_ip(input_parameters, "temp")),
			tbase_ip(get_ip(input_parameters, "tbase")),
			// Get pointers to output parameters
			TTc_op(get_op(output_parameters, "TTc"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* temp_ip;
		const double* tbase_ip;
		// Pointers to output parameters
		double* TTc_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> thermal_time_accumulator::get_inputs() {
	return {
		"temp",
		"tbase"
	};
}

std::vector<std::string> thermal_time_accumulator::get_outputs() {
	return {
		"TTc"
	};
}

void thermal_time_accumulator::do_operation() const {	
	// Collect inputs and make calculations	
	double temp_diff = (*temp_ip) - (*tbase_ip);
	
	// Update the output parameter list
	update(TTc_op, (temp_diff > eps) ? temp_diff / 24 : 0);
}

#endif
