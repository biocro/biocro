#ifndef CANAC_WITH_COLLATZ_H
#define CANAC_WITH_COLLATZ_H

#include "../modules.h"
#include "../system.h"
#include "../standalone_ss.h"
#include "water_vapor_properties_from_air_temperature.hpp"
#include "penman_monteith_transpiration.hpp"
#include "collatz_leaf.hpp"

struct Can_Str newCanAC(
        double LAI,
        int DOY,
        double hr,
        double solarR,
        double temperature,  // degrees C
        double RH,           // dimensionless from Pa / Pa
        double WindSpeed,    // m / s
        double lat,
        int nlayers,
        double Vmax,
        double Alpha,
        double Kparm,
        double beta,
        double Rd,
        double Catm,
        double b0,
        double b1,
        double theta,
        double kd,
        double chil,
        double heightf,
        double leafN,
        double kpLN,
        double lnb0,
        double lnb1,
        int lnfun,
        double upperT,
        double lowerT,
        const struct nitroParms &nitroP,
        double leafwidth,    // meter
        int eteq,
        double StomataWS,
        int water_stress_approach,
		double leaf_transmittance,
		double leaf_reflectance)
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
	
	// Set up the input parameters
	// Note: the list of required inputs was quickly determined by using the following R command
	//  with the BioCro library loaded:
	//  get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	// Note that some of these parameters have not been defined yet
	double Itot;
	double Ipar;
	double layer_wind_speed;
	double relative_humidity;
	double vmax1;
	double wsa = (double) water_stress_approach;
	double k_Q10 = 2.0;
	double stefan_boltzman = 5.67e-8;
	std::unordered_map<std::string, const double*> input_param_ptrs = {
		{"Catm", 					&Catm},
		{"Rd", 						&Rd},
		{"StomataWS", 				&StomataWS},
		{"alpha", 					&Alpha},
		{"b0", 						&b0},
		{"b1", 						&b1},
		{"beta", 					&beta},
		{"incident_irradiance", 	&Itot},
		{"incident_par", 			&Ipar},
		{"k_Q10", 					&k_Q10},
		{"kparm", 					&Kparm},
		{"layer_wind_speed", 		&layer_wind_speed},
		{"leaf_reflectance", 		&leaf_reflectance},
		{"leaf_transmittance", 		&leaf_transmittance},
		{"leafwidth", 				&leafwidth},
		{"lowerT", 					&lowerT},
		{"rh", 						&relative_humidity},
		{"stefan_boltzman", 		&stefan_boltzman},
		{"temp", 					&temperature},
		{"theta", 					&theta},
		{"upperT", 					&upperT},
		{"vmax", 					&vmax1},
		{"water_stress_approach", 	&wsa}
	};
	
	// Set up the output parameters
	// Note: the list of all possible outputs can be quickly determined by using
	//  the following R command with the BioCro library loaded:
	//  get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	// Here were are only interested in a few of them
	double leaf_ass;
	double leaf_trans;
	double leaf_cond;
	std::unordered_map<std::string, double*> output_param_ptrs = {
		{"leaf_assimilation_rate", 				&leaf_ass},
		{"leaf_transpiration_rate", 			&leaf_trans},
		{"leaf_stomatal_conductance",			&leaf_cond}
	};
	
	// Now that the inputs are defined, make the standalone modules and store a smart pointer to them
	std::shared_ptr<Standalone_SS> canopy_modules = std::shared_ptr<Standalone_SS>(new Standalone_SS(steady_state_modules, input_param_ptrs, output_param_ptrs));
	
	// Calculate the light and humidity properties at each layer of the canopy
	struct Light_model light_model = lightME(lat, DOY, hr);
	
	double Idir = light_model.direct_irradiance_fraction * solarR;  // micromole / m^2 / s. Flux through a plane perpendicular to the rays of the sun.
	double Idiff = light_model.diffuse_irradiance_fraction * solarR;
	double cosTh = light_model.cosine_zenith_angle;
	
	struct Light_profile light_profile = sunML(Idir, Idiff, LAI, nlayers, cosTh, kd, chil, heightf);
	
	double LAIc = LAI / nlayers;
	
	double relative_humidity_profile[nlayers];
	RHprof(RH, nlayers, relative_humidity_profile);  // Modifies relative_humidity_profile.
	
	double wind_speed_profile[nlayers];
	WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);  // Modifies wind_speed_profile.
	
	double leafN_profile[nlayers];
	LNprof(leafN, LAI, nlayers, kpLN, leafN_profile);  // Modifies leafN_profile.
	
	double CanopyA = 0.0,  CanopyT = 0.0;
	double canopy_conductance = 0.0;
	
	// Initialize some rates
	double sunlit_leaf_assimilation_rate;
	double sunlit_leaf_transpiration_rate;
	double sunlit_leaf_conductance;
	double shaded_leaf_assimilation_rate;
	double shaded_leaf_transpiration_rate;
	double shaded_leaf_conductance;
	
	// Go through the layers
	for (int i = 0; i < nlayers; ++i)
	{
		// Get new values for system inputs that vary by layer
		int current_layer = nlayers - 1 - i;
		double leafN_lay = leafN_profile[current_layer];
		
		if (lnfun == 0) {
			vmax1 = Vmax;
		} else {
			vmax1 = nitroP.Vmaxb1 * leafN_lay + nitroP.Vmaxb0;
			if (vmax1 < 0) vmax1 = 0.0;
			if (vmax1 > Vmax) vmax1 = Vmax;
			Alpha = nitroP.alphab1 * leafN_lay + nitroP.alphab0;
			Rd = nitroP.Rdb1 * leafN_lay + nitroP.Rdb0;
		}
		
		relative_humidity = relative_humidity_profile[current_layer];
		layer_wind_speed = wind_speed_profile[current_layer];
		Itot = light_profile.total_irradiance[current_layer];  // micromole / m^2 / s
		
		// Get assimilation, transpiration, and conductance rates for sunlit leaves
		Ipar = light_profile.direct_irradiance[current_layer];  // micromole / m^2 / s
		double pLeafsun = light_profile.sunlit_fraction[current_layer];  // dimensionless. Fraction of LAI that is sunlit.
		double Leafsun = LAIc * pLeafsun;
		canopy_modules->run();
		sunlit_leaf_assimilation_rate = leaf_ass;
		sunlit_leaf_transpiration_rate = leaf_trans;
		sunlit_leaf_conductance = leaf_cond;
		
		// Get assimilation, transpiration, and conductance rates for shaded leaves
		Ipar = light_profile.diffuse_irradiance[current_layer];  // micromole / m^2 / s
		double pLeafshade = light_profile.shaded_fraction[current_layer];  // dimensionless. Fraction of LAI that is shaded.
		double Leafshade = LAIc * pLeafshade;
		canopy_modules->run();
		shaded_leaf_assimilation_rate = leaf_ass;
		shaded_leaf_transpiration_rate = leaf_trans;
		shaded_leaf_conductance = leaf_cond;
		
		// Add rates to the total canopy assimilation, transpiration, and conductance
		CanopyA += Leafsun * sunlit_leaf_assimilation_rate + Leafshade * shaded_leaf_assimilation_rate;
		CanopyT += Leafsun * sunlit_leaf_transpiration_rate + Leafshade * shaded_leaf_transpiration_rate;
		canopy_conductance += Leafsun * sunlit_leaf_conductance + Leafshade * shaded_leaf_conductance;
	}
	
	struct Can_Str ans;
	/* For Assimilation */
	/* 3600 - seconds per hour */
	/* 1e-6 - moles per micromole */
	/* 30 - grams per mole for CO2 */
	/* 1e-6 - megagrams per gram */
	/* 10000 - meters squared per hectare*/
	ans.Assim = CanopyA * 3600 * 1e-6 * 30 * 1e-6 * 10000;  // Mg / ha / hr.
	
	/* For Transpiration */
	/* 3600 - seconds per hour */
	/* 1e-3 - millimoles per mole */
	/* 18 - grams per mole for H2O */
	/* 1e-6 - megagrams per  gram */
	/* 10000 - meters squared per hectare */
	ans.Trans = CanopyT * 3600 * 1e-3 * 18 * 1e-6 * 10000;  // Mg / ha / hr.
	ans.canopy_conductance = canopy_conductance;
	
	return ans;
}

