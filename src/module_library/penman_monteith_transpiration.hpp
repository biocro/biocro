#ifndef PENMAN_MONTEITH_TRANSPIRATION_H
#define PENMAN_MONTEITH_TRANSPIRATION_H

#include "../modules.h"

class penman_monteith_transpiration : public SteadyModule {
	public:
		penman_monteith_transpiration(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("penman_monteith_transpiration"),
			// Get pointers to input parameters
			slope_water_vapor_ip(get_ip(input_parameters, "slope_water_vapor")),
			psychrometric_parameter_ip(get_ip(input_parameters, "psychrometric_parameter")),
			latent_heat_vaporization_of_water_ip(get_ip(input_parameters, "latent_heat_vaporization_of_water")),
			leaf_boundary_layer_conductance_ip(get_ip(input_parameters, "leaf_boundary_layer_conductance")),
			leaf_stomatal_conductance_ip(get_ip(input_parameters, "leaf_stomatal_conductance")),
			leaf_net_irradiance_ip(get_ip(input_parameters, "leaf_net_irradiance")),
			vapor_density_deficit_ip(get_ip(input_parameters, "vapor_density_deficit")),
			// Get pointers to output parameters
			leaf_transpiration_rate_op(get_op(output_parameters, "leaf_transpiration_rate"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* slope_water_vapor_ip;
		const double* psychrometric_parameter_ip;
		const double* latent_heat_vaporization_of_water_ip;
		const double* leaf_boundary_layer_conductance_ip;
		const double* leaf_stomatal_conductance_ip;
		const double* leaf_net_irradiance_ip;
		const double* vapor_density_deficit_ip;
		// Pointers to output parameters
		double* leaf_transpiration_rate_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> penman_monteith_transpiration::get_inputs() {
	return {
		"slope_water_vapor",
		"psychrometric_parameter",
		"latent_heat_vaporization_of_water",
		"leaf_boundary_layer_conductance",
		"leaf_stomatal_conductance",
		"leaf_net_irradiance",
		"vapor_density_deficit"
	};
}

std::vector<std::string> penman_monteith_transpiration::get_outputs() {
	return {
		"leaf_transpiration_rate"
	};
}

void penman_monteith_transpiration::do_operation() const {
	// Collect input parameters and make calculations
	
	// From Thornley and Johnson 1990. pg 408. equation 14.4k.
	double slope_water_vapor = *slope_water_vapor_ip;		// kg / m^3 / K
	double psychr_parameter = *psychrometric_parameter_ip;	// kg / m^3 / K
	double LHV = *latent_heat_vaporization_of_water_ip;		// J / kg
	double ga = *leaf_boundary_layer_conductance_ip;		// m / s
	
	double volume_of_one_mole_of_air = 24.39e-3;											// m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
	double gc = *leaf_boundary_layer_conductance_ip * 1e-3 * volume_of_one_mole_of_air;		// m / s
	
	double PhiN = *leaf_net_irradiance_ip;		// In priestley_transpiration, PhiN is retrieved from "PhiN" (EBL)
	
	double vapor_density_deficit = *vapor_density_deficit_ip;
	double evapotranspiration = (slope_water_vapor * PhiN + LHV * psychr_parameter * ga * vapor_density_deficit) / (LHV * (slope_water_vapor + psychr_parameter * (1 + ga / gc)));
	
	// Update the output parameter list
	update(leaf_transpiration_rate_op, evapotranspiration);		// kg / m^2 / s. Leaf area basis.
}

#endif