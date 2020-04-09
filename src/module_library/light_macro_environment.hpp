#ifndef LIGHT_MACRO_ENVIRONMENT_H
#define LIGHT_MACRO_ENVIRONMENT_H

#include "../modules.h"

class light_macro_environment : public SteadyModule {
	public:
		light_macro_environment(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("light_macro_environment"),
			// Get pointers to input parameters
			cosine_zenith_angle_ip(get_ip(input_parameters, "cosine_zenith_angle")),
			atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
			atmospheric_transmittance_ip(get_ip(input_parameters, "atmospheric_transmittance")),
			// Get pointers to output parameters
			direct_irradiance_fraction_op(get_op(output_parameters, "direct_irradiance_fraction")),
			diffuse_irradiance_fraction_op(get_op(output_parameters, "diffuse_irradiance_fraction"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
		static std::string get_description();
	private:
		// Pointers to input parameters
		const double* cosine_zenith_angle_ip;
		const double* atmospheric_pressure_ip;
		const double* atmospheric_transmittance_ip;
		// Pointers to output parameters
		double* direct_irradiance_fraction_op;
		double* diffuse_irradiance_fraction_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> light_macro_environment::get_inputs() {
	return {
		"cosine_zenith_angle",
		"atmospheric_pressure",
		"atmospheric_transmittance"
	};
}

std::vector<std::string> light_macro_environment::get_outputs() {
	return {
		"direct_irradiance_fraction",
		"diffuse_irradiance_fraction"
	};
}

std::string light_macro_environment::get_description() {
	std::string description = std::string("  Calculates the amount of sunlight scattered out of the direct beam by the atmosphere.\n");
	description += std::string("  Intended to be used along with the solar_zenith_angle module, which should be run before this module.");
	return description;
}

void light_macro_environment::do_operation() const {
	// Collect inputs and make calculations
	const double cosine_zenith_angle = *cosine_zenith_angle_ip;
	const double atmospheric_pressure = *atmospheric_pressure_ip;
	const double atmospheric_transmittance = *atmospheric_transmittance_ip;
	
	// Calculate diffuse and direct irradiance transmittance
	double direct_irradiance_transmittance;
    double diffuse_irradiance_transmittance;
	if(cosine_zenith_angle <= 0) {
		// In this case, the sun is at or below the horizon, so there is no direct irradiance
		direct_irradiance_transmittance = 0.0;
		diffuse_irradiance_transmittance = 1.0;
	}
	else {
		// The sun is above the horizon, so use its zenith angle to calculate transmittance
		// The direct transmission is calculated using Equation 11.11 from Campbell & Norman (1998), solving for
		//  direct_irradiance_transmittance = S_p / S_p0
		// The indirect transmission is calculated using Equation 11.13 from Campbell & Norman (1998), solving for
		//  indirect_irradiance_transmittance = S_d / S_p0
		constexpr double atmospheric_pressure_at_sea_level = 101325;									// Pa.
		const double pressure_ratio = atmospheric_pressure / atmospheric_pressure_at_sea_level;			// dimensionless.
		constexpr double proportion_of_irradiance_scattered = 0.3;										// dimensionless.
		direct_irradiance_transmittance = pow(atmospheric_transmittance, (pressure_ratio / cosine_zenith_angle));								// Modified from equation 11.11 of Norman and Campbell.
		diffuse_irradiance_transmittance = proportion_of_irradiance_scattered * (1 - direct_irradiance_transmittance) * cosine_zenith_angle;	// Modified from equation 11.13 of Norman and Campbell.
	}
	
	// Update the output parameter list
	update(direct_irradiance_fraction_op, direct_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance));		// dimensionless.
	update(diffuse_irradiance_fraction_op, diffuse_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance));	// dimensionless.
}

#endif