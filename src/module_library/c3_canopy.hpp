#ifndef C3_CANOPY_H
#define C3_CANOPY_H

#include "../modules.h"

class c3_canopy : public SteadyModule {
	public:
		c3_canopy(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("c3_canopy"),
			// Get pointers to input parameters
			lai_ip(get_ip(input_parameters, "lai")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			solar_ip(get_ip(input_parameters, "solar")),
			temp_ip(get_ip(input_parameters, "temp")),
			rh_ip(get_ip(input_parameters, "rh")),
			windspeed_ip(get_ip(input_parameters, "windspeed")),
			lat_ip(get_ip(input_parameters, "lat")),
			nlayers_ip(get_ip(input_parameters, "nlayers")),
			vmax_ip(get_ip(input_parameters, "vmax")),
			jmax_ip(get_ip(input_parameters, "jmax")),
			Rd_ip(get_ip(input_parameters, "Rd")),
			Catm_ip(get_ip(input_parameters, "Catm")),
			O2_ip(get_ip(input_parameters, "O2")),
			b0_ip(get_ip(input_parameters, "b0")),
			b1_ip(get_ip(input_parameters, "b1")),
			theta_ip(get_ip(input_parameters, "theta")),
			kd_ip(get_ip(input_parameters, "kd")),
			heightf_ip(get_ip(input_parameters, "heightf")),
			LeafN_ip(get_ip(input_parameters, "LeafN")),
			kpLN_ip(get_ip(input_parameters, "kpLN")),
			lnb0_ip(get_ip(input_parameters, "lnb0")),
			lnb1_ip(get_ip(input_parameters, "lnb1")),
			lnfun_ip(get_ip(input_parameters, "lnfun")),
			chil_ip(get_ip(input_parameters, "chil")),
			StomataWS_ip(get_ip(input_parameters, "StomataWS")),
			growth_respiration_fraction_ip(get_ip(input_parameters, "growth_respiration_fraction")),
			water_stress_approach_ip(get_ip(input_parameters, "water_stress_approach")),
			electrons_per_carboxylation_ip(get_ip(input_parameters, "electrons_per_carboxylation")),
			electrons_per_oxygenation_ip(get_ip(input_parameters, "electrons_per_oxygenation")),
			// Get pointers to output parameters
	        canopy_assimilation_rate_op(get_op(output_parameters, "canopy_assimilation_rate")),
	        canopy_transpiration_rate_op(get_op(output_parameters, "canopy_transpiration_rate")),
	        GrossAssim_op(get_op(output_parameters, "GrossAssim"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* lai_ip;
		const double* doy_dbl_ip;
		const double* solar_ip;
		const double* temp_ip;
		const double* rh_ip;
		const double* windspeed_ip;
		const double* lat_ip;
		const double* nlayers_ip;
		const double* vmax_ip;
		const double* jmax_ip;
		const double* Rd_ip;
		const double* Catm_ip;
		const double* O2_ip;
		const double* b0_ip;
		const double* b1_ip;
		const double* theta_ip;
		const double* kd_ip;
		const double* heightf_ip;
		const double* LeafN_ip;
		const double* kpLN_ip;
		const double* lnb0_ip;
		const double* lnb1_ip;
		const double* lnfun_ip;
		const double* chil_ip;
		const double* StomataWS_ip;
		const double* growth_respiration_fraction_ip;
		const double* water_stress_approach_ip;
		const double* electrons_per_carboxylation_ip;
		const double* electrons_per_oxygenation_ip;
		// Pointers to output parameters
        double* canopy_assimilation_rate_op;
        double* canopy_transpiration_rate_op;
        double* GrossAssim_op;
		// Main operation
		void do_operation() const;
};

#endif


