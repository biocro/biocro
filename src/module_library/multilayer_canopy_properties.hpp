#ifndef MULTILAYER_CANOPY_PROPERTIES_H
#define MULTILAYER_CANOPY_PROPERTIES_H

#include "../modules.h"

// multilayer_canopy_properties is a child of SteadyModule and MultilayerModule
//  It represents a canopy properties module with an arbitrary number of layers
//  Note that this module cannot be directly instantiated by the module factory
//  Instead, it requires children that represent definite numbers of layers,
//   such as ten_layer_canopy_module

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
			alpha_scatter_ip(get_ip(input_parameters, "alpha_scatter")),
			// Get pointers to output parameters
			layer_cum_lai_ops(get_multilayer_op(output_parameters, nlayers, "layer_cum_lai_")),
			layer_LeafN_ops(get_multilayer_op(output_parameters, nlayers, "layer_LeafN_")),
			layer_windspeed_ops(get_multilayer_op(output_parameters, nlayers, "layer_windspeed_")),
			layer_Iscat_ops(get_multilayer_op(output_parameters, nlayers, "layer_Iscat_")),
			layer_Idiff_ops(get_multilayer_op(output_parameters, nlayers, "layer_Idiff_")),
			layer_Fsun_ops(get_multilayer_op(output_parameters, nlayers, "layer_Fsun_")),
			layer_Fshade_ops(get_multilayer_op(output_parameters, nlayers, "layer_Fshade_")),
			layer_Itot_ops(get_multilayer_op(output_parameters, nlayers, "layer_Itot_")),
			layer_rh_ops(get_multilayer_op(output_parameters, nlayers, "layer_rh_")),
			layer_height_ops(get_multilayer_op(output_parameters, nlayers, "layer_height_")),
			Isun_op(get_op(output_parameters, "Isun")),
			k_op(get_op(output_parameters, "k"))
		{}
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
		const double* alpha_scatter_ip;
		// Pointers to output parameters
		const std::vector<double*> layer_cum_lai_ops;
		const std::vector<double*> layer_LeafN_ops;
		const std::vector<double*> layer_windspeed_ops;
		const std::vector<double*> layer_Iscat_ops;
		const std::vector<double*> layer_Idiff_ops;
		const std::vector<double*> layer_Fsun_ops;
		const std::vector<double*> layer_Fshade_ops;
		const std::vector<double*> layer_Itot_ops;
		const std::vector<double*> layer_rh_ops;
		const std::vector<double*> layer_height_ops;
		double* Isun_op;
		double* k_op;
	protected:
		static std::vector<std::string> generate_inputs(int nlayers);
		static std::vector<std::string> generate_outputs(int nlayers);
		static std::string generate_description(int nlayers);
		void run() const;
};

std::vector<std::string> multilayer_canopy_properties::generate_inputs(int /*nlayers*/) {
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
		"heightf",
		"alpha_scatter"
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
		"layer_Itot_",
		"layer_rh_",
		"layer_height_"
	};
	// Get a full set of multilayer parameter names, based on the number of layers
	std::vector<std::string> outputs = MultilayerModule::get_multilayer_param_names(nlayers, multilayer_params);
	// Add any other outputs
	outputs.push_back("Isun");
	outputs.push_back("k");
	return outputs;
}

