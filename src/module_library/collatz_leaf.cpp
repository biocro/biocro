#include "collatz_leaf.hpp"

// Constructor
collatz_leaf::collatz_leaf(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
	// Define basic module properties by passing its name to its parent class
	SteadyModule("collatz_leaf"),
	// Get pointers to input parameters
	incident_irradiance_ip(get_ip(input_parameters, "incident_irradiance")),
	incident_par_ip(get_ip(input_parameters, "incident_par")),
	temp_ip(get_ip(input_parameters, "temp")),
	vmax_ip(get_ip(input_parameters, "vmax")),
	alpha_ip(get_ip(input_parameters, "alpha")),
	kparm_ip(get_ip(input_parameters, "kparm")),
	theta_ip(get_ip(input_parameters, "theta")),
	beta_ip(get_ip(input_parameters, "beta")),
	Rd_ip(get_ip(input_parameters, "Rd")),
	upperT_ip(get_ip(input_parameters, "upperT")),
	lowerT_ip(get_ip(input_parameters, "lowerT")),
	k_Q10_ip(get_ip(input_parameters, "k_Q10")),
	Catm_ip(get_ip(input_parameters, "Catm")),
	rh_ip(get_ip(input_parameters, "rh")),
	b0_ip(get_ip(input_parameters, "b0")),
	b1_ip(get_ip(input_parameters, "b1")),
	layer_wind_speed_ip(get_ip(input_parameters, "layer_wind_speed")),
	leafwidth_ip(get_ip(input_parameters, "leafwidth")),
	water_vapor_pressure_ip(get_ip(input_parameters, "water_vapor_pressure")),
	stefan_boltzman_ip(get_ip(input_parameters, "stefan_boltzman")),
	leaf_reflectance_ip(get_ip(input_parameters, "leaf_reflectance")),
	leaf_transmittance_ip(get_ip(input_parameters, "leaf_transmittance")),
	water_stress_approach_ip(get_ip(input_parameters, "water_stress_approach")),
	StomataWS_ip(get_ip(input_parameters, "StomataWS")),
	slope_water_vapor_ip(get_ip(input_parameters, "slope_water_vapor")),
	psychrometric_parameter_ip(get_ip(input_parameters, "psychrometric_parameter")),
	latent_heat_vaporization_of_water_ip(get_ip(input_parameters, "latent_heat_vaporization_of_water")),
	vapor_density_deficit_ip(get_ip(input_parameters, "vapor_density_deficit")),
	// Get pointers to output parameters
	leaf_assimilation_rate_op(get_op(output_parameters, "leaf_assimilation_rate")),
	leaf_stomatal_conductance_op(get_op(output_parameters, "leaf_stomatal_conductance")),
	leaf_boundary_layer_conductance_op(get_op(output_parameters, "leaf_boundary_layer_conductance")),
	leaf_temperature_op(get_op(output_parameters, "leaf_temperature")),
	Ci_op(get_op(output_parameters, "Ci")),
	leaf_net_irradiance_op(get_op(output_parameters, "leaf_net_irradiance"))
{
	// Here we need to initialize a standalone module to calculate the leaf temperature
	
	// Make the steady state module list
	std::vector<std::string> steady_state_modules = {
		"penman_monteith_leaf_temperature"
	};
	
	// Initialize pointers used for some input and output parameters
	boundary_layer_conductance_ptr = new double;
	gs_ptr = new double;
	leaf_net_irradiance_ptr = new double;
	leaf_temperature_ptr = new double;
	
	// Set up the input parameters
	// Note: the list of required inputs was quickly determined by using the following R command
	//  with the BioCro library loaded:
	//  get_standalone_ss_info("penman_monteith_leaf_temperature")
	std::unordered_map<std::string, const double*> input_param_ptrs {
		{"latent_heat_vaporization_of_water", 	latent_heat_vaporization_of_water_ip},
		{"leaf_boundary_layer_conductance", 	boundary_layer_conductance_ptr},
		{"leaf_net_irradiance", 				leaf_net_irradiance_ptr},
		{"leaf_stomatal_conductance", 			gs_ptr},
		{"psychrometric_parameter", 			psychrometric_parameter_ip},
		{"slope_water_vapor", 					slope_water_vapor_ip},
		{"temp", 								temp_ip},
		{"vapor_density_deficit", 				vapor_density_deficit_ip}
	};
	
	// Set up the output parameters
	std::unordered_map<std::string, double*> output_param_ptrs = {
		{"leaf_temperature", 					leaf_temperature_ptr},
	};
	
	// Now that the inputs are defined, make the standalone modules and store a smart pointer to them
	leaf_temperature_module = std::shared_ptr<Standalone_SS>(new Standalone_SS(steady_state_modules, input_param_ptrs, output_param_ptrs));
}

// Destructor
collatz_leaf::~collatz_leaf() {
	delete boundary_layer_conductance_ptr;
	delete gs_ptr;
	delete leaf_net_irradiance_ptr;
	delete leaf_temperature_ptr;
}

// Inputs
std::vector<std::string> collatz_leaf::get_inputs() {
	return {
		"incident_irradiance",
		"incident_par",
		"temp",
		"vmax",
		"alpha",
		"kparm",
		"theta",
		"beta",
		"Rd",
		"upperT",
		"lowerT",
		"k_Q10",
		"Catm",
		"rh",
		"b0",
		"b1",
		"layer_wind_speed",
		"leafwidth",
		"water_vapor_pressure",
		"stefan_boltzman",
		"leaf_reflectance",
		"leaf_transmittance",
		"water_stress_approach",
		"StomataWS",
		"slope_water_vapor",					// Required for penman_monteith_leaf_temperature
		"psychrometric_parameter",				// Required for penman_monteith_leaf_temperature
		"latent_heat_vaporization_of_water",	// Required for penman_monteith_leaf_temperature
		"vapor_density_deficit"					// Required for penman_monteith_leaf_temperature
	};
}

// Outputs
std::vector<std::string> collatz_leaf::get_outputs() {
	return {
		"leaf_assimilation_rate",
		"leaf_stomatal_conductance",
		"leaf_boundary_layer_conductance",
		"leaf_temperature",
		"Ci",
		"leaf_net_irradiance"
	};
}

// Module operation
void collatz_leaf::do_operation() const {	
	// Collect inputs and make calculations
	double incident_irradiance = *incident_irradiance_ip;
	double incident_par = *incident_par_ip;
	double temp = *temp_ip;	// air temperature
	double vmax = *vmax_ip;
	double alpha = *alpha_ip;
	double kparm = *kparm_ip;
	double theta = *theta_ip;
	double beta = *beta_ip;
	double Rd = *Rd_ip;
	double upperT = *upperT_ip;
	double lowerT = *lowerT_ip;
	double k_Q10 = *k_Q10_ip;
	double Catm = *Catm_ip;
	double rh = *rh_ip;
	double b0 = *b0_ip;
	double b1 = *b1_ip;
	double layer_wind_speed = *layer_wind_speed_ip;
	double leafwidth = *leafwidth_ip;
	double water_vapor_pressure = *water_vapor_pressure_ip;
	double stefan_boltzman = *stefan_boltzman_ip;
	double leaf_reflectance = *leaf_reflectance_ip;
	double leaf_transmittance = *leaf_transmittance_ip;
	double water_stress_approach = *water_stress_approach_ip;
	double StomataWS = *StomataWS_ip;
	
	double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
	
	// Initial guesses
	double intercelluar_co2_molar_fraction = Catm * 0.4;  // micromole / mol
	
	double leaf_temperature = temp;
	
	struct collatz_result r = collatz_photo(incident_par, leaf_temperature, vmax,
			alpha, kparm, theta, beta, Rd,
			upperT, lowerT, k_Q10, intercelluar_co2_molar_fraction);
	
	double gs = ball_berry(r.assimilation * 1e-6, Catm * 1e-6, rh, b0, b1);  // mmol / m^2 / s
	double conductance_in_m_per_s = gs * 1e-3 * volume_of_one_mole_of_air;  // m / s
	
	double boundary_layer_conductance = leaf_boundary_layer_conductance(layer_wind_speed,
			leafwidth, temp, leaf_temperature - temp, conductance_in_m_per_s, water_vapor_pressure);  // m / s
	
	double absorbed_irradiance = incident_irradiance * (1 - leaf_reflectance - leaf_transmittance) / (1 - leaf_transmittance);  // W / m^2. Leaf area basis.
	
	double black_body_radiation = 0;  // W / m^2. Leaf area basis.
	
	double leaf_net_irradiance = absorbed_irradiance;  // W / m^2. Leaf area basis.
	
	// Convergence iteration
	unsigned int constexpr max_iterations = 50;
	for (unsigned int n = 0; n < max_iterations; ++n) {
		// Store the previous assimilation rate
		double const previous_assimilation = r.assimilation;  // micromole / m^2 / s
		
		// Collect new inputs for the penman monteith leaf temperature model
		leaf_net_irradiance = absorbed_irradiance - black_body_radiation;  // W / m^2. Leaf area basis.
		
		// Update the penman monteith leaf temperature module input parameter values
		*gs_ptr = gs;
		*boundary_layer_conductance_ptr = boundary_layer_conductance;
		*leaf_net_irradiance_ptr = leaf_net_irradiance;
		
		// Get the new leaf temperature
		leaf_temperature_module->run();
		leaf_temperature = *leaf_temperature_ptr;
		
		// Update Ci
		double constexpr ratio = 1.6;  // The ratio of the diffusivities of H2O and CO2 in air.
		intercelluar_co2_molar_fraction = Catm - r.assimilation / gs * 1e3 * ratio;  // micromole / mol
		intercelluar_co2_molar_fraction = fmax(0, intercelluar_co2_molar_fraction);
		
		// Calculate new assimilation rate
		r = collatz_photo(incident_par, leaf_temperature, vmax,
				alpha, kparm, theta, beta, Rd,
				upperT, lowerT, k_Q10, intercelluar_co2_molar_fraction);
		
		// Apply water stress
		if (water_stress_approach == 0) r.assimilation *= StomataWS;
		if (water_stress_approach == 1) gs *= StomataWS;
		
		// Check for stability
		if (n > max_iterations - 10)
				gs = b0 * 1e3;  // mmol / m^2 / s. If it has gone through this many iterations, the convergence is not stable. This convergence is inapproriate for high water stress conditions, so use the minimum gs to try to get a stable system.
		
		// Update other things
		conductance_in_m_per_s = gs * 1e-3 * volume_of_one_mole_of_air;  // m / s
		
		boundary_layer_conductance = leaf_boundary_layer_conductance(layer_wind_speed, leafwidth, temp, leaf_temperature - temp, conductance_in_m_per_s, water_vapor_pressure);  // m / s
		
		double delta_t = leaf_temperature - temp;
		
		black_body_radiation = 4 * stefan_boltzman * pow(273.15 + temp, 3) * delta_t;  // W / m^2. Leaf area basis.
		
		// black_body_radiation = net long wave radiation emittted per second = radiation emitted per second - radiation absorbed per second = sigma * (Tair + deltaT)^4 - sigma * Tair^4
		// To make it a linear function of deltaT, do a Taylor series about deltaT = 0 and keep only the zero and first order terms.
		// black_body_radiation = sigma * (Tair - 0)^4 - sigma * Tair^4 + deltaT * (4 * sigma * Tair^3) = 4 * sigma * Tair^3 * deltaT
		// where 4 * sigma * Tair^3 is the derivative of sigma * (Tair + deltaT)^4 evaluated at deltaT = 0
		
		double constexpr tol = 0.01;  // micromole / m^2 / s
		double const diff = fabs(previous_assimilation - r.assimilation);
		if (diff <= tol) break;
	}
	
	// Update the output parameter list
	update(leaf_assimilation_rate_op, r.assimilation);
	update(leaf_stomatal_conductance_op, gs);
	update(leaf_boundary_layer_conductance_op, boundary_layer_conductance);
	update(leaf_temperature_op, leaf_temperature);
	update(Ci_op, intercelluar_co2_molar_fraction);
	update(leaf_net_irradiance_op, leaf_net_irradiance);
}