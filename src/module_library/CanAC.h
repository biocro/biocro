#ifndef C4CANAC_H
#define C4CANAC_H

#include "AuxBioCro.h"                      // for nitroParms
#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs

canopy_photosynthesis_outputs CanAC(
    const nitroParms& nitroP,
    double absorbed_longwave,  // J / m^2 / s
    double Alpha,
    double ambient_temperature,        // degrees C
    double atmospheric_pressure,       // Pa
    double atmospheric_scattering,     // dimensionless
    double atmospheric_transmittance,  // dimensionless
    double b0,                         // mol / m^2 / s
    double b1,                         // dimensionless
    double beta,                       // dimensionless
    double Catm,                       // ppm
    double chil,                       // dimensionless
    double cosine_zenith_angle,        // dimensionless
    double gbw_canopy,                 // m / s
    double Gs_min,                     // mol / m^2 / s
    double Kparm,
    double kpLN,
    double k_diffuse,  // dimensionless
    double LAI,        // dimensionless from m^2 / m^2
    double leafN,
    double leafwidth,               // m
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double lowerT,                  // degrees C
    double par_energy_content,      // J / micromol
    double par_energy_fraction,     // dimensionless
    double Rd,                      // micromol / m^2 / s
    double RH,                      // dimensionless from Pa / Pa
    double solarR,                  // micromol / m^2 / s
    double StomataWS,               // dimensionless
    double theta,                   // dimensionless
    double upperT,                  // degrees C
    double Vmax,                    // micromol / m^2 / s
    double WindSpeed,               // m / s
    int lnfun,                      // dimensionless switch
    int nlayers                     // dimensionless
);

#endif
