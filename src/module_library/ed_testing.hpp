#ifndef ED_TESTING_H
#define ED_TESTING_H

#include "../modules.h"
//#include <Rinternals.h>		// Temporary, for testing

///////////////////////////////////////
// BASE CLASS FOR MULTILAYER MODULES //
///////////////////////////////////////

// An abstract class with several private static member functions
// The purpose of this class is to give a module some new functions
//  used with multilayer parameter lists

class MultilayerModule {
	public:
		MultilayerModule() {}
		virtual ~MultilayerModule() = 0;
	protected:
		static std::vector<std::string> get_multilayer_param_names(int nlayers, std::vector<std::string> param_names);	// Should be protected so only child classes can access it
		const std::vector<double*> get_multilayer_op(std::unordered_map<std::string, double>* output_parameters, int nlayers, const std::string& name) const;
		const std::vector<const double*> get_multilayer_ip(const std::unordered_map<std::string, double>* input_parameters, int nlayers, const std::string& name) const;
};

inline MultilayerModule::~MultilayerModule() {}

std::vector<std::string> MultilayerModule::get_multilayer_param_names(int nlayers, std::vector<std::string> param_names) {
	// This function generates a list of parameter names based on the number of layers
	char buff[128];	// Hopefully no parameter names are this long!
	std::vector<std::string> full_multilayer_output_vector;
	for(size_t i = 0; i < param_names.size(); i++) {
		std::string basename = param_names[i];
		for(int j = 0; j < nlayers; j++) {
			if(nlayers < 10) sprintf(buff, "%.1i", j);
			else sprintf(buff, "%.2i", j);
			full_multilayer_output_vector.push_back(basename + std::string(buff));
		}
	}
	return full_multilayer_output_vector;
}

const std::vector<double*> MultilayerModule::get_multilayer_op(std::unordered_map<std::string, double>* output_parameters, int nlayers, const std::string& name) const {
	std::vector<std::string> param_base_name_vector(1, name);
	std::vector<std::string> param_names = get_multilayer_param_names(nlayers, param_base_name_vector);
	std::vector<double*> multilayer_op(nlayers);
	for(int i = 0; i < nlayers; i++) {
		std::string layer_name = param_names[i];
		if(output_parameters->find(layer_name) != output_parameters->end()) multilayer_op[i] = &((*output_parameters).at(layer_name));
		else throw std::logic_error(layer_name);
	}
	return multilayer_op;
}

const std::vector<const double*> MultilayerModule::get_multilayer_ip(const std::unordered_map<std::string, double>* input_parameters, int nlayers, const std::string& name) const {
	std::vector<std::string> param_base_name_vector(1, name);
	std::vector<std::string> param_names = get_multilayer_param_names(nlayers, param_base_name_vector);
	std::vector<const double*> multilayer_ip(nlayers);
	for(int i = 0; i < nlayers; i++) {
		std::string layer_name = param_names[i];
		if(input_parameters->find(layer_name) != input_parameters->end()) multilayer_ip[i] = &((*input_parameters).at(layer_name));
		else throw std::logic_error(layer_name);
	}
	return multilayer_ip;
}

////////////////////////
// SOLAR ZENITH ANGLE //
////////////////////////

