#include "c3_canopy.hpp"
#include "AuxBioCro.h"  // For Can_Str
#include "BioCro.h"     // For c3CanAC
#include <cmath>        // For floor

string_vector c3_canopy::get_inputs()
{
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
        "absorptivity_par",     // dimensionless
        "par_energy_content",   // J / micromol
        "par_energy_fraction",  // dimensionless
        "leaf_transmittance",   // dimensionless
        "leaf_reflectance"      // dimensionless
    };
}

string_vector c3_canopy::get_outputs()
{
    return {
        "canopy_assimilation_rate",   // Mg / ha / hr
        "canopy_transpiration_rate",  // Mg / ha / hr
        "GrossAssim"                  // Mg / ha / hr
    };
}

void c3_canopy::do_operation() const
{
    // Collect inputs and make calculations
    double doy = floor(time);           // Round time down to get the day of year
    double hour = 24.0 * (time - doy);  // Get the fractional part as the hour

    // c3CanAC is located in c3CanAc.cpp
    struct Can_Str can_result = c3CanAC(
        lai, doy, hour, solar, temp, rh, windspeed, lat, nlayers, vmax, jmax,
        tpu_rate_max, Rd, Catm, O2, b0, b1, Gs_min, theta, kd, heightf, LeafN,
        kpLN, lnb0, lnb1, lnfun, chil, StomataWS, specific_heat_of_air,
        atmospheric_pressure, growth_respiration_fraction,
        water_stress_approach, electrons_per_carboxylation,
        electrons_per_oxygenation, absorptivity_par, par_energy_content,
        par_energy_fraction, leaf_transmittance, leaf_reflectance);

    // Update the output quantity list
    update(canopy_assimilation_rate_op, can_result.Assim);   // Mg / ha / hr.
    update(canopy_transpiration_rate_op, can_result.Trans);  // Mg / ha / hr.
    update(GrossAssim_op, can_result.GrossAssim);
}
