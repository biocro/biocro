#ifndef C4CANAC_H
#define C4CANAC_H

#include "AuxBioCro.h"  // for Can_Str and nitroParms

struct Can_Str CanAC(
    double LAI,
    double cosine_zenith_angle,
    double solarR,
    double temperature,
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
    const struct nitroParms& nitroP,
    double leafwidth,
    int eteq,
    double StomataWS,
    double specific_heat_of_air,
    double atmospheric_pressure,
    int water_stress_approach,
    double absorptivity_par,
    double par_energy_content,
    double par_energy_fraction,
    double leaf_transmittance,
    double leaf_reflectance,
    double minimum_gbw);

#endif
