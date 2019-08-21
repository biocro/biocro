#ifndef COLLATZ_LEAF_H
#define COLLATZ_LEAF_H

#include "../modules.h"
#include "../standalone_ss.h"
#include "collatz_photo.hpp"
#include "ball_berry.hpp"

class collatz_leaf : public SteadyModule {
	public:
		collatz_leaf(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters);
		~collatz_leaf();	// This class has pointer members, so they must be deleted in its destructor
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// This module uses other modules to make calculations,
		//  so it requres a standalone_ss member
		// Some of the module's inputs and outputs require additional
		//  member variables
		std::shared_ptr<Standalone_SS> leaf_temperature_module;
		double* boundary_layer_conductance_ptr;
		double* gs_ptr;
		double* leaf_net_irradiance_ptr;
		double* leaf_temperature_ptr;
		// Pointers to input parameters
		const double* incident_irradiance_ip;
		const double* incident_par_ip;
		const double* temp_ip;
		const double* vmax_ip;
		const double* alpha_ip;
		const double* kparm_ip;
		const double* theta_ip;
		const double* beta_ip;
		const double* Rd_ip;
		const double* upperT_ip;
		const double* lowerT_ip;
		const double* k_Q10_ip;
		const double* Catm_ip;
		const double* rh_ip;
		const double* b0_ip;
		const double* b1_ip;
		const double* layer_wind_speed_ip;
		const double* leafwidth_ip;
		const double* water_vapor_pressure_ip;
		const double* stefan_boltzman_ip;
		const double* leaf_reflectance_ip;
		const double* leaf_transmittance_ip;
		const double* water_stress_approach_ip;
		const double* StomataWS_ip;
		const double* slope_water_vapor_ip;
		const double* psychrometric_parameter_ip;
		const double* latent_heat_vaporization_of_water_ip;
		const double* vapor_density_deficit_ip;
		// Pointers to output parameters
		double* leaf_assimilation_rate_op;
		double* leaf_stomatal_conductance_op;
		double* leaf_boundary_layer_conductance_op;
		double* leaf_temperature_op;
		double* Ci_op;
		double* leaf_net_irradiance_op;
		// Main operation
		void do_operation() const;
};

#endif