#ifndef C4PHOTO_H
#define C4PHOTO_H

#include "../math/roots/onedim/roots.h"  // for root_finding::result_t
#include "photosynthesis_outputs.h"      // for photosynthesis_outputs

double collatz_assim(
    double const atmospheric_pressure,  // Pa
    double const beta,                  // dimensionless
    double const Ci_pa,                 // Pa
    double const kT,                    // mol / m^2 / s
    double const M,                     // micromol / m^2 / s
    double const RT                     // micromol / m^2 / s
);

double check_c4_assim_rate(
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const bb0_adj,               // mol / m^2 / s
    double const bb1_adj,               // dimensionless
    double const beta,                  // dimensionless
    double const Ca,                    // micromol / mol
    double const Ca_pa,                 // Pa
    double const Ci_pa,                 // Pa
    double const gbw,                   // mol / m^2 / s
    double const kT,                    // micromol / m^2 / s
    double const leaf_temperature,      // degrees C
    double const M,                     // micromol / m^2 / s
    double const relative_humidity,     // dimensionless
    double const RT                     // micromol / m^2 / s
);

photosynthesis_outputs c4photoC(
    double const Qp,                    // micromol / m^2 / s
    double const leaf_temperature,      // degrees C
    double const ambient_temperature,   // degrees C
    double const relative_humidity,     // dimensionless from Pa / Pa
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const alpha,                 // mol / mol
    double const kparm,                 // mol / m^2 / s
    double const theta,                 // dimensionless
    double const beta,                  // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const bb0,                   // mol / m^2 / s
    double const bb1,                   // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double const Gs_min,                // mol / m^2 / s
    double const StomaWS,               // dimensionless
    double const Ca,                    // micromol / mol
    double const atmospheric_pressure,  // Pa
    double const upperT,                // degrees C
    double const lowerT,                // degrees C
    double const gbw                    // mol / m^2 / s
);

double check_c4_gs(
    double const absorbed_longwave,     // J / (m^2 leaf) / s
    double const absorbed_shortwave,    // J / (m^2 leaf) / s
    double const alpha,                 // mol / mol
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const b0,                    // mol / m^2 / s
    double const b1,                    // dimensionless
    double const beta,                  // dimensionless
    double const Catm,                  // micromol / mol
    double const current_gs,            // mol / m^2 / s
    double const gbw_canopy,            // m / s
    double const Gs_min,                // mol / m^2 / s
    double const incident_ppfd,         // micromol / m^2 / s
    double const kparm,                 // mol / m^2 / s
    double const leafwidth,             // m
    double const lowerT,                // degrees C
    double const rh,                    // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const StomataWS,             // dimensionless
    double const theta,                 // dimensionless
    double const upperT,                // degrees C
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const windspeed              // m / s
);

root_finding::result_t solve_c4_gs(
    double const absorbed_longwave,     // J / (m^2 leaf) / s
    double const absorbed_shortwave,    // J / (m^2 leaf) / s
    double const alpha,                 // mol / mol
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const b0,                    // mol / m^2 / s
    double const b1,                    // dimensionless
    double const beta,                  // dimensionless
    double const Catm,                  // micromol / mol
    double const gbw_canopy,            // m / s
    double const Gs_min,                // mol / m^2 / s
    double const incident_ppfd,         // micromol / m^2 / s
    double const kparm,                 // mol / m^2 / s
    double const leafwidth,             // m
    double const lowerT,                // degrees C
    double const rh,                    // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const StomataWS,             // dimensionless
    double const theta,                 // dimensionless
    double const upperT,                // degrees C
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const windspeed              // m / s
);

#endif
