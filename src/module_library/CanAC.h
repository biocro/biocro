#ifndef C4CANAC_H
#define C4CANAC_H

#include "AuxBioCro.h"  // for nitroParms
#include "canopy_photosynthesis_outputs.h"

canopy_photosynthesis_outputs CanAC(
    nitroParms const& nitroP,
    double const absorbed_longwave,  // J / m^2 / s
    double const Alpha,
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta,                         // dimensionless
    double const Catm,                         // ppm
    double const chil,                         // dimensionless
    double const cosine_zenith_angle,          // dimensionless
    double const gbw_canopy,                   // m / s
    double const growth_respiration_fraction,  // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const k_diffuse,                    // dimensionless
    double const Kparm,
    double const kpLN,
    double const LAI,                     // dimensionless from m^2 / m^2
    double const leaf_reflectance_nir,    // dimensionless
    double const leaf_reflectance_par,    // dimensionless
    double const leaf_transmittance_nir,  // dimensionless
    double const leaf_transmittance_par,  // dimensionless
    double const leafN,
    double const leafwidth,              // m
    double const lowerT,                 // degrees C
    double const nir_incident_diffuse,   // J / m^2 / s
    double const nir_incident_direct,    // J / m^2 / s
    double const par_energy_content,     // J / micromol
    double const par_energy_fraction,    // dimensionless
    double const ppfd_incident_diffuse,  // micromol / m^2 / s
    double const ppfd_incident_direct,   // micromol / m^2 / s
    double const RH,                     // dimensionless from Pa / Pa
    double const RL_at_25,               // micromol / m^2 / s
    double const solarR,                 // micromol / m^2 / s
    double const StomataWS,              // dimensionless
    double const theta,                  // dimensionless
    double const upperT,                 // degrees C
    double const Vcmax_at_25,            // micromol / m^2 / s
    double const WindSpeed,              // m / s
    int const lnfun,                     // dimensionless switch
    int const nlayers                    // dimensionless
);

#endif
