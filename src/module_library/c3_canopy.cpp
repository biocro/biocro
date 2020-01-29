#include "c3_canopy.hpp"

std::vector<std::string> c3_canopy::get_inputs() {
	return {
		"lai",
		"doy_dbl",
		"solar",
		"temp",
		"rh",
		"windspeed",
		"lat",
		"nlayers",
		"vmax",
		"jmax",
		"Rd",
		"Catm",
		"O2",
		"b0",
		"b1",
		"theta",
		"kd",
		"heightf",
		"LeafN",
		"kpLN",
		"lnb0",
		"lnb1",
		"lnfun",
		"chil",
		"StomataWS",
		"growth_respiration_fraction",
		"water_stress_approach",
		"electrons_per_carboxylation",
		"electrons_per_oxygenation"
	};
}

std::vector<std::string> c3_canopy::get_outputs() {
	return {
        "canopy_assimilation_rate",
        "canopy_transpiration_rate",
        "GrossAssim"
	};
}

void c3_canopy::do_operation() const {
	// Collect inputs and make calculations
	int doy = int(*doy_dbl_ip);					// Round doy_dbl down to get the day of year
	double hour = 24.0 * ((*doy_dbl_ip) - doy);	// Get the fractional part as the hour
	
	// c3CanAC is located in c3CanAc.cpp
	struct Can_Str can_result = c3CanAC(*lai_ip, doy, hour, *solar_ip, *temp_ip,
            *rh_ip, *windspeed_ip, *lat_ip, (int)(*nlayers_ip), *vmax_ip,
            *jmax_ip, *Rd_ip, *Catm_ip, *O2_ip, *b0_ip,
            *b1_ip, *theta_ip, *kd_ip, *heightf_ip, *LeafN_ip,
            *kpLN_ip, *lnb0_ip, *lnb1_ip, (int)(*lnfun_ip), *chil_ip,
            *StomataWS_ip, *growth_respiration_fraction_ip, (int)(*water_stress_approach_ip), *electrons_per_carboxylation_ip, *electrons_per_oxygenation_ip);
	
	// Update the output parameter list
	update(canopy_assimilation_rate_op, can_result.Assim);	// Mg / ha / hr.
	update(canopy_transpiration_rate_op, can_result.Trans);	// Mg / ha / hr.
	update(GrossAssim_op, can_result.GrossAssim);
}
