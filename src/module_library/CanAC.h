#ifndef C4CANAC_H
#define C4CANAC_H

#include "AuxBioCro.h"                      // for nitroParms
#include "canopy_photosynthesis_outputs.h"  // for canopy_photosynthesis_outputs

canopy_photosynthesis_outputs CanAC(
    double LAI,
    double cosine_zenith_angle,
    double solarR,
    double ambient_temperature,
    double RH,
    double WindSpeed,
    int nlayers,
    double Vmax,
    double Alpha,
    double Kparm,
    double beta,
    double Rd,
    double Catm,
    double b0,
    double b1,
    double Gs_min,
    double theta,
    double kd,
    double chil,
    double leafN,
    double kpLN,
    int lnfun,
    double upperT,
    double lowerT,
    const nitroParms& nitroP,
    double leafwidth,
    int eteq,
    double StomataWS,
    double specific_heat_of_air,
    double atmospheric_pressure,
    double atmospheric_transmittance,
    double atmospheric_scattering,
    double absorptivity_par,
    double par_energy_content,
    double par_energy_fraction,
    double leaf_transmittance,
    double leaf_reflectance,
    double minimum_gbw);

#endif
