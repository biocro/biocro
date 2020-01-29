#ifndef ONE_LAYER_SOIL_PROFILE_H
#define ONE_LAYER_SOIL_PROFILE_H

#include "../modules.h"

class one_layer_soil_profile : public DerivModule {
	public:
		one_layer_soil_profile(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("one_layer_soil_profile"),
			// Get pointers to input parameters
			soil_evaporation_rate_ip(get_ip(input_parameters, "soil_evaporation_rate")),
			canopy_transpiration_rate_ip(get_ip(input_parameters, "canopy_transpiration_rate")),
			precip_ip(get_ip(input_parameters, "precip")),
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			soil_depth_ip(get_ip(input_parameters, "soil_depth")),
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			phi1_ip(get_ip(input_parameters, "phi1")),
			phi2_ip(get_ip(input_parameters, "phi2")),
			soil_saturation_capacity_ip(get_ip(input_parameters, "soil_saturation_capacity")),
			soil_sand_content_ip(get_ip(input_parameters, "soil_sand_content")),
			soil_saturated_conductivity_ip(get_ip(input_parameters, "soil_saturated_conductivity")),
			soil_air_entry_ip(get_ip(input_parameters, "soil_air_entry")),
			soil_b_coefficient_ip(get_ip(input_parameters, "soil_b_coefficient")),
			// Get pointers to output parameters
			soil_water_content_op(get_op(output_parameters, "soil_water_content"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* soil_evaporation_rate_ip;
		const double* canopy_transpiration_rate_ip;
		const double* precip_ip;
		const double* soil_water_content_ip;
		const double* soil_depth_ip;
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* phi1_ip;
		const double* phi2_ip;
		const double* soil_saturation_capacity_ip;
		const double* soil_sand_content_ip;
		const double* soil_saturated_conductivity_ip;
		const double* soil_air_entry_ip;
		const double* soil_b_coefficient_ip;
		// Pointers to output parameters
		double* soil_water_content_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> one_layer_soil_profile::get_inputs() {
	return {
		"soil_evaporation_rate",
		"canopy_transpiration_rate",
		"precip",
		"soil_water_content",
		"soil_depth",
		"soil_field_capacity",
		"soil_wilting_point",
		"phi1",
		"phi2",
		"soil_saturation_capacity",
		"soil_sand_content",
		"soil_saturated_conductivity",
		"soil_air_entry",
		"soil_b_coefficient"
	};
}

std::vector<std::string> one_layer_soil_profile::get_outputs() {
	return {
		"soil_water_content"
	};
}

void one_layer_soil_profile::do_operation() const {	
	// Collect inputs and make calculations
	double soilEvap = (*soil_evaporation_rate_ip);	// Mg / ha / hr
	double TotEvap = soilEvap + (*canopy_transpiration_rate_ip);
	
	// watstr(...) is located in AuxBioCro.cpp
	struct ws_str WaterS = watstr(*precip_ip, TotEvap, *soil_water_content_ip, *soil_depth_ip, *soil_field_capacity_ip,
					*soil_wilting_point_ip, *phi1_ip, *phi2_ip, *soil_saturation_capacity_ip, *soil_sand_content_ip,
					*soil_saturated_conductivity_ip, *soil_air_entry_ip, *soil_b_coefficient_ip);
	
	// Update the output parameter list
	update(soil_water_content_op, WaterS.awc - (*soil_water_content_ip));
}

#endif