class canac_with_collatz : public SteadyModule {
	public:
		canac_with_collatz(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("canac_with_collatz"),
			// Get pointers to input parameters
			nileafn_ip(get_ip(input_parameters, "nileafn")),
			nkln_ip(get_ip(input_parameters, "nkln")),
			nvmaxb1_ip(get_ip(input_parameters, "nvmaxb1")),
			nvmaxb0_ip(get_ip(input_parameters, "nvmaxb0")),
			nalphab1_ip(get_ip(input_parameters, "nalphab1")),
			nalphab0_ip(get_ip(input_parameters, "nalphab0")),
			nRdb1_ip(get_ip(input_parameters, "nRdb1")),
			nRdb0_ip(get_ip(input_parameters, "nRdb0")),
			nkpLN_ip(get_ip(input_parameters, "nkpLN")),
			nlnb0_ip(get_ip(input_parameters, "nlnb0")),
			nlnb1_ip(get_ip(input_parameters, "nlnb1")),
			lai_ip(get_ip(input_parameters, "lai")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			solar_ip(get_ip(input_parameters, "solar")),
			temp_ip(get_ip(input_parameters, "temp")),
			rh_ip(get_ip(input_parameters, "rh")),
			windspeed_ip(get_ip(input_parameters, "windspeed")),
			lat_ip(get_ip(input_parameters, "lat")),
			nlayers_ip(get_ip(input_parameters, "nlayers")),
			vmax1_ip(get_ip(input_parameters, "vmax1")),
			alpha1_ip(get_ip(input_parameters, "alpha1")),
			kparm_ip(get_ip(input_parameters, "kparm")),
			beta_ip(get_ip(input_parameters, "beta")),
			Rd_ip(get_ip(input_parameters, "Rd")),
			Catm_ip(get_ip(input_parameters, "Catm")),
			b0_ip(get_ip(input_parameters, "b0")),
			b1_ip(get_ip(input_parameters, "b1")),
			theta_ip(get_ip(input_parameters, "theta")),
			kd_ip(get_ip(input_parameters, "kd")),
			chil_ip(get_ip(input_parameters, "chil")),
			heightf_ip(get_ip(input_parameters, "heightf")),
			LeafN_ip(get_ip(input_parameters, "LeafN")),
			kpLN_ip(get_ip(input_parameters, "kpLN")),
			lnb0_ip(get_ip(input_parameters, "lnb0")),
			lnb1_ip(get_ip(input_parameters, "lnb1")),
			lnfun_ip(get_ip(input_parameters, "lnfun")),
			upperT_ip(get_ip(input_parameters, "upperT")),
			lowerT_ip(get_ip(input_parameters, "lowerT")),
			leafwidth_ip(get_ip(input_parameters, "leafwidth")),
			et_equation_ip(get_ip(input_parameters, "et_equation")),
			StomataWS_ip(get_ip(input_parameters, "StomataWS")),
			water_stress_approach_ip(get_ip(input_parameters, "water_stress_approach")),
			leaf_transmittance_ip(get_ip(input_parameters, "leaf_transmittance")),
			leaf_reflectance_ip(get_ip(input_parameters, "leaf_reflectance")),
			// Get pointers to output parameters
			canopy_assimilation_rate_op(get_op(output_parameters, "canopy_assimilation_rate")),
			canopy_transpiration_rate_op(get_op(output_parameters, "canopy_transpiration_rate")),
			canopy_conductance_op(get_op(output_parameters, "canopy_conductance"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* nileafn_ip;
		const double* nkln_ip;
		const double* nvmaxb1_ip;
		const double* nvmaxb0_ip;
		const double* nalphab1_ip;
		const double* nalphab0_ip;
		const double* nRdb1_ip;
		const double* nRdb0_ip;
		const double* nkpLN_ip;
		const double* nlnb0_ip;
		const double* nlnb1_ip;
		const double* lai_ip;
		const double* doy_dbl_ip;
		const double* solar_ip;
		const double* temp_ip;
		const double* rh_ip;
		const double* windspeed_ip;
		const double* lat_ip;
		const double* nlayers_ip;
		const double* vmax1_ip;
		const double* alpha1_ip;
		const double* kparm_ip;
		const double* beta_ip;
		const double* Rd_ip;
		const double* Catm_ip;
		const double* b0_ip;
		const double* b1_ip;
		const double* theta_ip;
		const double* kd_ip;
		const double* chil_ip;
		const double* heightf_ip;
		const double* LeafN_ip;
		const double* kpLN_ip;
		const double* lnb0_ip;
		const double* lnb1_ip;
		const double* lnfun_ip;
		const double* upperT_ip;
		const double* lowerT_ip;
		const double* leafwidth_ip;
		const double* et_equation_ip;
		const double* StomataWS_ip;
		const double* water_stress_approach_ip;
		const double* leaf_transmittance_ip;
		const double* leaf_reflectance_ip;
		// Pointers to output parameters
		double* canopy_assimilation_rate_op;
		double* canopy_transpiration_rate_op;
		double* canopy_conductance_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> canac_with_collatz::get_inputs() {
	return {
		"nileafn",
		"nkln",
		"nvmaxb1",
		"nvmaxb0",
		"nalphab1",
		"nalphab0",
		"nRdb1",
		"nRdb0",
		"nkpLN",
		"nlnb0",
		"nlnb1",
		"lai",
		"doy_dbl",
		"solar",
		"temp",
		"rh",
		"windspeed",
		"lat",
		"nlayers",
		"vmax1",
		"alpha1",
		"kparm",
		"beta",
		"Rd",
		"Catm",
		"b0",
		"b1",
		"theta",
		"kd",
		"chil",
		"heightf",
		"LeafN",
		"kpLN",
		"lnb0",
		"lnb1",
		"lnfun",
		"upperT",
		"lowerT",
		"leafwidth",
		"et_equation",
		"StomataWS",
		"water_stress_approach",
		"leaf_transmittance",
		"leaf_reflectance"
	};
}

std::vector<std::string> canac_with_collatz::get_outputs() {
	return {
		"canopy_assimilation_rate",
		"canopy_transpiration_rate",
		"canopy_conductance"
	};
}

void canac_with_collatz::do_operation() const {	
	// Collect inputs and make calculations
	double nileafn = *nileafn_ip;
	double nkln = *nkln_ip;
	double nvmaxb1 = *nvmaxb1_ip;
	double nvmaxb0 = *nvmaxb0_ip;
	double nalphab1 = *nalphab1_ip;
	double nalphab0 = *nalphab0_ip;
	double nRdb1 = *nRdb1_ip;
	double nRdb0 = *nRdb0_ip;
	double nkpLN = *nkpLN_ip;
	double nlnb0 = *nlnb0_ip;
	double nlnb1 = *nlnb1_ip;
	double lai = *lai_ip;
	double doy_dbl = *doy_dbl_ip;
	double solar = *solar_ip;
	double temp = *temp_ip;
	double rh = *rh_ip;
	double windspeed = *windspeed_ip;
	double lat = *lat_ip;
	double nlayers = *nlayers_ip;
	double vmax1 = *vmax1_ip;
	double alpha1 = *alpha1_ip;
	double kparm = *kparm_ip;
	double beta = *beta_ip;
	double Rd = *Rd_ip;
	double Catm = *Catm_ip;
	double b0 = *b0_ip;
	double b1 = *b1_ip;
	double theta = *theta_ip;
	double kd = *kd_ip;
	double chil = *chil_ip;
	double heightf = *heightf_ip;
	double LeafN = *LeafN_ip;
	double kpLN = *kpLN_ip;
	double lnb0 = *lnb0_ip;
	double lnb1 = *lnb1_ip;
	double lnfun = *lnfun_ip;
	double upperT = *upperT_ip;
	double lowerT = *lowerT_ip;
	double leafwidth = *leafwidth_ip;
	double et_equation = *et_equation_ip;
	double StomataWS = *StomataWS_ip;
	double water_stress_approach = *water_stress_approach_ip;
	double leaf_transmittance = *leaf_transmittance_ip;
	double leaf_reflectance = *leaf_reflectance_ip;
	
	// Convert doy_dbl into doy and hour
	int doy = floor(doy_dbl);
	double hour = 24.0 * (doy_dbl - doy);
	
	// Define the nitrogen parameters
	struct nitroParms nitroP;
	nitroP.ileafN = nileafn;
	nitroP.kln = nkln;
	nitroP.Vmaxb1 = nvmaxb1;
	nitroP.Vmaxb0 = nvmaxb0;
	nitroP.alphab1 = nalphab1;
	nitroP.alphab0 = nalphab0;
	nitroP.Rdb1 = nRdb1;
	nitroP.Rdb0 = nRdb0;
	nitroP.kpLN = nkpLN;
	nitroP.lnb0 = nlnb0;
	nitroP.lnb1 = nlnb1;
	
	// Make the CanAC calculation
	struct Can_Str can_result = newCanAC(lai, doy, hour, solar, temp,
			rh, windspeed, lat, (int)nlayers, vmax1,
			alpha1, kparm, beta, Rd, Catm,
			b0, b1, theta, kd, chil,
			heightf, LeafN, kpLN, lnb0, lnb1,
			(int)lnfun, upperT, lowerT, nitroP, leafwidth,
			(int)et_equation, StomataWS, (int)water_stress_approach,
			leaf_transmittance, leaf_reflectance);
	
	// Update the output parameter list
	update(canopy_assimilation_rate_op, can_result.Assim);		// Mg / ha / hr.
	update(canopy_transpiration_rate_op, can_result.Trans);		// Mg / ha / hr.
	update(canopy_conductance_op, can_result.canopy_conductance);
}

#endif