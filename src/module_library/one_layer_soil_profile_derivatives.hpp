#ifndef ONE_LAYER_SOIL_PROFILE_DERIVATIVES_H
#define ONE_LAYER_SOIL_PROFILE_DERIVATIVES_H

#include "../modules.h"

class one_layer_soil_profile_derivatives : public DerivModule {
	public:
		one_layer_soil_profile_derivatives(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("one_layer_soil_profile_derivatives"),
			// Get pointers to input parameters
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			soil_depth_ip(get_ip(input_parameters, "soil_depth")),
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			soil_saturated_conductivity_ip(get_ip(input_parameters, "soil_saturated_conductivity")),
			soil_air_entry_ip(get_ip(input_parameters, "soil_air_entry")),
			soil_b_coefficient_ip(get_ip(input_parameters, "soil_b_coefficient")),
			acceleration_from_gravity_ip(get_ip(input_parameters, "acceleration_from_gravity")),
			precipitation_rate_ip(get_ip(input_parameters, "precipitation_rate")),
			soil_saturation_capacity_ip(get_ip(input_parameters, "soil_saturation_capacity")),
			soil_sand_content_ip(get_ip(input_parameters, "soil_sand_content")),
			evapotranspiration_ip(get_ip(input_parameters, "evapotranspiration")),
			// Get pointers to output parameters
			soil_water_content_op(get_op(output_parameters, "soil_water_content")),
			soil_n_content_op(get_op(output_parameters, "soil_n_content"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_water_content_ip;
		const double* soil_depth_ip;
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* soil_saturated_conductivity_ip;
		const double* soil_air_entry_ip;
		const double* soil_b_coefficient_ip;
		const double* acceleration_from_gravity_ip;
		const double* precipitation_rate_ip;
		const double* soil_saturation_capacity_ip;
		const double* soil_sand_content_ip;
		const double* evapotranspiration_ip;
		// Pointers to output parameters
		double* soil_water_content_op;
		double* soil_n_content_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> one_layer_soil_profile_derivatives::get_inputs() {
	return {
		"soil_water_content",
		"soil_depth",
		"soil_field_capacity",
		"soil_wilting_point",
		"soil_saturated_conductivity",
		"soil_air_entry",
		"soil_b_coefficient",
		"acceleration_from_gravity",
		"precipitation_rate",
		"soil_saturation_capacity",
		"soil_sand_content",
		"evapotranspiration"
	};
}

std::vector<std::string> one_layer_soil_profile_derivatives::get_outputs() {
	return {
		"soil_water_content",
		"soil_n_content"
	};
}

void one_layer_soil_profile_derivatives::do_operation() const {	
	// Collect inputs and make calculations
	double soil_water_content = *soil_water_content_ip;	// m^3 m^-3.
	double soil_depth = *soil_depth_ip;
	double soil_field_capacity = *soil_field_capacity_ip;
	double soil_wilting_point = *soil_wilting_point_ip;
	double soil_saturated_conductivity = *soil_saturated_conductivity_ip;
	double soil_air_entry = *soil_air_entry_ip;
	double soil_b_coefficient = *soil_b_coefficient_ip;
	double acceleration_from_gravity = *acceleration_from_gravity_ip;
	double precipitation_rate = *precipitation_rate_ip;
	double soil_saturation_capacity = *soil_saturation_capacity_ip;
	double soil_sand_content = *soil_sand_content_ip;
	double evapotranspiration = *evapotranspiration_ip;
	
	double soil_water_potential = -exp(log(0.033) + ((log(soil_field_capacity) - log(soil_water_content)) / (log(soil_field_capacity) - log(soil_wilting_point)) * (log(1.5) - log(0.033)))) * 1e3;	// kPa.
	
	double hydraulic_conductivity = soil_saturated_conductivity * pow(soil_air_entry / soil_water_potential, 2 + 3 / soil_b_coefficient);	// Kg s m^-3.
	double J_w = -hydraulic_conductivity * (-soil_water_potential / (soil_depth * 0.5)) - acceleration_from_gravity * hydraulic_conductivity;	// kg m^-2 s^-1. Campbell, pg 129. I multiply soil depth by 0.5 to calculate the average depth.
	
	double density_of_water_at_20_celcius = 998.2;	// kg m^-3

	double drainage = J_w / density_of_water_at_20_celcius;	// m s^-1.
	double precipitation_m_s = precipitation_rate * 1e-3;	// m s^-2.

	double second_per_hour = 3600;
	double runoff = (soil_water_content - soil_saturation_capacity) * soil_depth / second_per_hour;	// m s^-1. The previous model drained everything in a single time-step. By default the time step was one hour, so use a rate that drains everything in one hour.
	double n_leach = runoff / 18 * (0.2 + 0.7 * soil_sand_content) / second_per_hour;	// Base the rate on an hour for the same reason as was used with `runoff`.

	double evapotranspiration_volume = evapotranspiration / density_of_water_at_20_celcius / 1e4 / second_per_hour;	// m^3 m^-2 s^-1
	
	// Update the output parameter list
	update(soil_water_content_op, (drainage + precipitation_m_s - runoff - evapotranspiration_volume) / soil_depth);
	update(soil_n_content_op, -n_leach);
}

#endif