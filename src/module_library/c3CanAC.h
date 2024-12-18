#ifndef C3CANAC_H
#define C3CANAC_H

#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs
#include "c3_temperature_response.h"        // for c3_temperature_response_parameters

canopy_photosynthesis_outputs c3CanAC(
    c3_temperature_response_parameters const tr_param,
    double absorbed_longwave,            // J / m^2 / s
    double ambient_temperature,          // degrees C
    double atmospheric_pressure,         // Pa
    double atmospheric_scattering,       // dimensionless
    double atmospheric_transmittance,    // dimensionless
    double b0,                           // mol / m^2 / s
    double b1,                           // dimensionless
    double beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double Catm,                         // ppm
    double chil,                         // dimensionless
    double cosine_zenith_angle,          // dimensionless
    double electrons_per_carboxylation,  // self-explanatory units
    double electrons_per_oxygenation,    // self-explanatory units
    double gbw_canopy,                   // m / s
    double growth_respiration_fraction,  // dimensionless
    double Gs_min,                       // mol / m^2 / s
    double heightf,                      // m^(-1)
    double Jmax,                         // micromol / m^2 / s
    double kpLN,
    double k_diffuse,  // dimensionless
    double LAI,        // dimensionless
    double leafN,
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leaf_width,              // m
    double lnb0,                    // micromol / m^2 / s
    double lnb1,
    double o2,                   // mmol / mol
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double Rd,                   // micromol / m^2 / s
    double RH,                   // Pa / Pa
    double solarR,               // micromol / m^2 / s
    double StomataWS,            // dimensionless
    double tpu_rate_max,         // micromol / m^2 / s
    double Vmax,                 // micromol / m^2 / s
    double WindSpeed,            // m / s
    double WindSpeedHeight,      // m
    int lnfun,                   // dimensionless switch
    int nlayers                  // dimensionless
);

#endif
