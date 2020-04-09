#ifndef LEAF_WATER_STRESS_EXPONENTIAL_H
#define LEAF_WATER_STRESS_EXPONENTIAL_H

#include "../modules.h"

class leaf_water_stress_exponential : public SteadyModule {
	public:
		leaf_water_stress_exponential(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("leaf_water_stress_exponential"),
			// Get pointers to input parameters
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			phi2_ip(get_ip(input_parameters, "phi2")),
			// Get pointers to output parameters
			LeafWS_op(get_op(output_parameters, "LeafWS"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_water_content_ip;
		const double* soil_field_capacity_ip;
		const double* phi2_ip;
		// Pointers to output parameters
		double* LeafWS_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> leaf_water_stress_exponential::get_inputs() {
	return {
		"soil_field_capacity",
		"soil_water_content",
		"phi2"
	};
}

std::vector<std::string> leaf_water_stress_exponential::get_outputs() {
	return {
		"LeafWS"
	};
}

void leaf_water_stress_exponential::do_operation() const {	
	// Update the output parameter list
	update(LeafWS_op, std::min(std::max(pow((*soil_water_content_ip)/(*soil_field_capacity_ip), *phi2_ip), eps), 1.0));
}

#endif
