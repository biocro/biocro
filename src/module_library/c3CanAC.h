#ifndef C3CANAC_H
#define C3CANAC_H

#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs
#include "c3_temperature_response.h"        // for c3_temperature_response_parameters

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
);

#endif