class solar_zenith_angle : public SteadyModule {
	public:
		solar_zenith_angle(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("solar_zenith_angle"),
			// Get pointers to input parameters
			lat_ip(get_ip(input_parameters, "lat")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			// Get pointers to output parameters
			cosine_zenith_angle_op(get_op(output_parameters, "cosine_zenith_angle"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* lat_ip;
		const double* doy_dbl_ip;
		// Pointers to output parameters
		double* cosine_zenith_angle_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> solar_zenith_angle::get_inputs() {
	return {
		"lat",
		"doy_dbl"
	};
}

std::vector<std::string> solar_zenith_angle::get_outputs() {
	return {
		"cosine_zenith_angle"
	};
}

void solar_zenith_angle::do_operation() const {
	// Collect inputs and make calculations
	const double lat = *lat_ip;
	const double doy_dbl = *doy_dbl_ip;
	
	// Unpack the doy and hour
	const double doy = floor(doy_dbl);
	const double hour = 24.0 * (doy_dbl - doy);
	
	// Convert latitude to radians (=phi)
	const double phi = lat * M_PI / 180.0;
	
	// Determine the number of days since the December solstice (=NDS)
	//  January 1 => DOY = 0
	//  December 21 => DOY = 355
	// We assume 365 days per year with solstice on December 21
	const double NDS = doy + 10.0;
	
	// Find the angular position of the Earth relative to the Sun in
	//   radians (=omega), where omega = 0 corresponds to the winter solstice
	const double omega = (NDS / 365.0) * (2 * M_PI);
	
	// Find the sun's declination in radians (=delta) using the Earth's axial tilt (23.5 degrees)
	constexpr double axial_tilt = 23.5 * M_PI / 180.0;
	const double delta = -axial_tilt * cos(omega);
	
	// Find the hour angle in radians (=tau), noting that the Earth rotates once every
	//  24 hours, i.e., turns at a rate of 2 * pi / 24 = pi / 12 radians per hour
	// Here we don't apply any corrections to calculate noon and assume it occurs at 12:00,
	//  in contrast to equation 11.3 in Campbell & Norman (1998)
	constexpr int solar_noon = 12;
	const double tau = (hour - solar_noon) * M_PI / 12.0;
	
	// Find the cosine of the solar zenith angle using a straight-forward application
	//  of the law of cosines for spherical triangles, substituting cofunctions of coangles
	//  in the case of latitude and declination
	// See, e.g., equation 11.1 in Campbell & Norman (1998)
	const double cosine_zenith_angle = sin(delta) * sin(phi) + cos(delta) * cos(phi) * cos(tau);
	
	// Update the output parameter list
	update(cosine_zenith_angle_op, cosine_zenith_angle);
}

/////////////////////////////
// LIGHT MACRO ENVIRONMENT //
/////////////////////////////

class light_macro_environment : public SteadyModule {
	public:
		light_macro_environment(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("light_macro_environment"),
			// Get pointers to input parameters
			cosine_zenith_angle_ip(get_ip(input_parameters, "cosine_zenith_angle")),
			atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
			// Get pointers to output parameters
			direct_irradiance_fraction_op(get_op(output_parameters, "direct_irradiance_fraction")),
			diffuse_irradiance_fraction_op(get_op(output_parameters, "diffuse_irradiance_fraction"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* cosine_zenith_angle_ip;
		const double* atmospheric_pressure_ip;
		// Pointers to output parameters
		double* direct_irradiance_fraction_op;
		double* diffuse_irradiance_fraction_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> light_macro_environment::get_inputs() {
	return {
		"cosine_zenith_angle",
		"atmospheric_pressure"
	};
}

std::vector<std::string> light_macro_environment::get_outputs() {
	return {
		"direct_irradiance_fraction",
		"diffuse_irradiance_fraction"
	};
}

void light_macro_environment::do_operation() const {
	// Collect inputs and make calculations
	const double cosine_zenith_angle = *cosine_zenith_angle_ip;
	const double local_atmospheric_pressure = *atmospheric_pressure_ip;
	
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
		constexpr double atmospheric_transmittance = 0.85;												// dimensionless. (note: this is higher than the values in Campbell & Norman, indicating a very clear day)
		constexpr double atmospheric_pressure_at_sea_level = 101325;									// Pa.
		const double pressure_ratio = local_atmospheric_pressure / atmospheric_pressure_at_sea_level;	// dimensionless.
		constexpr double proportion_of_irradiance_scattered = 0.3;										// dimensionless.
		direct_irradiance_transmittance = pow(atmospheric_transmittance, (pressure_ratio / cosine_zenith_angle));								// Modified from equation 11.11 of Norman and Campbell.
		diffuse_irradiance_transmittance = proportion_of_irradiance_scattered * (1 - direct_irradiance_transmittance) * cosine_zenith_angle;	// Modified from equation 11.13 of Norman and Campbell.
	}
	
	// Update the output parameter list
	update(direct_irradiance_fraction_op, direct_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance));		// dimensionless.
	update(diffuse_irradiance_fraction_op, diffuse_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance));	// dimensionless.
}

/////////////////////////////////////////////////
// BASE CLASS FOR MULTILAYER CANOPY PROPERTIES //
/////////////////////////////////////////////////

// A child class of SteadyModule used to represent a canopy properties module
//  module with an arbitrary number of layers.
// Note that this module cannot be directly instantiated using the module factory.
// Instead, it requires children that represent definite numbers of layers.

class multilayer_canopy_properties : public SteadyModule, public MultilayerModule {
	public:
		multilayer_canopy_properties(const std::string & module_name, const int & nlayers, const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule(module_name),
			// Add the multilayer module functions
			MultilayerModule(),
			// Store the number of layers
			_nlayers(nlayers),
			// Get pointers to input parameters
			cosine_zenith_angle_ip(get_ip(input_parameters, "cosine_zenith_angle")),
			direct_irradiance_fraction_ip(get_ip(input_parameters, "direct_irradiance_fraction")),
			diffuse_irradiance_fraction_ip(get_ip(input_parameters, "diffuse_irradiance_fraction")),
			lai_ip(get_ip(input_parameters, "lai")),
			kpLN_ip(get_ip(input_parameters, "kpLN")),
			LeafN_ip(get_ip(input_parameters, "LeafN")),
			rh_ip(get_ip(input_parameters, "rh")),
			windspeed_ip(get_ip(input_parameters, "windspeed")),
			chil_ip(get_ip(input_parameters, "chil")),
			solar_ip(get_ip(input_parameters, "solar")),
			kd_ip(get_ip(input_parameters, "kd")),
			heightf_ip(get_ip(input_parameters, "heightf")),
			// Get pointers to output parameters
			layer_cum_lai_ops(get_multilayer_op(output_parameters, nlayers, "layer_cum_lai_")),
			layer_LeafN_ops(get_multilayer_op(output_parameters, nlayers, "layer_LeafN_")),
			layer_windspeed_ops(get_multilayer_op(output_parameters, nlayers, "layer_windspeed_")),
			layer_Iscat_ops(get_multilayer_op(output_parameters, nlayers, "layer_Iscat_")),
			layer_Idiff_ops(get_multilayer_op(output_parameters, nlayers, "layer_Idiff_")),
			layer_Fsun_ops(get_multilayer_op(output_parameters, nlayers, "layer_Fsun_")),
			layer_Fshade_ops(get_multilayer_op(output_parameters, nlayers, "layer_Fshade_")),
			layer_Iaverage_ops(get_multilayer_op(output_parameters, nlayers, "layer_Iaverage_")),
			layer_rh_ops(get_multilayer_op(output_parameters, nlayers, "layer_rh_")),
			layer_height_ops(get_multilayer_op(output_parameters, nlayers, "layer_height_"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Number of layers
		const int _nlayers;
		// Pointers to input parameters
		const double* cosine_zenith_angle_ip;
		const double* direct_irradiance_fraction_ip;
		const double* diffuse_irradiance_fraction_ip;
		const double* lai_ip;
		const double* kpLN_ip;
		const double* LeafN_ip;
		const double* rh_ip;
		const double* windspeed_ip;
		const double* chil_ip;
		const double* solar_ip;
		const double* kd_ip;
		const double* heightf_ip;
		// Pointers to output parameters
		const std::vector<double*> layer_cum_lai_ops;
		const std::vector<double*> layer_LeafN_ops;
		const std::vector<double*> layer_windspeed_ops;
		const std::vector<double*> layer_Iscat_ops;
		const std::vector<double*> layer_Idiff_ops;
		const std::vector<double*> layer_Fsun_ops;
		const std::vector<double*> layer_Fshade_ops;
		const std::vector<double*> layer_Iaverage_ops;
		const std::vector<double*> layer_rh_ops;
		const std::vector<double*> layer_height_ops;
	protected:
		static std::vector<std::string> generate_inputs(int nlayers);
		static std::vector<std::string> generate_outputs(int nlayers);
		void run() const;
};

std::vector<std::string> multilayer_canopy_properties::generate_inputs(int nlayers) {
	// The inputs don't actually depend on the number of layers, so just return
	//  a list of parameter names
	return {
		"cosine_zenith_angle",
		"direct_irradiance_fraction",
		"diffuse_irradiance_fraction",
		"lai",
		"kpLN",
		"LeafN",
		"rh",
		"windspeed",
		"chil",
		"solar",
		"kd",
		"heightf"
	};
}

std::vector<std::string> multilayer_canopy_properties::generate_outputs(int nlayers) {
	// Define the list of parameters that exist for each layer
	std::vector<std::string> multilayer_params = {
		"layer_cum_lai_",
		"layer_LeafN_",
		"layer_windspeed_",
		"layer_Iscat_",
		"layer_Idiff_",
		"layer_Fsun_",
		"layer_Fshade_",
		"layer_Iaverage_",
		"layer_rh_",
		"layer_height_"
	};
	// Get a full set of multilayer parameter names, based on the number of layers
	std::vector<std::string> outputs = MultilayerModule::get_multilayer_param_names(nlayers, multilayer_params);
	return outputs;
}

void multilayer_canopy_properties::run() const {
	// Collect inputs and make calculations
	const double cosine_zenith_angle = *cosine_zenith_angle_ip;
	const double direct_irradiance_fraction = *direct_irradiance_fraction_ip;
	const double diffuse_irradiance_fraction = *diffuse_irradiance_fraction_ip;
	const double lai = *lai_ip;
	const double kpLN = *kpLN_ip;
	const double LeafN = *LeafN_ip;
	const double rh = *rh_ip;
	const double windspeed = *windspeed_ip;
	const double chil = *chil_ip;
	const double solar = *solar_ip;
	const double kd = *kd_ip;
	const double heightf = *heightf_ip;
	
	// Define some quantities that don't change with each layer
	constexpr double alphascatter = 0.8;	// See discussion of Eq. 15.6 in Campbell & Norman (1998)
	
	// Determine the canopy extinction coefficient from the leaf shape
	//  and solar zenith angle. See e.g. Eq. 15.4 in Campbell & Norman (1998)
	// Here we make use of the fact that if x = cos(theta), then tan^2(theta) = 
	//  (1 - x^2) / x^2
	double k = sqrt(chil * chil + (1.0 - cosine_zenith_angle * cosine_zenith_angle) / (cosine_zenith_angle * cosine_zenith_angle));		// Calculate numerator
	k /= chil + 1.744 * pow((chil + 1.183), -0.733);																					// Divide by denominator
	
	// Determine the direct and diffuse photon flux density
	const double Idir = direct_irradiance_fraction * solar;		// micromole / m^2 / s. Flux through a plane perpendicular to the rays of the sun.
	const double Idiff = diffuse_irradiance_fraction * solar;
	
	// Determine the beam irradiance, i.e. the direct photon flux density incident on the canopy
	// This is Lambert's law; see e.g. Eq. 10.3 in Campbell & Norman (1998)
	const double Ibeam = Idir * cosine_zenith_angle;
	
	// Determine the intensity of light directly falling on the sunlit leaves
	double Isun = Ibeam * k;
	
	// Calculate canopy dynamics at each layer and update the output parameters accordingly
	for (int i = 0; i < _nlayers; ++i) {
		// Find the cumulative LAI at the layer midpoint
		//  Cumulative LAI is zero at the canopy bottom and linearly increases to LAI at the top
		//  i = 0 represents the lowest layer and i = nlayers - 1 is the highest
		//  If we imagine a height h along the canopy running from 0 to 1, the ith layer stretches
		//   from h = i / nlayers to (i + 1) / nlayers, so its midpoint is at h = (i + 0.5) / nlayers
		double layer_cum_lai = lai * (i + 0.5) / _nlayers;
		
		// Find the leaf nitrogen and windspeed values for the current layer,
		//  based on its cumulative LAI value
		// Note: this is a little bit different than the previous BioCro version,
		//  where LeafN and windspeed were calculated using the cumulative LAI at
		//  the layer's top rather than its midpoint
		double layer_LeafN = LeafN * exp(-kpLN * (lai - layer_cum_lai));			// Nitrogen decreases exponentially away from the canopy top with decay coefficient kpLN
		double layer_windspeed = windspeed * exp(-0.7 * (lai - layer_cum_lai));		// Windspeed decreases exponentially away from the canopy top with a hardcoded decay coefficient of 0.7
		
		// Calculate the intensity of scattered light in this canopy layer
		//  The first term represents the unintercepted beam & down-scattered light incident on this layer
		//  To find just the scattered light, we need to subtract another term representing just the unintercepted beam
		//  Note that both terms decrease away from the canopy top
		//  See Eq. 15.20 in Campbell & Norman (1998) and associated discussion
		double layer_Iscat = Ibeam * (exp(-k * sqrt(alphascatter) * (lai - layer_cum_lai)) - exp(-k * (lai - layer_cum_lai)));
		
		// Calculate the total amount of diffuse light in this canopy layer
		// There are two contributions:
		//  (1) Scattered light from other layers, as calculated above
		//  (2) Diffuse light from the atmosphere
		// The second contribution is calculated using Eq. 15.6 from Campbell & Norman (1998)
		//  with alpha=1 and Kbe(phi) = kd
		double layer_Idiff = layer_Iscat + Idiff * exp(-kd * (lai - layer_cum_lai));
		
		// Calculate the amounts of sunlit and shaded LAI in this canopy layer
		//  using Equation 15.21 in Campbell & Norman (1998), where lai / nlayers
		//  represents the total LAI in this layer.
		// Note that Eq. 15.22 is not appropriate since the layer thickness is not
		//  infinitesimally small in BioCro.
		double layer_sunlit_lai = exp(-k * (lai - layer_cum_lai)) * (1 - exp(-k * lai / _nlayers)) / k;
		double layer_Fsun = layer_sunlit_lai / (lai / _nlayers);
		double layer_Fshade = 1 - layer_Fsun;
		
		// Calculate something (??)
		double layer_Iaverage = (layer_Fsun * (Isun + layer_Idiff) + layer_Fshade * layer_Idiff) * (1 - exp(-k * lai / _nlayers)) / k;
		
		// For values of cosTheta close to or less than 0, in place of the
		// calculations above, we want to use the limits of the above
		// expressions as cosTheta approaches 0 from the right:
		if (cosine_zenith_angle <= 1E-10) {
			Isun = Idir / (chil + 1.744 * pow((chil + 1.183), -0.733));
			layer_Idiff = Idiff * exp(-kd * (lai - layer_cum_lai));
			layer_Fsun = 0;
			layer_Fshade = 1;
			layer_Iaverage = 0;
		}
		
		// Find the residual humidity for the current layer
		double layer_rh = rh * exp(-(1.0 - rh) * (1.0 + i) / _nlayers);				// Residual humidity decreases exponentially away from the canopy top with a decay coefficient of 1 - rh (why??)
		
		// Find the height of the current layer
		double layer_height = layer_cum_lai / heightf;
		
		// Update the output parameters
		update(layer_cum_lai_ops[i], layer_cum_lai);
		update(layer_LeafN_ops[i], layer_LeafN);
		update(layer_windspeed_ops[i], layer_windspeed);
		update(layer_Iscat_ops[i], layer_Iscat);
		update(layer_Idiff_ops[i], layer_Idiff);
		update(layer_Fsun_ops[i], layer_Fsun);
		update(layer_Fshade_ops[i], layer_Fshade);
		update(layer_Iaverage_ops[i], layer_Iaverage);
		update(layer_rh_ops[i], layer_rh);
		update(layer_height_ops[i], layer_height);
	}
}

////////////////////////////////////////
// TEN LAYER CANOPY PROPERTIES MODULE //
////////////////////////////////////////

// A child class of multilayer_test where the number of layers has been defined.
// Instances of this class can be created using the module factory.

class ten_layer_canopy_properties : public multilayer_canopy_properties {
	public:
		ten_layer_canopy_properties(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			multilayer_canopy_properties("ten_layer_canopy_properties", ten_layer_canopy_properties::nlayers, input_parameters, output_parameters)		// Create the base class with the appropriate number of layers
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Number of layers
		static const int nlayers;
		// Main operation
		void do_operation() const;
};

const int ten_layer_canopy_properties::nlayers = 10;		// Set the number of layers

std::vector<std::string> ten_layer_canopy_properties::get_inputs() {
	return multilayer_canopy_properties::generate_inputs(ten_layer_canopy_properties::nlayers);		// Just call the parent class's input function with the appropriate number of layers
}

std::vector<std::string> ten_layer_canopy_properties::get_outputs() {
	return multilayer_canopy_properties::generate_outputs(ten_layer_canopy_properties::nlayers);	// Just call the parent class's output function with the appropriate number of layers
}

void ten_layer_canopy_properties::do_operation() const {
	multilayer_canopy_properties::run();															// Just call the parent class's run operation
}

//////////////////////////////////////////////
// BASE CLASS FOR MULTILAYER TESTING MODULE //
//////////////////////////////////////////////

// A child class of SteadyModule used to represent a "multilayer_test" module
//  with an arbitrary number of layers

class multilayer_test : public SteadyModule, public MultilayerModule {
	public:
		multilayer_test(const std::string & module_name, const int & nlayers, const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule(module_name),
			// Add the multilayer module functions
			MultilayerModule(),
			// Store the number of layers
			_nlayers(nlayers),
			// Get pointers to non-multilayer input parameters
			other_test_input_param_ip(get_ip(input_parameters, "other_test_input_param")),
			// Get pointers to multilayer input parameters
			test_input_ips(get_multilayer_ip(input_parameters, nlayers, "test_input_layer_")),
			// Get pointers to non-multilayer output parameters
			other_test_output_param_op(get_op(output_parameters, "other_test_output_param")),
			// Get pointers to multilayer output parameters
			test_output_ops(get_multilayer_op(output_parameters, nlayers, "test_output_layer_"))
		{}
	private:
		// Number of layers
		const int _nlayers;
		// Pointers to input parameters
		const double* other_test_input_param_ip;
		const std::vector<const double*> test_input_ips;
		// Pointers to output parameters
		double* other_test_output_param_op;
		const std::vector<double*> test_output_ops;
	protected:
		static std::vector<std::string> generate_inputs(int nlayers);
		static std::vector<std::string> generate_outputs(int nlayers);
		void run() const;
};

std::vector<std::string> multilayer_test::generate_inputs(int nlayers) {
	std::vector<std::string> inputs = MultilayerModule::get_multilayer_param_names(nlayers, {"test_input_layer_"});
	inputs.push_back("other_test_input_param");
	return inputs;
}

std::vector<std::string> multilayer_test::generate_outputs(int nlayers) {
	std::vector<std::string> outputs = MultilayerModule::get_multilayer_param_names(nlayers, {"test_output_layer_"});
	outputs.push_back("other_test_output_param");
	return outputs;
}

void multilayer_test::run() const {
	// Collect inputs
	double other_test_input_param = *other_test_input_param_ip;
	
	// Make layer-based calculations and update the outputs
	for(int i = 0; i < _nlayers; i++) {
		double current_value = *(test_input_ips[i]);
		update(test_output_ops[i], 3 * other_test_input_param * current_value);
	}
	
	// Update other outputs
	update(other_test_output_param_op, 2 * other_test_input_param);
}

///////////////////////////////////////
// N LAYER MULTILAYER TESTING MODULE //
///////////////////////////////////////

// A child class of multilayer_test where the number of layers has been defined.
// Unlike "multilayer_test" and "MultilayerModule", instances of this class can
//  be created using the module factory.

class n_layer_multi_test : public multilayer_test {
	public:
		n_layer_multi_test(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			multilayer_test("n_layer_multi_test", n_layer_multi_test::nlayers, input_parameters, output_parameters)		// Create the base class with the appropriate number of layers
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Number of layers
		static const int nlayers;
		// Main operation
		void do_operation() const;
};

const int n_layer_multi_test::nlayers = 12;		// Set the number of layers

std::vector<std::string> n_layer_multi_test::get_inputs() {
	return multilayer_test::generate_inputs(n_layer_multi_test::nlayers);	// Just call the parent class's input function with the appropriate number of layers
}

std::vector<std::string> n_layer_multi_test::get_outputs() {
	return multilayer_test::generate_outputs(n_layer_multi_test::nlayers);	// Just call the parent class's output function with the appropriate number of layers
}

void n_layer_multi_test::do_operation() const {
	multilayer_test::run();													// Just call the parent class's run operation
}

/*

///////////////////////////////////////
// BASE CLASS FOR MULTILAYER MODULES //
///////////////////////////////////////

class MultilayerModule : public SteadyModule {
	public:
		MultilayerModule(const std::string& module_name, std::vector<std::string> multilayer_outputs, int nlayers, bool adaptive_compatible = true) :
			SteadyModule(module_name, adaptive_compatible),
			_multilayer_outputs(multilayer_outputs),
			_nlayers(nlayers)
		{
			if(nlayers < 1) throw std::logic_error(std::string("Thrown by MultilayerModule: nlayers must be positive\n"));
			if(nlayers > 99) throw std::logic_error(std::string("Thrown by MultilayerModule: nlayers must be < 100\n"));
		}
	private:
		const std::vector<std::string> _multilayer_outputs;
		const int _nlayers;
	protected:
		std::vector<std::string> get_multilayer_outputs() const;	// Should be protected so only child classes can access it
}

MultilayerModule::get_multilayer_outputs() const {
	// This function generates a list of output parameters based on the number of layers
	// It is intended to be used as part of the static get_outputs function of any child module classes
	char buff[128];	// Hopefully no parameter names are this long!
	std::vector<std::string> full_multilayer_output_vector;
	for(size_t i = 0; i < _multilayer_outputs.size(); i++) {
		std::string basename = _multilayer_outputs[i];
		for(int j = 0; j < _nlayers; j++) {
			if(_nlayers < 10) sprintf(buff, "%.1i", j);
			else sprintf(buff, "%.2i", j);
			full_multilayer_output_vector.push_back(basename + std::string(buff));
		}
	}
	return full_multilayer_output_vector;
}

//////////////////////////////////////////////
// BASE CLASS FOR MUTLILAYER TESTING MODULE //
//////////////////////////////////////////////

class multilayer_test : public MultilayerModule {
	public:
		multilayer_test(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters, int nlayers) :
			// Define basic module properties by passing its name, multilayer outputs, and layer number to its parent class
			MultilayerModule("multilayer_test", {"test_param_layer_"}, nlayers)
			// Get pointers to input parameters
			// Get pointers to output parameters
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		// Pointers to output parameters
		// Main operation
		void do_operation() const;
};

std::vector<std::string> multilayer_test::get_inputs() {
	return {
		"cosine_zenith_angle",
		"atmospheric_pressure"
	};
}

std::vector<std::string> multilayer_test::get_outputs() {
	return {
		"direct_irradiance_fraction",
		"diffuse_irradiance_fraction"
	};
}

void multilayer_test::do_operation() const {
	// Collect inputs and make calculations
	const double cosine_zenith_angle = *cosine_zenith_angle_ip;
	const double local_atmospheric_pressure = *atmospheric_pressure_ip;
	
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
		constexpr double atmospheric_transmittance = 0.85;												// dimensionless. (note: this is higher than the values in Campbell & Norman, indicating a very clear day)
		constexpr double atmospheric_pressure_at_sea_level = 101325;									// Pa.
		const double pressure_ratio = local_atmospheric_pressure / atmospheric_pressure_at_sea_level;	// dimensionless.
		constexpr double proportion_of_irradiance_scattered = 0.3;										// dimensionless.
		direct_irradiance_transmittance = pow(atmospheric_transmittance, (pressure_ratio / cosine_zenith_angle));								// Modified from equation 11.11 of Norman and Campbell.
		diffuse_irradiance_transmittance = proportion_of_irradiance_scattered * (1 - direct_irradiance_transmittance) * cosine_zenith_angle;	// Modified from equation 11.13 of Norman and Campbell.
	}
	
	// Update the output parameter list
	update(direct_irradiance_fraction_op, direct_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance));	// dimensionless.
	update(diffuse_irradiance_fraction_op, diffuse_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance))	// dimensionless.
}

*/

#endif /* ED_TESTING_H */