#ifndef PENMAN_MONTEITH_LEAF_TEMPERATURE_H
#define PENMAN_MONTEITH_LEAF_TEMPERATURE_H

#include "../modules.h"
#include "../state_map.h"

class penman_monteith_leaf_temperature : public SteadyModule {
	public:
		penman_monteith_leaf_temperature(state_map const& input_quantities, state_map& output_quantities) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("penman_monteith_leaf_temperature"),
			// Get pointers to input quantities
			slope_water_vapor_ip(get_ip(input_quantities, "slope_water_vapor")),
			psychrometric_parameter_ip(get_ip(input_quantities, "psychrometric_parameter")),
			latent_heat_vaporization_of_water_ip(get_ip(input_quantities, "latent_heat_vaporization_of_water")),
			leaf_boundary_layer_conductance_ip(get_ip(input_quantities, "leaf_boundary_layer_conductance")),
			leaf_stomatal_conductance_ip(get_ip(input_quantities, "leaf_stomatal_conductance")),
			leaf_net_irradiance_ip(get_ip(input_quantities, "leaf_net_irradiance")),
			vapor_density_deficit_ip(get_ip(input_quantities, "vapor_density_deficit")),
			temp_ip(get_ip(input_quantities, "temp")),
			// Get pointers to output quantities
			leaf_temperature_op(get_op(output_quantities, "leaf_temperature"))
		{}
		static string_vector get_inputs();
		static string_vector get_outputs();
	private:
		// Pointers to input quantities
		const double* slope_water_vapor_ip;
		const double* psychrometric_parameter_ip;
		const double* latent_heat_vaporization_of_water_ip;
		const double* leaf_boundary_layer_conductance_ip;
		const double* leaf_stomatal_conductance_ip;
		const double* leaf_net_irradiance_ip;
		const double* vapor_density_deficit_ip;
		const double* temp_ip;
		// Pointers to output quantities
		double* leaf_temperature_op;
		// Main operation
		void do_operation() const;
};

#endif
