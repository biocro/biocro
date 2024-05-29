#include <cmath>      // For floor
#include "c3CanAC.h"  // For c3CanAC
#include "c3_canopy.h"

using standardBML::c3_canopy;

string_vector c3_canopy::get_inputs()
{
    return {
        "absorbed_longwave",            // J / m^2 / s
        "atmospheric_pressure",         // Pa
        "atmospheric_scattering",       // dimensionless
        "atmospheric_transmittance",    // dimensionless
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "beta_PSII",                    // dimensionless (fraction of absorbed light that reaches photosystem II)
        "Catm",                         // ppm
        "chil",                         // dimensionless
        "cosine_zenith_angle",          // dimensionless
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "gbw_canopy",                   // m / s
        "growth_respiration_fraction",  // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "heightf",                      // m^(-1)
        "jmax",                         // micromol / m^2 / s
        "kpLN",
        "k_diffuse",  // dimensionless
        "lai",        // dimensionless
        "LeafN",
        "leafwidth",               // m
        "leaf_reflectance_nir",    // dimensionless
        "leaf_reflectance_par",    // dimensionless
        "leaf_transmittance_nir",  // dimensionless
        "leaf_transmittance_par",  // dimensionless
        "lnb0",
        "lnb1",
        "lnfun",
        "nlayers",              // dimensionless
        "O2",                   // mmol / mol
        "par_energy_content",   // J / micromol
        "par_energy_fraction",  // dimensionless
        "Rd",                   // micromol / m^2 / s
        "rh",                   // dimensionless
        "solar",                // micromol / m^2 / s
        "StomataWS",            // dimensionless
        "temp",                 // degrees C
        "theta",                // dimensionless
        "tpu_rate_max",         // micromol / m^2 / s
        "vmax",                 // micromol / m^2 / s
        "windspeed",            // m / s
        "windspeed_height",     // m
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
    const canopy_photosynthesis_outputs can_result = c3CanAC(
        absorbed_longwave,
        temp,
        atmospheric_pressure,
        atmospheric_scattering,
        atmospheric_transmittance,
        b0,
        b1,
        beta_PSII,
        Catm,
        chil,
        cosine_zenith_angle,
        electrons_per_carboxylation,
        electrons_per_oxygenation,
        gbw_canopy,
        growth_respiration_fraction,
        Gs_min,
        heightf,
        jmax,
        kpLN,
        k_diffuse,
        lai,
        LeafN,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        leafwidth,
        lnb0,
        lnb1,
        O2,
        par_energy_content,
        par_energy_fraction,
        Rd,
        rh,
        solar,
        StomataWS,
        theta,
        tpu_rate_max,
        vmax,
        windspeed,
        windspeed_height,
        lnfun,
        nlayers);

    // Update the output quantity list
    update(canopy_assimilation_rate_op, can_result.Assim);         // Mg / ha / hr
    update(canopy_transpiration_rate_op, can_result.Trans);        // Mg / ha / hr
    update(canopy_conductance_op, can_result.canopy_conductance);  // mmol / m^2 / s
    update(GrossAssim_op, can_result.GrossAssim);                  // Mg / ha / hr
    update(canopy_photorespiration_rate_op, can_result.Rp);        // Mg / ha / hr
}
