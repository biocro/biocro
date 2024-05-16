#ifndef C3CANAC_H
#define C3CANAC_H

#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs

canopy_photosynthesis_outputs c3CanAC(
    double LAI,                  // dimensionless
    double cosine_zenith_angle,  // hr
    double solarR,               // micromol / m^2 / s
    double ambient_temperature,  // degrees C
    double RH,                   // Pa / Pa
    double WindSpeed,            // m / s
    int nlayers,                 // dimensionless
    double Vmax,
    double Jmax,
    double tpu_rate_max,
    double Rd,
    double Catm,  // ppm
    double o2,
    double b0,
    double b1,
    double Gs_min,  // mol / m^2 / s
    double theta,   // dimensionless
    double k_diffuse,
    double heightf,
    double leafN,
    double kpLN,
    double lnb0,
    double lnb1,
    int lnfun,  // dimensionless switch
    double chil,
    double StomataWS,                    // dimensionless
    double specific_heat_of_air,         // J / kg / K
    double atmospheric_pressure,         // Pa
    double atmospheric_transmittance,    // dimensionless
    double atmospheric_scattering,       // dimensionless
    double growth_respiration_fraction,  // dimensionless
    double electrons_per_carboxylation,  // self-explanatory units
    double electrons_per_oxygenation,    // self-explanatory units
    double absorptivity_nir,             // dimensionless
    double absorptivity_par,             // dimensionless
    double par_energy_content,           // J / micromol
    double par_energy_fraction,          // dimensionless
    double leaf_transmittance_nir,       // dimensionless
    double leaf_transmittance_par,       // dimensionless
    double leaf_reflectance_nir,         // dimensionless
    double leaf_reflectance_par,         // dimensionless
    double minimum_gbw,                  // mol / m^2 / s
    double WindSpeedHeight,              // m
    double beta_PSII                     // dimensionless (fraction of absorbed light that reaches photosystem II)
);

#endif
