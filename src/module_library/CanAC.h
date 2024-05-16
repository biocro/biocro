#ifndef C4CANAC_H
#define C4CANAC_H

#include "AuxBioCro.h"                      // for nitroParms
#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs

canopy_photosynthesis_outputs CanAC(
    double LAI,                  // dimensionless from m^2 / m^2
    double cosine_zenith_angle,  // dimensionless
    double solarR,               // micromol / m^2 / s
    double ambient_temperature,  // degrees C
    double RH,                   // dimensionless from Pa / Pa
    double WindSpeed,            // m / s
    int nlayers,                 // dimensionless
    double Vmax,
    double Alpha,
    double Kparm,
    double beta,  // dimensionless
    double Rd,
    double Catm,  // ppm
    double b0,
    double b1,
    double Gs_min,  // mol / m^2 / s
    double theta,   // dimensionless
    double k_diffuse,
    double chil,
    double leafN,
    double kpLN,
    int lnfun,      // dimensionless switch
    double upperT,  // degrees C
    double lowerT,  // degrees C
    const nitroParms& nitroP,
    double leafwidth,                  // m
    int eteq,                          // dimensionless switch
    double StomataWS,                  // dimensionless
    double specific_heat_of_air,       // J / kg / K
    double atmospheric_pressure,       // Pa
    double atmospheric_transmittance,  // dimensionless
    double atmospheric_scattering,     // dimensionless
    double absorptivity_nir,           // dimensionless
    double absorptivity_par,           // dimensionless
    double par_energy_content,         // J / micromol
    double par_energy_fraction,        // dimensionless
    double leaf_transmittance_nir,     // dimensionless
    double leaf_transmittance_par,     // dimensionless
    double leaf_reflectance_nir,       // dimensionless
    double leaf_reflectance_par,       // dimensionless
    double minimum_gbw                 // mol / m^2 / s
);

#endif
