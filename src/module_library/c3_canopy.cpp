#include <cmath>                      // For floor
#include "c3CanAC.h"                  // For c3CanAC
#include "c3_temperature_response.h"  // for c3_temperature_response_parameters
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
        "Gstar_c",                      // dimensionless
        "Gstar_Ea",                     // J / mol
        "heightf",                      // m^(-1)
        "Jmax_at_25",                   // micromol / m^2 / s
        "Jmax_c",                       // dimensionless
        "Jmax_Ea",                      // J / mol
        "k_diffuse",                    // dimensionless
        "Kc_c",                         // dimensionless
        "Kc_Ea",                        // J / mol
        "Ko_c",                         // dimensionless
        "Ko_Ea",                        // J / mol
        "kpLN",
        "lai",                     // dimensionless
        "leaf_reflectance_nir",    // dimensionless
        "leaf_reflectance_par",    // dimensionless
        "leaf_transmittance_nir",  // dimensionless
        "leaf_transmittance_par",  // dimensionless
        "LeafN",
        "leafwidth",  // m
        "lnb0",
        "lnb1",
        "lnfun",
        "nlayers",              // dimensionless
        "O2",                   // mmol / mol
        "par_energy_content",   // J / micromol
        "par_energy_fraction",  // dimensionless
        "phi_PSII_0",           // dimensionless
        "phi_PSII_1",           // (degrees C)^(-1)
        "phi_PSII_2",           // (degrees C)^(-2)
        "rh",                   // dimensionless
        "RL_at_25",             // micromol / m^2 / s
        "RL_c",                 // dimensionless
        "RL_Ea",                // J / mol
        "solar",                // micromol / m^2 / s
        "StomataWS",            // dimensionless
        "temp",                 // degrees C
        "theta_0",              // dimensionless
        "theta_1",              // (degrees C)^(-1)
        "theta_2",              // (degrees C)^(-2)
        "Tp_at_25",             // micromol / m^2 / s
        "Tp_c",                 // dimensionless
        "Tp_Ha",                // J / mol
        "Tp_Hd",                // J / mol
        "Tp_S",                 // J / K / mol
        "Vcmax_at_25",          // micromol / m^2 / s
        "Vcmax_c",              // dimensionless
        "Vcmax_Ea",             // J / mol
        "windspeed",            // m / s
        "windspeed_height"      // m
    };
}

string_vector c3_canopy::get_outputs()
{
    return {
        "canopy_assimilation_molar_flux",                      // micromol / m^2 / s
        "canopy_conductance",                                  // mol / m^2 / s
        "canopy_gross_assimilation_molar_flux",                // micromol / m^2 / s
        "canopy_non_photorespiratory_CO2_release_molar_flux",  // micromol / m^2 / s
        "canopy_photorespiration_molar_flux",                  // micromol / m^2 / s
        "canopy_transpiration_rate",                           // Mg / ha / hr
        "whole_plant_growth_respiration_molar_flux"            // micromol / m^2 / s
    };
}

void c3_canopy::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        Gstar_c,
        Gstar_Ea,
        Jmax_c,
        Jmax_Ea,
        Kc_c,
        Kc_Ea,
        Ko_c,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        RL_c,
        RL_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_c,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_c,
        Vcmax_Ea};

    const canopy_photosynthesis_outputs can_result = c3CanAC(
        tr_param,
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
        Jmax_at_25,
        k_diffuse,
        kpLN,
        lai,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        leafwidth,
        LeafN,
        lnb0,
        lnb1,
        O2,
        par_energy_content,
        par_energy_fraction,
        rh,
        RL_at_25,
        solar,
        StomataWS,
        Tp_at_25,
        Vcmax_at_25,
        windspeed,
        windspeed_height,
        lnfun,
        nlayers);

    // Update the output quantity list
    update(canopy_assimilation_molar_flux_op, can_result.Assim);                      // micromol / m^2 / s
    update(canopy_conductance_op, can_result.canopy_conductance);                     // mol / m^2 / s
    update(canopy_gross_assimilation_molar_flux_op, can_result.GrossAssim);           // micromol / m^2 / s
    update(canopy_non_photorespiratory_CO2_release_rate_op, can_result.RL);           // micromol / m^2 / s
    update(canopy_photorespiration_molar_flux_op, can_result.Rp);                     // micromol / m^2 / s
    update(canopy_transpiration_rate_op, can_result.Trans);                           // Mg / ha / hr
    update(whole_plant_growth_respiration_molar_flux_op, can_result.whole_plant_gr);  // micromol / m^2 / s
}
