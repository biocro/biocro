#ifndef C3PHOTO_H
#define C3PHOTO_H

#include "../math/roots/onedim/roots.h"  // for root_finding::result_t
#include "c3_temperature_response.h"     // for c3_temperature_response_parameters
#include "photosynthesis_outputs.h"      // for photosynthesis_outputs

double check_c3_assim_rate(
    double const alpha_TPU,                    // dimensionless
    double const b0_adj,                       // mol / m^2 / s
    double const b1_adj,                       // dimensionless
    double const Ca,                           // micromol / mol
    double const Cc,                           // micromol / mol
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw,                          // mol / m^2 / s
    double const Gstar,                        // micromol / mol
    double const J,                            // micromol / m^2 / s
    double const Kc,                           // micromol / mol
    double const Ko,                           // mmol / mol
    double const Oi,                           // mmol / mol
    double const RH,                           // dimensionless
    double const RL,                           // micromol / m^2 / s
    double const Tambient,                     // degrees C
    double const Tleaf,                        // degrees C
    double const TPU,                          // micromol / m^2 / s
    double const Vcmax                         // micromol / m^2 / s
);

photosynthesis_outputs c3photoC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_ppfd,                // micromol / m^2 / s
    double const Tleaf,                        // degrees C
    double const Tambient,                     // degrees C
    double const RH,                           // dimensionless
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gstar_at_25,                  // micromol / mol
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const TPU_rate_max,                 // micromol / m^2 / s
    double const RL_at_25,                     // micromol / m^2 / s
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Ca,                           // micromol / mol
    double const atmospheric_pressure,         // Pa
    double const O2,                           // millimol / mol (atmospheric oxygen mole fraction)
    double const StomWS,                       // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const gbw                           // mol / m^2 / s
);

double check_c3_gs(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / (m^2 leaf) / s
    double const absorbed_ppfd,                // micromol / (m^2 leaf) / s
    double const absorbed_shortwave,           // J / (m^2 leaf) / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless
    double const Catm,                         // micromol / mol
    double const current_gs,                   // mol / m^2 / s
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // m / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const leafwidth,                    // m
    double const O2,                           // mmol / mol
    double const rh,                           // dimensionless
    double const RL_at_25,                     // micromol / m^2 / s
    double const StomataWS,                    // dimensionless
    double const Tp_at_25,                     // micromol / m^2 / s
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const windspeed                     // m / s
);

root_finding::result_t solve_c3_gs(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / (m^2 leaf) / s
    double const absorbed_ppfd,                // micromol / (m^2 leaf) / s
    double const absorbed_shortwave,           // J / (m^2 leaf) / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless
    double const Catm,                         // micromol / mol
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // m / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const leafwidth,                    // m
    double const O2,                           // mmol / mol
    double const rh,                           // dimensionless
    double const RL_at_25,                     // micromol / m^2 / s
    double const StomataWS,                    // dimensionless
    double const Tp_at_25,                     // micromol / m^2 / s
    double const Vcmax_at_25,                  // micromol / m^2 / s
    double const windspeed                     // m / s
);

double solo(double LeafT);

#endif
