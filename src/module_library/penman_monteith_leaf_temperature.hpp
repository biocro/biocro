#ifndef PENMAN_MONTEITH_LEAF_TEMPERATURE_H
#define PENMAN_MONTEITH_LEAF_TEMPERATURE_H

#include "../modules.h"

class penman_monteith_leaf_temperature : public SteadyModule {
	public:
		penman_monteith_leaf_temperature(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("penman_monteith_leaf_temperature"),
			// Get pointers to input parameters
			slope_water_vapor_ip(get_ip(input_parameters, "slope_water_vapor")),
			psychrometric_parameter_ip(get_ip(input_parameters, "psychrometric_parameter")),
			latent_heat_vaporization_of_water_ip(get_ip(input_parameters, "latent_heat_vaporization_of_water")),
			leaf_boundary_layer_conductance_ip(get_ip(input_parameters, "leaf_boundary_layer_conductance")),
			leaf_stomatal_conductance_ip(get_ip(input_parameters, "leaf_stomatal_conductance")),
			leaf_net_irradiance_ip(get_ip(input_parameters, "leaf_net_irradiance")),
			vapor_density_deficit_ip(get_ip(input_parameters, "vapor_density_deficit")),
			temp_ip(get_ip(input_parameters, "temp")),
			// Get pointers to output parameters
			leaf_temperature_op(get_op(output_parameters, "leaf_temperature"))
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
		const double* temp_ip;
		// Pointers to output parameters
		double* leaf_temperature_op;
		// Main operation
		void do_operation() const;
};

#endif
