#ifndef C3CANAC_H
#define C3CANAC_H

#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs

canopy_photosynthesis_outputs c3CanAC(
    double LAI,
    double cosine_zenith_angle,
    double solarR,
    double ambient_temperature,
    double RH,
    double WindSpeed,
    int nlayers,
    double Vmax,
    double Jmax,
    double tpu_rate_max,
    double Rd,
    double Catm,
    double o2,
    double b0,
    double b1,
    double Gs_min,
    double theta,
    double kd,
    double heightf,
    double leafN,
    double kpLN,
    double lnb0,
    double lnb1,
    int lnfun,
    double chil,
    double StomataWS,
    double specific_heat_of_air,
    double atmospheric_pressure,
    double atmospheric_transmittance,
    double atmospheric_scattering,
    double growth_respiration_fraction,
    double electrons_per_carboxylation,
    double electrons_per_oxygenation,
    double absorptivity_par,
    double par_energy_content,
    double par_energy_fraction,
    double leaf_transmittance,
    double leaf_reflectance,
    double minimum_gbw,
    double WindSpeedHeight,
    double beta_PSII);

#endif
