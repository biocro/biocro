#ifndef STOMATA_WATER_STRESS_LINEAR_AND_ABA_RESPONSE_H
#define STOMATA_WATER_STRESS_LINEAR_AND_ABA_RESPONSE_H

#include "../modules.h"

class stomata_water_stress_linear_and_aba_response : public SteadyModule {
	public:
		stomata_water_stress_linear_and_aba_response(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("stomata_water_stress_linear_and_aba_response"),
			// Get pointers to input parameters
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			soil_aba_concentration_ip(get_ip(input_parameters, "soil_aba_concentration")),
			aba_influence_coefficient_ip(get_ip(input_parameters, "aba_influence_coefficient")),
			max_b1_ip(get_ip(input_parameters, "max_b1")),
			// Get pointers to output parameters
			StomataWS_op(get_op(output_parameters, "StomataWS")),
			b1_op(get_op(output_parameters, "b1"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* soil_water_content_ip;
		const double* soil_aba_concentration_ip;
		const double* aba_influence_coefficient_ip;
		const double* max_b1_ip;
		// Pointers to output parameters
		double* StomataWS_op;
		double* b1_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> stomata_water_stress_linear_and_aba_response::get_inputs() {
	return {
		"soil_field_capacity",
		"soil_wilting_point",
		"soil_water_content",
		"soil_aba_concentration",
		"aba_influence_coefficient",
		"max_b1"
	};
}

std::vector<std::string> stomata_water_stress_linear_and_aba_response::get_outputs() {
	return {
		"StomataWS",
		"b1"
	};
}

void stomata_water_stress_linear_and_aba_response::do_operation() const {	
	// Collect inputs and make calculations
	double soil_wilting_point = *soil_wilting_point_ip;
	double soil_field_capacity = *soil_field_capacity_ip;
	double soil_water_content = *soil_water_content_ip;
	double soil_aba_concentration = *soil_aba_concentration_ip;
	double aba_influence_coefficient = *aba_influence_coefficient_ip;
	double max_b1 = *max_b1_ip;
	
	double const slope = 1.0 / (soil_field_capacity - soil_wilting_point);
	double const intercept = 1.0 - soil_field_capacity * slope;
	double const aba_effect = exp(soil_aba_concentration / aba_influence_coefficient);  // dimensionless. A value in the interval (0, 1] that will reduce the slope of the Ball-Berry model.
	
	// Update the output parameter list
	update(StomataWS_op, std::min(std::max(slope *soil_water_content + intercept, 1e-10), 1.0));
	update(b1_op, max_b1 * aba_effect);
}

#endif