std::string multilayer_canopy_properties::generate_description(int nlayers) {
	std::string description = std::string("  Calculates the properties of each layer in a ") + std::to_string(nlayers) + std::string(" layer canopy.\n");
	description += std::string("  Intended to be used along with the light_macro_environment and solar_zenith_angle modules,\n");
	description += std::string("  which should be run before this module.");
	return description;
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
	const double alpha_scatter = *alpha_scatter_ip;
	
	// Define variables
	double layer_Iscat;
	double layer_Idiff;
	double layer_Fsun;
	double layer_Fshade;
	double layer_Itot;
	
	// Determine the direct and diffuse photon flux density
	//  Idir represents the beam flux through a plane perpendicular to the rays of the sun
	//  Diffuse light is travelling in all directions, so Idiff is the flux through
	//   any plane (is this right??)
	const double Idir = direct_irradiance_fraction * solar;		// micromole / m^2 / s.
	const double Idiff = diffuse_irradiance_fraction * solar;
	
	// Determine the beam flux through a horizontal plane using Lambert's law.
	// See e.g. Eq. 10.3 in Campbell & Norman (1998)
	const double Ibeam = Idir * cosine_zenith_angle;
	
	// Determine the shape factor k from the solar zenith angle and the leaf shape
	//  chi (expressed as the ratio between horizontal and vertical leaf elements,
	//  where chi = 0, 1, and infinity for horizontal, spherical, and vertical
	//  leaf distributions, respectively).
	// The shape factor is the ratio of the leaf shadow area projected on a horizontal
	//  plane (A_h) and the leaf surface area (A), i.e., k = A_h / A.
	// See sections 7.1.1 and 7.1.1.1 in Monteith & Unsworth (2013) for a discussion
	//  of its meaning and section 8.1.1.1 for a simple application to plant canopies
	//  where its use as an extinction coefficient is explained.
	// Here we calculate k using Eq. 15.4 in Campbell & Norman (1998), where we make
	//  use of the fact that if x = cos(theta), then tan^2(theta) = (1 - x^2) / x^2
	// Note that when the sun is below the horizon, we don't need to use k. Just set
	//  it to -1 to indicate this.
	double k;
	if(cosine_zenith_angle > 0) {
		k = sqrt(chil * chil + (1.0 - cosine_zenith_angle * cosine_zenith_angle) / (cosine_zenith_angle * cosine_zenith_angle));	// Calculate numerator
		k /= chil + 1.744 * pow((chil + 1.183), -0.733);																			// Divide by denominator
	}
	else k = -1.0;
	
	// Determine the flux density of beam light directly falling on the sunlit leaves
	// Here we use the shape factor k to convert from the flux density in a horizontal
	//  plane just above the canopy (Ibeam) to the number of photons striking each 
	//  unit of leaf area per second (Isun)
	// I.e., Isun = (beam flux in horizontal plane) * (leaf shadow area) / (leaf surface area)
	//  = Ibeam * k
	double Isun = Ibeam * k;
	
	// Calculate canopy dynamics at each layer and update the output parameters accordingly
	for (int i = 0; i < _nlayers; ++i) {
		// Find the cumulative LAI at the layer midpoint
		//  Cumulative LAI is zero at the canopy bottom and linearly increases to LAI at the top
		//  i = 0 represents the lowest layer and i = nlayers - 1 is the highest
		//  If we imagine a height h along the canopy running from 0 to 1, the ith layer stretches
		//   from h = i / nlayers to (i + 1) / nlayers, so its midpoint is at h = (i + 0.5) / nlayers
		double layer_cum_lai = lai * (i + 0.5) / _nlayers;
		
		// Find the leaf nitrogen, windspeed, and height values for the current layer,
		//  based on its cumulative LAI value
		// Note: this is a little bit different than the previous BioCro version,
		//  where LeafN and windspeed were calculated using the cumulative LAI at
		//  the layer's top rather than its midpoint
		double layer_LeafN = LeafN * exp(-kpLN * (lai - layer_cum_lai));			// Nitrogen decreases exponentially away from the canopy top with decay coefficient kpLN
		double layer_windspeed = windspeed * exp(-0.7 * (lai - layer_cum_lai));		// Windspeed decreases exponentially away from the canopy top with a hardcoded decay coefficient of 0.7
		double layer_height = layer_cum_lai / heightf;
		
		// Find the residual humidity for the current layer, based on ???
		double layer_rh = rh * exp(-(1.0 - rh) * (i + 0.5) / _nlayers);				// Residual humidity decreases exponentially away from the canopy top with a decay coefficient of 1 - rh (why??)
		
		// Calculate light properties
		if(cosine_zenith_angle > 0) {
			// Calculate the scattered light flux through a horizontal plane in this canopy layer
			//  The first term represents the unintercepted beam & down-scattered light incident on this layer,
			//   calculated by using an empirical scattering factor to modify the extinction coefficient k
			//  To find just the scattered light, we need to subtract another term representing the unintercepted beam flux
			//  See Eq. 15.20 in Campbell & Norman (1998) and associated discussion
			// Note that (lai - layer_cum_lai) represents the leaf area above this layer, so both terms decrease
			//  throughout the canopy. They are equal at the canopy top, where there is no scattered light.
			layer_Iscat = Ibeam * (exp(-k * sqrt(alpha_scatter) * (lai - layer_cum_lai)) - exp(-k * (lai - layer_cum_lai)));
			
			// Calculate the total flux of diffuse light through a horizontal in this canopy layer
			// There are two contributions:
			//  (1) Scattered light, as calculated above
			//  (2) Diffuse light from the atmosphere
			// The second contribution is calculated using Eq. 15.6 from Campbell & Norman (1998)
			//  with alpha = 1 and Kbe(phi) = kd.
			// In other words, we have assumed no additional scattering of the diffuse light (alpha = 1)
			//  and used an empirical extinction coefficient (kd rather than the shape factor k as
			//  discussed and calculated above). See section 15.3 in Campbell & Norman (1998).
			// TODO: kd is known to depend on both LAI and chi (see graph 15.4 in Campbell & Norman),
			//  so it might be better to calculate kd for each layer rather than treating it as a constant.
			layer_Idiff = layer_Iscat + Idiff * exp(-kd * (lai - layer_cum_lai));
			
			// Calculate the amounts of sunlit and shaded LAI in this canopy layer using Eq. 15.21
			//  in Campbell & Norman (1998), where lai / nlayers represents the total LAI in this layer.
			// Note that Eq. 15.22 is not appropriate since the layer thickness is not
			//  infinitesimally small in BioCro.
			// To understand this equation (using the same notation as Campbell & Norman, where
			//  L represents the cumulative LAI from the canopy's top), note that we can calculate
			//  the flux density in a horizontal layer at depth L and width dL in two different ways:
			//   (1) Ibeam * e^(-k*L) * (1 - e^(-k*dL))
			//   (2) sunlit_lai * Ibeam * k
			//  The first is the beam flux through a horizontal plane at depth L, multiplied by the fraction
			//   absorbed by a layer with thickness dL
			//  The second is simply the beam flux incident on sunlit leaves (Ibeam * k) multiplied by the amount
			//   of sunlit LAI
			//  Setting them equal, we can solve for the amount of sunlit LAI in the layer, and then the
			//   relative amounts of sunlit and shaded LAI
			double layer_sunlit_lai = exp(-k * (lai - layer_cum_lai)) * (1 - exp(-k * lai / _nlayers)) / k;
			layer_Fsun = layer_sunlit_lai / (lai / _nlayers);
			layer_Fshade = 1 - layer_Fsun;
			
			// Calculate an effective total light flux incident on this canopy layer
			// To do this, we can first calculate the amount of flux absorbed by the sunlit and shaded leaves:
			//  total_absorbed = (sunlit fraction) * (flux on sunlit leaves) * (flux absorbed by sunlit leaves)
			//                 + (shaded fraction) * (flux on shaded leaves) * (flux absorbed by shaded leaves)
			// Then we can rewrite this quantity as Itot * k, where Itot is an effective flux through a horizontal plane
			//  (rather than an extended layer)
			// Setting these quantities equal to each other, we can solve for Itot
			// Note: earlier versions of BioCro calculate this incorrectly because they do not use kd for the shaded leaves
			layer_Itot = (layer_Fsun * (Isun + layer_Idiff) * (1 - exp(-k * lai / _nlayers)) + layer_Fshade * layer_Idiff * (1 - exp(-kd * lai / _nlayers))) / k;
		}
		else {
			// Here the sun is below the horizon, so in place of the calculations above, we want to use the limits as
			//  cosine_zenith_angle approaches zero from the right.
			// In this case, k goes to infinity regardless of chi, simplifying many calculations
			layer_Iscat = 0;											// The exponents in the equation for Iscat become large and negative as k becomes large, so Iscat goes to zero
			layer_Idiff = Idiff * exp(-kd * (lai - layer_cum_lai));		// The Iscat term is now zero, but kd does not change so the other term remains
			layer_Fsun = 0.0;											// The exponents in the equation for sunlit_lai become large and negative as k becomes large, so Fsun goes to zero
			layer_Fshade = 1.0;											// Fsun = 0 means Fshade = 1
			layer_Itot = 0.0;											// The denominator in the equation for Itot becomes large but the numerator doesn't, so Itot goes to zero
		}
		
		// Update the output parameters that change for each layer
		update(layer_cum_lai_ops[i], layer_cum_lai);
		update(layer_LeafN_ops[i], layer_LeafN);
		update(layer_windspeed_ops[i], layer_windspeed);
		update(layer_Iscat_ops[i], layer_Iscat);
		update(layer_Idiff_ops[i], layer_Idiff);
		update(layer_Fsun_ops[i], layer_Fsun);
		update(layer_Fshade_ops[i], layer_Fshade);
		update(layer_Itot_ops[i], layer_Itot);
		update(layer_rh_ops[i], layer_rh);
		update(layer_height_ops[i], layer_height);
	}
	
	// Update output parameters that don't change for each layer
	update(Isun_op, Isun);
	update(k_op, k);
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
		static std::string get_description();
	private:
		// Number of layers
		static const int nlayers;
		// Main operation
		void do_operation() const;
};

const int ten_layer_canopy_properties::nlayers = 10;		// Set the number of layers

std::vector<std::string> ten_layer_canopy_properties::get_inputs() {
	// Just call the parent class's input function with the appropriate number of layers
	return multilayer_canopy_properties::generate_inputs(ten_layer_canopy_properties::nlayers);
}

std::vector<std::string> ten_layer_canopy_properties::get_outputs() {
	// Just call the parent class's output function with the appropriate number of layers
	return multilayer_canopy_properties::generate_outputs(ten_layer_canopy_properties::nlayers);
}

void ten_layer_canopy_properties::do_operation() const {
	// Just call the parent class's run operation
	multilayer_canopy_properties::run();
}

std::string ten_layer_canopy_properties::get_description() {
	// Just call the parent class's description function with the appropriate number of layers
	return multilayer_canopy_properties::generate_description(ten_layer_canopy_properties::nlayers);
}

#endif