#include "c3_canopy.hpp"
#include "AuxBioCro.h"  // For Can_Str
#include "BioCro.h"     // For c3CanAC
#include <cmath>        // For floor

std::vector<std::string> c3_canopy::get_inputs() {
    return {
        "lai",
        "time",
        "solar",
        "temp",
        "rh",
        "windspeed",
        "lat",
        "nlayers",
        "vmax",
        "jmax",
        "tpu_rate_max",
        "Rd",
        "Catm",
        "O2",
        "b0",
        "b1",
        "Gs_min",  // mol / m^2 / s
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
        "specific_heat_of_air",  // J / kg / K
        "atmospheric_pressure",  // Pa
        "growth_respiration_fraction",
        "water_stress_approach",
        "electrons_per_carboxylation",
        "electrons_per_oxygenation",
        "absorptivity_par" // dimensionless
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
    double doy = floor(*time_ip);            // Round time down to get the day of year
    double hour = 24.0 * ((*time_ip) - doy); // Get the fractional part as the hour

    // c3CanAC is located in c3CanAc.cpp
    struct Can_Str can_result = c3CanAC(*lai_ip, doy, hour, *solar_ip, *temp_ip,
            *rh_ip, *windspeed_ip, *lat_ip, *nlayers_ip, *vmax_ip,
            *jmax_ip, *tpu_rate_max_ip, *Rd_ip, *Catm_ip, *O2_ip, *b0_ip,
            *b1_ip, *Gs_min_ip, *theta_ip, *kd_ip, *heightf_ip, *LeafN_ip,
            *kpLN_ip, *lnb0_ip, *lnb1_ip, *lnfun_ip, *chil_ip,
            *StomataWS_ip, *specific_heat_of_air_ip, *atmospheric_pressure_ip,
            *growth_respiration_fraction_ip, *water_stress_approach_ip,
            *electrons_per_carboxylation_ip, *electrons_per_oxygenation_ip,
            *absorptivity_par_ip);

    // Update the output quantity list
    update(canopy_assimilation_rate_op, can_result.Assim);  // Mg / ha / hr.
    update(canopy_transpiration_rate_op, can_result.Trans); // Mg / ha / hr.
    update(GrossAssim_op, can_result.GrossAssim);
}
