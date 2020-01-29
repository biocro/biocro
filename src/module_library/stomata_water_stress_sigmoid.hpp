#ifndef STOMATA_WATER_STRESS_SIGMOID_H
#define STOMATA_WATER_STRESS_SIGMOID_H

#include "../modules.h"

class stomata_water_stress_sigmoid : public SteadyModule {
	public:
		stomata_water_stress_sigmoid(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("stomata_water_stress_sigmoid"),
			// Get pointers to input parameters
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			phi1_ip(get_ip(input_parameters, "phi1")),
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
		const double* phi1_ip;
		// Pointers to output parameters
		double* StomataWS_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> stomata_water_stress_sigmoid::get_inputs() {
	return {
		"soil_field_capacity",
		"soil_wilting_point",
		"soil_water_content",
		"phi1"
	};
}

std::vector<std::string> stomata_water_stress_sigmoid::get_outputs() {
	return {
		"StomataWS"
	};
}

void stomata_water_stress_sigmoid::do_operation() const {	
	// Collect inputs and make calculations
	double soil_wilting_point = *soil_wilting_point_ip;
	double soil_field_capacity = *soil_field_capacity_ip;
	double soil_water_content = *soil_water_content_ip;
	double phi1 = *phi1_ip;
	
	const double phi10 = (soil_field_capacity + soil_wilting_point) / 2;
	
	// Update the output parameter list
	update(StomataWS_op, std::min(std::max(1.0 / (1.0 + exp((phi10 - soil_water_content) / phi1)), 1e-10), 1.0));
}

#endif
