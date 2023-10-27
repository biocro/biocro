#include "c3_canopy.h"
#include "c3CanAC.h"  // For c3CanAC
#include <cmath>      // For floor

using standardBML::c3_canopy;

string_vector c3_canopy::get_inputs()
{
    return {
        "lai",
        "cosine_zenith_angle",
        "solar",
        "temp",
        "rh",
        "windspeed",
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
        "specific_heat_of_air",       // J / kg / K
        "atmospheric_pressure",       // Pa
        "atmospheric_transmittance",  // dimensionless
        "atmospheric_scattering",     // dimensionless
        "growth_respiration_fraction",
        "electrons_per_carboxylation",
        "electrons_per_oxygenation",
        "absorptivity_par",     // dimensionless
        "par_energy_content",   // J / micromol
        "par_energy_fraction",  // dimensionless
        "leaf_transmittance",   // dimensionless
        "leaf_reflectance",     // dimensionless
        "minimum_gbw",          // mol / m^2 / s
        "windspeed_height",     // m
        "beta_PSII"             // dimensionless (fraction of absorbed light that reaches photosystem II)
    };
}

string_vector c3_canopy::get_outputs()
{
    return {
        "canopy_assimilation_rate",     // Mg / ha / hr
        "canopy_transpiration_rate",    // Mg / ha / hr
        "canopy_conductance",           // Mg / ha / hr
        "GrossAssim",                   // Mg / ha / hr
        "canopy_photorespiration_rate"  // Mg / ha / hr
    };
}

void c3_canopy::do_operation() const
{
    canopy_photosynthesis_outputs can_result = c3CanAC(
        lai, cosine_zenith_angle, solar, temp, rh, windspeed, nlayers, vmax,
        jmax, tpu_rate_max, Rd, Catm, O2, b0, b1, Gs_min, theta, kd, heightf,
        LeafN, kpLN, lnb0, lnb1, lnfun, chil, StomataWS, specific_heat_of_air,
        atmospheric_pressure, atmospheric_transmittance, atmospheric_scattering,
        growth_respiration_fraction, electrons_per_carboxylation,
        electrons_per_oxygenation, absorptivity_par, par_energy_content,
        par_energy_fraction, leaf_transmittance, leaf_reflectance, minimum_gbw,
        windspeed_height, beta_PSII);

    // Update the output quantity list
    update(canopy_assimilation_rate_op, can_result.Assim);         // Mg / ha / hr
    update(canopy_transpiration_rate_op, can_result.Trans);        // Mg / ha / hr
    update(canopy_conductance_op, can_result.canopy_conductance);  // mmol / m^2 / s
    update(GrossAssim_op, can_result.GrossAssim);                  // Mg / ha / hr
    update(canopy_photorespiration_rate_op, can_result.Rp);        // Mg / ha / hr
}
