#ifndef BIG_LEAF_MULTILAYER_CANOPY_H
#define BIG_LEAF_MULTILAYER_CANOPY_H

#include "../modules.h"

class big_leaf_multilayer_canopy : public SteadyModule {
	public:
		big_leaf_multilayer_canopy(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("big_leaf_multilayer_canopy"),
			// Get pointers to input parameters
			lai_ip(get_ip(input_parameters, "lai")),
			nlayers_ip(get_ip(input_parameters, "nlayers")),
			rh_ip(get_ip(input_parameters, "rh")),
			windspeed_ip(get_ip(input_parameters, "windspeed")),
			kd_ip(get_ip(input_parameters, "kd")),
			lat_ip(get_ip(input_parameters, "lat")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			solar_ip(get_ip(input_parameters, "solar")),
			chil_ip(get_ip(input_parameters, "chil")),
			vmax1_ip(get_ip(input_parameters, "vmax1")),
			alpha1_ip(get_ip(input_parameters, "alpha1")),
			temperature_ip(get_ip(input_parameters, "temperature")),
			Kparm_ip(get_ip(input_parameters, "Kparm")),
			beta_ip(get_ip(input_parameters, "beta")),
			upperT_ip(get_ip(input_parameters, "upperT")),
			lowerT_ip(get_ip(input_parameters, "lowerT")),
			Catm_ip(get_ip(input_parameters, "Catm")),
			b0_ip(get_ip(input_parameters, "b0")),
			b1_ip(get_ip(input_parameters, "b1")),
			leafwidth_ip(get_ip(input_parameters, "leafwidth")),
			leaf_reflectance_ip(get_ip(input_parameters, "leaf_reflectance")),
			leaf_transmittance_ip(get_ip(input_parameters, "leaf_transmittance")),
			water_stress_approach_ip(get_ip(input_parameters, "water_stress_approach")),
			StomataWS_ip(get_ip(input_parameters, "StomataWS")),
			Rd_ip(get_ip(input_parameters, "Rd")),
			// Get pointers to output parameters
			canopy_assimilation_rate_op(get_op(output_parameters, "canopy_assimilation_rate")),
			canopy_transpiration_rate_op(get_op(output_parameters, "canopy_transpiration_rate")),
			canopy_conductance_op(get_op(output_parameters, "canopy_conductance"))
		{
			// Set up a system with a few other modules that we will be using
			
			// Make the steady state module list
			// The order is important, since the outputs from the first module
			//  are used as inputs for the next two, and the outputs from the
			//  second module are used as inputs for the third
			std::vector<std::string> steady_state_modules = {
				"water_vapor_properties_from_air_temperature",	// water_vapor_module
				"collatz_leaf",									// leaf_assim_module
				"penman_monteith_transpiration"					// leaf_evapo_module
			};
			
			// Make the derivative module list
			std::vector<std::string> derivative_modules = {
				// Nothing here
			};

			// Set the initial state
			// Its contents should be the state variables, i.e., the outputs
			//  of the derivative modules
			// In this case, it's empty
			std::unordered_map<std::string, double> initial_state;
			
			// Set the invariant parameters
			// Note: the system requires a timestep parameter,
			//  although it won't be used in this case
			// The other parameters should be the inputs to the steady
			//  state modules
			std::unordered_map<std::string, double> invariant_parameters = {
				{"timestep", 	1.0}
			};
			for(std::string param : water_vapor_properties_from_air_temperature::get_inputs()) invariant_parameters[param] = 1.0;	// The values will be set later
			for(std::string param : collatz_leaf::get_inputs()) invariant_parameters[param] = 1.0;	// The values will be set later
			for(std::string param : penman_monteith_transpiration::get_inputs()) invariant_parameters[param] = 1.0;	// The values will be set later
			
			// Set the varying parameters
			// Note: the system requires a few varying parameters,
			//  although they won't be used in this case
			std::vector<double> temp_vector(1, 0.0);	// Make a vector with just one entry of 0.0
			std::unordered_map<std::string, std::vector<double>> varying_parameters {
				{"time", temp_vector},
				{"doy", temp_vector},
				{"hour", temp_vector}
			};
			
			// Now that the inputs are defined, make the system (with verbose = false) and store a smart pointer to it
			canopy_sys = std::shared_ptr<System>(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, false));
		}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// This module require a system member so it can
		//  use other modules to make calculations
		std::shared_ptr<System> canopy_sys;
		// Pointers to input parameters
		const double* lai_ip;
		const double* nlayers_ip;
		const double* rh_ip;
		const double* windspeed_ip;
		const double* kd_ip;
		const double* lat_ip;
		const double* doy_dbl_ip;
		const double* solar_ip;
		const double* chil_ip;
		const double* vmax1_ip;
		const double* alpha1_ip;
		const double* temperature_ip;
		const double* Kparm_ip;
		const double* beta_ip;
		const double* upperT_ip;
		const double* lowerT_ip;
		const double* Catm_ip;
		const double* b0_ip;
		const double* b1_ip;
		const double* leafwidth_ip;
		const double* leaf_reflectance_ip;
		const double* leaf_transmittance_ip;
		const double* water_stress_approach_ip;
		const double* StomataWS_ip;
		const double* Rd_ip;
		// Pointers to output parameters
		double* canopy_assimilation_rate_op;
		double* canopy_transpiration_rate_op;
		double* canopy_conductance_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> big_leaf_multilayer_canopy::get_inputs() {
	return {
		"lai",
		"nlayers",
		"rh",
		"windspeed",
		"kd",
		"lat",
		"doy_dbl",
		"solar",
		"chil",
		"vmax1",
		"alpha1",
		"temperature",
		"Kparm",
		"beta",
		"upperT",
		"lowerT",
		"Catm",
		"b0",
		"b1",
		"leafwidth",
		"leaf_reflectance",
		"leaf_transmittance",
		"water_stress_approach",
		"StomataWS",
		"Rd"
	};
}

std::vector<std::string> big_leaf_multilayer_canopy::get_outputs() {
	return {
		"canopy_assimilation_rate",
		"canopy_transpiration_rate",
		"canopy_conductance"
	};
}

void big_leaf_multilayer_canopy::do_operation() const {	
	// Collect inputs and make calculations
	double lai = *lai_ip;
	double n_layers = *nlayers_ip;
	double rh = *rh_ip;
	double windspeed = *windspeed_ip;
	double kd = *kd_ip;
	double lat = *lat_ip;
	double doy_dbl = *doy_dbl_ip;
	double solar = *solar_ip;
	double chil = *chil_ip;
	double vmax1 = *vmax1_ip;
	double alpha1 = *alpha1_ip;
	double temperature = *temperature_ip;
	double Kparm = *Kparm_ip;
	double beta = *beta_ip;
	double upperT = *upperT_ip;
	double lowerT = *lowerT_ip;
	double Catm = *Catm_ip;
	double b0 = *b0_ip;
	double b1 = *b1_ip;
	double leafwidth = *leafwidth_ip;
	double leaf_reflectance = *leaf_reflectance_ip;
	double leaf_transmittance = *leaf_transmittance_ip;
	double water_stress_approach = *water_stress_approach_ip;
	double StomataWS = *StomataWS_ip;
	double Rd = *Rd_ip;
	
	const double kh = 1 - rh;
	const double layer_lai = lai / n_layers;
	
	// Convert doy_dbl into doy and hour
	int doy = floor(doy_dbl);
	double hour = 24.0 * (doy_dbl - doy);
	
	// Define constants
	constexpr double wind_speed_extinction = 0.7;
	constexpr double leaf_radiation_absorptivity = 0.8;
	constexpr double fraction_of_irradiance_in_PAR = 0.5;  // dimensionless.
	constexpr double joules_per_micromole_PAR = 0.235;   // J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:
	
	// Make state and derivative vectors to pass to the canopy system
	// They are required for calculations, although we don't
	// need them in this case (so they are empty)
	std::vector<double> x;
	std::vector<double> dxdt;
	
	// Get light properties
	Light_model irradiance_fractions = lightME(lat, doy, hour);
	
	// TODO: The light at the top of the canopy should not be part of this function. They should be calculated before this is called.
	// If cos(theta) is less than zero, the Sun is below the horizon, and lightME sets direct fraction to 0.
	// Thus, calculation depending on direct_par_at_canopy_top should not need to check cos(theta), although other calcuations may need to.
	const double direct_par_at_canopy_top = irradiance_fractions.direct_irradiance_fraction * solar;
	const double diffuse_par_at_canopy_top = irradiance_fractions.diffuse_irradiance_fraction * solar;
	
	const double cosine_theta = irradiance_fractions.cosine_zenith_angle;
	const double theta = acos(cosine_theta);
	const double k0 = sqrt( pow(chil, 2) + pow(tan(theta), 2) );
	const double k1 = chil + 1.744 * pow((chil + 1.183), -0.733);
	const double light_extinction_k = k0 / k1;  // Canopy extinction coefficient for an ellipsoidal leaf angle distribution. Page 251, Campbell and Norman. Environmental Biophysics. second edition.
	
	const double beam_radiation_at_canopy_top = direct_par_at_canopy_top * cosine_theta;
	double mean_incident_direct_par = beam_radiation_at_canopy_top * light_extinction_k;  // This is the same for all layers.
	
	// Initialize rates
	double canopy_assimilation = 0;
	double canopy_conductance = 0;
	double canopy_transpiration = 0;
	double sunlit_leaf_assimilation_rate;
	double sunlit_leaf_stomatal_conductance;
	double sunlit_leaf_transpiration_rate;
	double shaded_leaf_assimilation_rate;
	double shaded_leaf_stomatal_conductance;
	double shaded_leaf_transpiration_rate;
	
	// Set some system parameters that don't change with the layer
	canopy_sys->set_param(5.67e-8, "stefan_boltzman");
	canopy_sys->set_param(2, "k_Q10");
	canopy_sys->set_param(vmax1, "vmax");
	canopy_sys->set_param(alpha1, "alpha");
	canopy_sys->set_param(theta, "theta");
	canopy_sys->set_param(temperature, "temp");
	canopy_sys->set_param(Kparm, "kparm");
	canopy_sys->set_param(beta, "beta");
	canopy_sys->set_param(upperT, "upperT");
	canopy_sys->set_param(lowerT, "lowerT");
	canopy_sys->set_param(Catm, "Catm");
	canopy_sys->set_param(b0, "b0");
	canopy_sys->set_param(b1, "b1");
	canopy_sys->set_param(leafwidth, "leafwidth");
	canopy_sys->set_param(leaf_reflectance, "leaf_reflectance");
	canopy_sys->set_param(leaf_transmittance, "leaf_transmittance");
	canopy_sys->set_param(water_stress_approach, "water_stress_approach");
	canopy_sys->set_param(StomataWS, "StomataWS");
	canopy_sys->set_param(Rd, "Rd");
	
	// Iterate over the canopy layers
	for (unsigned int n = 0; n < n_layers; ++n) {
		// Get properties of this layer
		const double CumLAI = layer_lai * (n + 1);
		const double layer_wind_speed = windspeed * exp(-wind_speed_extinction * (CumLAI - layer_lai));
		
		const double j = n + 1;  // Explicitly make j a double so that j / n_layers isn't truncated.
		const double layer_relative_humidity = rh * exp(kh * (j / n_layers));
		
		const double layer_scattered_par = beam_radiation_at_canopy_top * (exp(-light_extinction_k * sqrt(leaf_radiation_absorptivity) * CumLAI) - exp(-light_extinction_k * CumLAI));
		double layer_diffuse_par = diffuse_par_at_canopy_top * exp(-kd * CumLAI) + layer_scattered_par;  // The exponential term is equation 15.6, pg 255 of Campbell and Normal. Environmental Biophysics. with alpha=1 and Kbe(phi) = Kd.
		
		double sunlit_lai = (1 - exp(-light_extinction_k * layer_lai)) * exp(-light_extinction_k * CumLAI) / light_extinction_k;
		
		if (cosine_theta <= 1e-10) {
			// For values of cos(theta) close to or less than 0, the sun is below the horizon.
			// In that case, no leaves are sunlit.
			// I don't know how C++ handles that calculation since light_extinction_k would be infinite, so set sunlai_lai to 0 explicitly.
			sunlit_lai = 0;
			mean_incident_direct_par = direct_par_at_canopy_top / k1;
			layer_diffuse_par = diffuse_par_at_canopy_top * exp(-light_extinction_k * CumLAI);
		}
		
		const double shaded_lai = layer_lai - sunlit_lai;
		
		const double layer_shaded_par = layer_diffuse_par;
		const double layer_sunlit_par = mean_incident_direct_par + layer_shaded_par;
		
		const double layer_sunlit_incident_irradiance = layer_sunlit_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR;  // W / m^2.
		const double layer_shaded_incident_irradiance = layer_shaded_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR; // W / m^2.
		
		// Set some system parameters that do change with the layer
		canopy_sys->set_param(layer_wind_speed, "layer_wind_speed");
		canopy_sys->set_param(layer_relative_humidity, "rh");
		
		// Run a calculation for the sunlit leaves and get the resulting rates
		canopy_sys->set_param(layer_sunlit_par, "incident_par");
		canopy_sys->set_param(layer_sunlit_incident_irradiance, "incident_irradiance");
		canopy_sys->operator()(x, dxdt, 0);
		canopy_sys->get_param(sunlit_leaf_assimilation_rate, "leaf_assimilation_rate");
		canopy_sys->get_param(sunlit_leaf_stomatal_conductance, "leaf_stomatal_conductance");
		canopy_sys->get_param(sunlit_leaf_transpiration_rate, "leaf_transpiration_rate");
		
		// Run a calculation for the shaded leaves and get the resulting rates
		canopy_sys->set_param(layer_shaded_par, "incident_par");
		canopy_sys->set_param(layer_shaded_incident_irradiance, "incident_irradiance");
		canopy_sys->operator()(x, dxdt, 0);
		canopy_sys->get_param(shaded_leaf_assimilation_rate, "leaf_assimilation_rate");
		canopy_sys->get_param(shaded_leaf_stomatal_conductance, "leaf_stomatal_conductance");
		canopy_sys->get_param(shaded_leaf_transpiration_rate, "leaf_transpiration_rate");
		
		// Add the results to the totals
		canopy_assimilation += sunlit_lai * sunlit_leaf_assimilation_rate + shaded_lai * shaded_leaf_assimilation_rate;  // micromoles / m^2 / s. Ground-area basis.
		canopy_conductance += sunlit_lai * sunlit_leaf_stomatal_conductance + shaded_lai * shaded_leaf_stomatal_conductance;  // mmol / m^2 / s. Ground-area basis.
		canopy_transpiration += sunlit_lai * sunlit_leaf_transpiration_rate + shaded_lai * shaded_leaf_transpiration_rate;  // kg / m^2 / s. Ground-area basis.
	}
	
	// Update the output parameter list
	
	// Convert assimilation units
	// 3600 - seconds per hour
	// 1e-6 - moles per micromole
	// 30 - grams per mole for CO2
	// 1e-6 - megagrams per gram
	// 10000 - meters squared per hectar
	
	// Convert transpiration units
	// 3600 - seconds per hour
	// 1e-3 - megagrams per kilogram
	// 10000 - meters squared per hectare
	
	
	//TODO: Figure out why canopy_conductance is always 0.
	update(canopy_assimilation_rate_op, canopy_assimilation * 3600 * 1e-6 * 30 * 1e-6 * 10000);
	update(canopy_assimilation_rate_op, canopy_transpiration * 3600 * 1e-3 * 10000);
	update(canopy_conductance_op, canopy_conductance);
}

#endif