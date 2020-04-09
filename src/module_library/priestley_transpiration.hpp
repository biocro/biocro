#ifndef PRIESTLEY_TRANSPIRATION_H
#define PRIESTLEY_TRANSPIRATION_H

#include "../modules.h"

class priestley_transpiration : public SteadyModule {
	public:
		priestley_transpiration(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("priestley_transpiration"),
			// Get pointers to input parameters
			slope_water_vapor_ip(get_ip(input_parameters, "slope_water_vapor")),
			psychrometric_parameter_ip(get_ip(input_parameters, "psychrometric_parameter")),
			latent_heat_vaporization_of_water_ip(get_ip(input_parameters, "latent_heat_vaporization_of_water")),
			PhiN_ip(get_ip(input_parameters, "PhiN")),
			// Get pointers to output parameters
			transpiration_rate_op(get_op(output_parameters, "transpiration_rate"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* slope_water_vapor_ip;
		const double* psychrometric_parameter_ip;
		const double* latent_heat_vaporization_of_water_ip;
		const double* PhiN_ip;
		// Pointers to output parameters
		double* transpiration_rate_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> priestley_transpiration::get_inputs() {
	return {
		"slope_water_vapor",
		"psychrometric_parameter",
		"latent_heat_vaporization_of_water",
		"PhiN"
	};
}

std::vector<std::string> priestley_transpiration::get_outputs() {
	return {
		"transpiration_rate"
	};
}

void priestley_transpiration::do_operation() const {
	// Collect input parameters and make calculations
	
	double slope_water_vapor = *slope_water_vapor_ip;		// kg / m^3 / K
	double psychr_parameter = *psychrometric_parameter_ip;	// kg / m^3 / K
	double LHV = *latent_heat_vaporization_of_water_ip;		// J / kg
	double PhiN = *PhiN_ip;		// In penman_monteith_transpiration, PhiN is retrieved from "leaf_net_irradiance" (EBL)
	
	double evapotranspiration = 1.26 * slope_water_vapor * PhiN / (LHV * (slope_water_vapor + psychr_parameter));
	
	// Update the output parameter list
	update(transpiration_rate_op, evapotranspiration);		// kg / m^2 / s. Leaf area basis.
}

#endif