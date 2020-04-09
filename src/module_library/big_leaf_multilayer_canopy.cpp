#include "big_leaf_multilayer_canopy.hpp"

// Constructor
big_leaf_multilayer_canopy::big_leaf_multilayer_canopy(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
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
	temp_ip(get_ip(input_parameters, "temp")),
	kparm_ip(get_ip(input_parameters, "kparm")),
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
	stefan_boltzman_ip(get_ip(input_parameters, "stefan_boltzman")),
	k_Q10_ip(get_ip(input_parameters, "k_Q10")),
	// Get pointers to output parameters
	canopy_assimilation_rate_op(get_op(output_parameters, "canopy_assimilation_rate")),
	canopy_transpiration_rate_op(get_op(output_parameters, "canopy_transpiration_rate")),
	canopy_conductance_op(get_op(output_parameters, "canopy_conductance"))
{
	// Set up a few standalone modules that we will be using
	
	// Make the steady state module list
	// The order is important, since the outputs from the first module
	//  are used as inputs for the next two, and the outputs from the
	//  second module are used as inputs for the third
	std::vector<std::string> steady_state_modules = {
		"water_vapor_properties_from_air_temperature",	// water_vapor_module
		"collatz_leaf",									// leaf_assim_module
		"penman_monteith_transpiration"					// leaf_evapo_module
	};
	
	// Initialize pointers used for some of the input and output parameters
	theta_ptr = new double;
	layer_wind_speed_ptr = new double;
	layer_relative_humidity_ptr = new double;
	Ipar_ptr = new double;
	Irad_ptr = new double;
	leaf_ass_ptr = new double;
	leaf_trans_ptr = new double;
	leaf_cond_ptr = new double;
	
	// Set up the input parameters
	// Note: the list of required inputs was quickly determined by using the following R command
	//  with the BioCro library loaded:
	//  get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	std::unordered_map<std::string, const double*> input_param_ptrs = {
		{"Catm", 					Catm_ip},
		{"Rd", 						Rd_ip},
		{"StomataWS", 				StomataWS_ip},
		{"alpha", 					alpha1_ip},
		{"b0", 						b0_ip},
		{"b1", 						b1_ip},
		{"beta", 					beta_ip},
		{"incident_irradiance", 	Irad_ptr},
		{"incident_par", 			Ipar_ptr},
		{"k_Q10", 					k_Q10_ip},
		{"kparm", 					kparm_ip},
		{"layer_wind_speed", 		layer_wind_speed_ptr},
		{"leaf_reflectance", 		leaf_reflectance_ip},
		{"leaf_transmittance", 		leaf_transmittance_ip},
		{"leafwidth", 				leafwidth_ip},
		{"lowerT", 					lowerT_ip},
		{"rh", 						layer_relative_humidity_ptr},
		{"stefan_boltzman", 		stefan_boltzman_ip},
		{"temp", 					temp_ip},
		{"theta", 					theta_ptr},
		{"upperT", 					upperT_ip},
		{"vmax", 					vmax1_ip},
		{"water_stress_approach", 	water_stress_approach_ip},
	};
	
	// Set up the output parameters
	// Note: the list of all possible outputs can be quickly determined by using
	//  the following R command with the BioCro library loaded:
	//  get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	// Here were are only interested in a few of them
	std::unordered_map<std::string, double*> output_param_ptrs = {
		{"leaf_assimilation_rate", 				leaf_ass_ptr},
		{"leaf_transpiration_rate", 			leaf_trans_ptr},
		{"leaf_stomatal_conductance",			leaf_cond_ptr}
	};
	
	// Now that the inputs are defined, make the standalone modules and store a smart pointer to them
	canopy_modules = std::shared_ptr<Standalone_SS>(new Standalone_SS(steady_state_modules, input_param_ptrs, output_param_ptrs));
}

// Destructor
big_leaf_multilayer_canopy::~big_leaf_multilayer_canopy() {
	delete theta_ptr;
	delete layer_wind_speed_ptr;
	delete layer_relative_humidity_ptr;
	delete Ipar_ptr;
	delete Irad_ptr;
	delete leaf_ass_ptr;
	delete leaf_trans_ptr;
	delete leaf_cond_ptr;
}

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
		"stefan_boltzman",			// For a standalone module
		"k_Q10",					// For a standalone module
		"vmax1",					// For a standalone module
		"alpha1",					// For a standalone module
		"temp",						// For a standalone module
		"kparm",					// For a standalone module
		"beta",						// For a standalone module
		"upperT",					// For a standalone module
		"lowerT",					// For a standalone module
		"Catm",						// For a standalone module
		"b0",						// For a standalone module
		"b1",						// For a standalone module
		"leafwidth",				// For a standalone module
		"leaf_reflectance",			// For a standalone module
		"leaf_transmittance",		// For a standalone module
		"water_stress_approach",	// For a standalone module
		"StomataWS",				// For a standalone module
		"Rd"						// For a standalone module
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
	
	const double kh = 1.0 - rh;
	const double layer_lai = lai / n_layers;
	
	// Convert doy_dbl into doy and hour
	int doy = floor(doy_dbl);
	double hour = 24.0 * (doy_dbl - doy);
	
	// Define constants
	constexpr double wind_speed_extinction = 0.7;
	constexpr double leaf_radiation_absorptivity = 0.8;
	constexpr double fraction_of_irradiance_in_PAR = 0.5;  // dimensionless.
	constexpr double joules_per_micromole_PAR = 0.235;   // J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:
	
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
	
	// Set some input parameters for the standalone modules that don't vary by layer
	*theta_ptr = theta;
	
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
		
		// Set some input parameters for the standalone modules that do vary by layer
		*layer_wind_speed_ptr = layer_wind_speed;
		*layer_relative_humidity_ptr = layer_relative_humidity;
		
		// Run a calculation for the sunlit leaves and get the resulting rates
		*Ipar_ptr = layer_sunlit_par;
		*Irad_ptr = layer_sunlit_incident_irradiance;
		canopy_modules->run();
		sunlit_leaf_assimilation_rate = *leaf_ass_ptr;
		sunlit_leaf_transpiration_rate = *leaf_trans_ptr;
		sunlit_leaf_stomatal_conductance = *leaf_cond_ptr;
		
		// Run a calculation for the shaded leaves and get the resulting rates
		*Ipar_ptr = layer_shaded_par;
		*Irad_ptr = layer_shaded_incident_irradiance;
		canopy_modules->run();
		shaded_leaf_assimilation_rate = *leaf_ass_ptr;
		shaded_leaf_transpiration_rate = *leaf_trans_ptr;
		shaded_leaf_stomatal_conductance = *leaf_cond_ptr;
		
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
	
	//TODO: Figure out why canopy_conductance is always 0. NOTE: I don't think this is true anymore (EBL)
	update(canopy_assimilation_rate_op, canopy_assimilation * 3600 * 1e-6 * 30 * 1e-6 * 10000);
	update(canopy_transpiration_rate_op, canopy_transpiration * 3600 * 1e-3 * 10000);
	update(canopy_conductance_op, canopy_conductance);
}