#ifndef STOMATA_WATER_STRESS_EXPONENTIAL_H
#define STOMATA_WATER_STRESS_EXPONENTIAL_H

#include "../modules.h"
#include <cmath>  // for exp

class stomata_water_stress_exponential : public SteadyModule {
	public:
		stomata_water_stress_exponential(const std::unordered_map<std::string, double>* input_quantities, std::unordered_map<std::string, double>* output_quantities) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("stomata_water_stress_exponential"),
			// Get pointers to input quantities
			soil_field_capacity_ip(get_ip(input_quantities, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_quantities, "soil_wilting_point")),
			soil_water_content_ip(get_ip(input_quantities, "soil_water_content")),
			// Get pointers to output quantities
			StomataWS_op(get_op(output_quantities, "StomataWS"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input quantities
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* soil_water_content_ip;
		// Pointers to output quantities
		double* StomataWS_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> stomata_water_stress_exponential::get_inputs() {
	return {
		"soil_field_capacity",
		"soil_wilting_point",
		"soil_water_content"
	};
}

std::vector<std::string> stomata_water_stress_exponential::get_outputs() {
	return {
		"StomataWS"
	};
}

void stomata_water_stress_exponential::do_operation() const {
	// Collect inputs and make calculations
	double soil_wilting_point = *soil_wilting_point_ip;
	double soil_field_capacity = *soil_field_capacity_ip;
	double soil_water_content = *soil_water_content_ip;

	double slope = (1.0 - soil_wilting_point) / (soil_field_capacity - soil_wilting_point);
	double intercept = 1.0 - soil_field_capacity * slope;
	double theta = slope * soil_water_content + intercept;

	// Update the output quantity list
	update(StomataWS_op, std::min(std::max((1.0 - exp(-2.5 * (theta - soil_wilting_point)/(1.0 - soil_wilting_point))) / (1.0 - exp(-2.5)), 1e-10), 1.0));
}

#endif
