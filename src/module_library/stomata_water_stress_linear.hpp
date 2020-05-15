#ifndef STOMATA_WATER_STRESS_LINEAR_H
#define STOMATA_WATER_STRESS_LINEAR_H

#include "../modules.h"

class stomata_water_stress_linear : public SteadyModule {
	public:
		stomata_water_stress_linear(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("stomata_water_stress_linear"),
			// Get pointers to input parameters
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			// Get pointers to output parameters
			StomataWS_op(get_op(output_parameters, "StomataWS"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* soil_water_content_ip;
		// Pointers to output parameters
		double* StomataWS_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> stomata_water_stress_linear::get_inputs() {
	return {
		"soil_field_capacity",
		"soil_wilting_point",
		"soil_water_content"
	};
}

std::vector<std::string> stomata_water_stress_linear::get_outputs() {
	return {
		"StomataWS"
	};
}

void stomata_water_stress_linear::do_operation() const {	
	// Collect inputs and make calculations
	double soil_wilting_point = *soil_wilting_point_ip;
	double soil_field_capacity = *soil_field_capacity_ip;
	double soil_water_content = *soil_water_content_ip;
	
    double slope = 1.0 / (soil_field_capacity - soil_wilting_point);
    double intercept = 1.0 - soil_field_capacity * slope;
	
	// Update the output parameter list
	update(StomataWS_op, std::min(std::max(slope * soil_water_content + intercept, 1e-10), 1.0));
}

#endif
