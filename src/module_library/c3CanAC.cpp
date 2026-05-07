#include "../framework/constants.h"           // for molar_mass_of_water, molar_mass_of_glucose
#include "../math/quadrature/quad.h"          // for quadrature::gauss_legendre
#include "core/leaf_photosynthesis.h"         // for PhotoCore::C3LeafPhoto
#include "core/photosynthesis.h"              // for PhotoCore::LeafAssim, CanopyIntegrand
#include "core/atmosphere_light_scattering.h" // for PhotoCore::AtmosphereLightScattering
#include "respiration.h"                      // for growth_resp
#include "c3CanAC.h"

canopy_photosynthesis_outputs c3CanAC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / m^2 / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const atmospheric_scattering,       // dimensionless
    double const atmospheric_transmittance,    // dimensionless
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const Catm,                         // ppm
    double const chil,                         // dimensionless
    double const cosine_zenith_angle,          // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // m / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const growth_respiration_fraction,  // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const heightf,                      // m^(-1)
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const k_diffuse,                    // dimensionless
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const kpLN,
    double const LAI,                     // dimensionless
    double const leaf_reflectance_nir,    // dimensionless
    double const leaf_reflectance_par,    // dimensionless
    double const leaf_transmittance_nir,  // dimensionless
    double const leaf_transmittance_par,  // dimensionless
    double const leaf_width,              // m
    double const leafN,
    double const lnb0,  // micromol / m^2 / s
    double const lnb1,
    double const o2,                   // mmol / mol
    double const par_energy_content,   // J / micromol
    double const par_energy_fraction,  // dimensionless
    double const RH,                   // Pa / Pa
    double const RL_at_25,             // micromol / m^2 / s
    double const solarR,               // micromol / m^2 / s
    double const StomataWS,            // dimensionless
    double const Tp_at_25,             // micromol / m^2 / s
    double Vcmax_at_25,                // micromol / m^2 / s
    double const WindSpeed,            // m / s
    double const WindSpeedHeight,      // m
    int const lnfun,                   // dimensionless switch
    int const nlayers                  // dimensionless
)
{
    PhotoCore::AtmosphereLightScattering const light_model(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double const q_dir = light_model.direct_fraction * solarR;    // micromol / m^2 / s
    double const q_diff = light_model.diffuse_fraction * solarR;  // micromol / m^2 / s

    PhotoCore::CanopyLight canopy_light_model(
        q_dir,
        q_diff,  // micromol / m^2 / s
        chil,
        cosine_zenith_angle,
        heightf,
        k_diffuse,
        LAI,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        par_energy_content,
        par_energy_fraction);

    // When lnfun == 0, nitrogen scaling is disabled: effective Vcmax = Vcmax_at_25 for all layers.
    // Encode as lnb0 = Vcmax_at_25, lnb1 = 0 so C3LeafPhoto::operator() computes
    // Vcmax = leafN * lnb1 + lnb0 = Vcmax_at_25 regardless of leafN.
    double const lnb0_eff = (lnfun != 0) ? lnb0 : Vcmax_at_25;
    double const lnb1_eff = (lnfun != 0) ? lnb1 : 0.0;

    PhotoCore::C3LeafPhoto leaf_photo(
        tr_param,
        /* iabs =          */ 0.0,   // overridden per layer by operator()
        /* j_shortwave =   */ 0.0,   // overridden per layer by operator()
        absorbed_longwave,
        ambient_temperature,
        atmospheric_pressure,
        RH,
        Gstar_at_25, Kc_at_25, Ko_at_25,
        Vcmax_at_25, lnb0_eff, lnb1_eff,
        Jmax_at_25, Tp_at_25, RL_at_25,
        b0, b1, Gs_min, StomataWS,
        Catm, o2,
        electrons_per_carboxylation, electrons_per_oxygenation,
        beta_PSII, gm_at_25,
        gbw_canopy, leaf_width,
        WindSpeed   // overridden per layer by operator()
    );

    PhotoCore::CanopyIntegrand<PhotoCore::C3LeafPhoto> integrand(
        leaf_photo,
        canopy_light_model,
        kpLN,
        leafN,    // micromol / m^2 / s
        WindSpeed // m / s
    );

    // use `quadrature::midpoint_rule` for previous behavior
    PhotoCore::LeafAssim const canopy =
        quadrature::gauss_legendre<2, PhotoCore::LeafAssim>(integrand, 0.0, LAI, nlayers);

    // Calculate the rate of whole-plant growth respiration
    double const whole_plant_gr =
        growth_resp(canopy.assim, growth_respiration_fraction);  // micromol / m^2 / s

    return canopy_photosynthesis_outputs{
        /* .Assim = */ canopy.assim - whole_plant_gr,                   // micromol / m^2 / s
        /* .canopy_conductance = */ canopy.stomatal_vapor_conductance,  // mol / m^2 / s
        /* .canopy_transpiration_penman = */ canopy.penman,             // mmol / m^2 / s
        /* .canopy_transpiration_priestly =*/canopy.priestly,           // mmol / m^2 / s
        /* .GrossAssim = */ canopy.carboxylation,                       // micromol / m^2 / s
        /* .RL = */ canopy.leaf_respiration,                            // micromol / m^2 / s
        /* .Rp = */ canopy.photorespiration,                            // micromol / m^2 / s
        /* .Trans = */ canopy.transpiration,                            // Mg / ha / hr
        /* .whole_plant_gr = */ whole_plant_gr                          // micromol / m^2 / s
    };
}
