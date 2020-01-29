#ifndef BIG_LEAF_MULTILAYER_CANOPY_H
#define BIG_LEAF_MULTILAYER_CANOPY_H

#include "../modules.h"
#include "../standalone_ss.h"

class big_leaf_multilayer_canopy : public SteadyModule {
	public:
		big_leaf_multilayer_canopy(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters);
		~big_leaf_multilayer_canopy();	// This class has pointer members, so they must be deleted in its destructor
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// This module uses other modules to make calculations,
		//  so it requres a standalone_ss member
		// Some of the module's inputs and outputs require additional
		//  member variables
		std::shared_ptr<Standalone_SS> canopy_modules;
		double* theta_ptr;
		double* layer_wind_speed_ptr;
		double* layer_relative_humidity_ptr;
		double* Ipar_ptr;
		double* Irad_ptr;
		double* leaf_ass_ptr;
		double* leaf_trans_ptr;
		double* leaf_cond_ptr;
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
		const double* temp_ip;
		const double* kparm_ip;
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
		const double* stefan_boltzman_ip;
		const double* k_Q10_ip;
		// Pointers to output parameters
		double* canopy_assimilation_rate_op;
		double* canopy_transpiration_rate_op;
		double* canopy_conductance_op;
		// Main operation
		void do_operation() const;
};

#endif