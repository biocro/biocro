#ifndef SOIL_EVAPORATION_H
#define SOIL_EVAPORATION_H

#include "../modules.h"

class soil_evaporation : public SteadyModule {
	public:
		soil_evaporation(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("soil_evaporation"),
			// Get pointers to input parameters			
			lai_ip(get_ip(input_parameters, "lai")),
			temp_ip(get_ip(input_parameters, "temp")),
			solar_ip(get_ip(input_parameters, "solar")),
			soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
			soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
			soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
			windspeed_ip(get_ip(input_parameters, "windspeed")),
			rh_ip(get_ip(input_parameters, "rh")),
			rsec_ip(get_ip(input_parameters, "rsec")),
			soil_clod_size_ip(get_ip(input_parameters, "soil_clod_size")),
			soil_reflectance_ip(get_ip(input_parameters, "soil_reflectance")),
			soil_transmission_ip(get_ip(input_parameters, "soil_transmission")),
			specific_heat_ip(get_ip(input_parameters, "specific_heat")),
			stefan_boltzman_ip(get_ip(input_parameters, "stefan_boltzman")),
			// Get pointers to output parameters
			soil_evaporation_rate_op(get_op(output_parameters, "soil_evaporation_rate"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* lai_ip;
		const double* temp_ip;
		const double* solar_ip;
		const double* soil_water_content_ip;
		const double* soil_field_capacity_ip;
		const double* soil_wilting_point_ip;
		const double* windspeed_ip;
		const double* rh_ip;
		const double* rsec_ip;
		const double* soil_clod_size_ip;
		const double* soil_reflectance_ip;
		const double* soil_transmission_ip;
		const double* specific_heat_ip;
		const double* stefan_boltzman_ip;		
		// Pointers to output parameters
		double* soil_evaporation_rate_op;		
		// Main operation
		void do_operation() const;
};

std::vector<std::string> soil_evaporation::get_inputs() {
	return {
		"lai",
		"temp",
		"solar",
		"soil_water_content",
		"soil_field_capacity",
		"soil_wilting_point",
		"windspeed",
		"rh",
		"rsec",
		"soil_clod_size",
		"soil_reflectance",
		"soil_transmission",
		"specific_heat",
		"stefan_boltzman"
	};
}

std::vector<std::string> soil_evaporation::get_outputs() {
	return {
        "soil_evaporation_rate"
	};
}

void soil_evaporation::do_operation() const {
	// Collect inputs and make calculations
	// SoilEvapo(...) is located in AuxBioCro.cpp
	double soilEvap = SoilEvapo(*lai_ip, 0.68, *temp_ip, *solar_ip, *soil_water_content_ip,
		*soil_field_capacity_ip, *soil_wilting_point_ip, *windspeed_ip, *rh_ip, *rsec_ip,
		*soil_clod_size_ip, *soil_reflectance_ip, *soil_transmission_ip, *specific_heat_ip, *stefan_boltzman_ip);	// kg / m^2 / s
	
	soilEvap *= 3600 * 1e-3 * 10000;	// Convert to Mg / ha / hr for consistency with canopy_transpiration_rate and two_layer_soil_profile's output
	
	// Update the output parameter list
	update(soil_evaporation_rate_op, soilEvap);
}

#endif


