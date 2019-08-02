#ifndef WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE_H
#define WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE_H

#include "../modules.h"

class water_vapor_properties_from_air_temperature : public SteadyModule {
	public:
		water_vapor_properties_from_air_temperature(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("water_vapor_properties_from_air_temperature"),
			// Get pointers to input parameters
			temp_ip(get_ip(input_parameters, "temp")),
			rh_ip(get_ip(input_parameters, "rh")),
			// Get pointers to output parameters
			latent_heat_vaporization_of_water_op(get_op(output_parameters, "latent_heat_vaporization_of_water")),
			slope_water_vapor_op(get_op(output_parameters, "slope_water_vapor")),
			saturation_water_vapor_pressure_op(get_op(output_parameters, "saturation_water_vapor_pressure")),
			water_vapor_pressure_op(get_op(output_parameters, "water_vapor_pressure")),
			vapor_density_deficit_op(get_op(output_parameters, "vapor_density_deficit")),
			psychrometric_parameter_op(get_op(output_parameters, "psychrometric_parameter"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* temp_ip;
		const double* rh_ip;
		// Pointers to output parameters
		double* latent_heat_vaporization_of_water_op;
		double* slope_water_vapor_op;
		double* saturation_water_vapor_pressure_op;
		double* water_vapor_pressure_op;
		double* vapor_density_deficit_op;
		double* psychrometric_parameter_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> water_vapor_properties_from_air_temperature::get_inputs() {
	return {
		"temp",
		"rh"
	};
}

std::vector<std::string> water_vapor_properties_from_air_temperature::get_outputs() {
	return {
		"latent_heat_vaporization_of_water",
		"slope_water_vapor",
		"saturation_water_vapor_pressure",
		"water_vapor_pressure",
		"vapor_density_deficit",
		"psychrometric_parameter"
	};
}

void water_vapor_properties_from_air_temperature::do_operation() const {
	// Collect input parameters and make calculations
	
	double air_temperature = *temp_ip;														// Degrees C
	double rh = *rh_ip;
	
	double specific_heat_of_water = 1010;													// J / kg / K
	double molar_mass_of_water = 18.01528e-3;												// kg / mol
	double R = 8.314472;																	// joule / kelvin / mole
	
	double density_of_dry_air = TempToDdryA(air_temperature);								// kg / m^3
	double latent_heat_vaporization_of_water = TempToLHV(air_temperature);					// J / kg
	double saturation_water_vapor_pressure = saturation_vapor_pressure(air_temperature);	// Pa
	double saturation_water_vapor_content = saturation_water_vapor_pressure / R / (air_temperature + 273.15) * molar_mass_of_water;	// kg / m^3. Convert from vapor pressure to vapor density using the ideal gas law. This is approximately right for temperatures what won't kill plants.
	double vapor_density_deficit = saturation_water_vapor_content * (1 - rh);
	
	// Update the output parameter list
	update(latent_heat_vaporization_of_water_op, latent_heat_vaporization_of_water);	// J / kg
	update(slope_water_vapor_op, TempToSFS(air_temperature));							// kg / m^3 / K. It is also kg / m^3 / degrees C since it's a change in temperature.
	update(saturation_water_vapor_pressure_op, saturation_water_vapor_pressure);		// Pa
	update(water_vapor_pressure_op, saturation_water_vapor_pressure * rh);				// Pa
	update(vapor_density_deficit_op, vapor_density_deficit);							// kg / m^3
	update(psychrometric_parameter_op, density_of_dry_air * specific_heat_of_water / latent_heat_vaporization_of_water);	// kg / m^3 / K
}

#endif
