#ifndef BIOCRO_H
#define BIOCRO_H

#include "AuxBioCro.h"

struct Light_model {
	double direct_irradiance_fraction;
	double diffuse_irradiance_fraction;
};

struct Can_Str CanAC(
    double LAI, double cosine_zenith_angle, double solarR, double Temp,
    double RH, double WindSpeed, int nlayers, double Vmax, double Alpha,
    double Kparm, double beta, double Rd, double Catm, double b0, double b1,
    double Gs_min, double theta, double kd, double chil, double leafN,
    double kpLN, int lnfun, double upperT, double lowerT,
    const struct nitroParms &nitroP, double leafwidth, int eteq,
    double StomataWS, double specific_heat_of_air, double atmospheric_pressure,
    int water_stress_approach, double absorptivity_par,
    double par_energy_content, double par_energy_fraction,
    double leaf_transmittance, double leaf_reflectance, double minimum_gbw);

struct Can_Str c3CanAC(
    double LAI, double cosine_zenith_angle, double solarR, double Temp,
    double RH, double WindSpeed, int nlayers, double Vmax, double Jmax,
    double tpu_rate_max, double Rd, double Catm, double o2, double b0,
    double b1, double Gs_min, double theta, double kd, double heightf,
    double leafN, double kpLN, double lnb0, double lnb1, int lnfun, double chil,
    double StomataWS, double specific_heat_of_air, double atmospheric_pressure,
    double growth_respiration_fraction, int water_stress_approach,
    double electrons_per_carboxylation, double electrons_per_oxygenation,
    double absorptivity_par, double par_energy_content,
    double par_energy_fraction, double leaf_transmittance,
    double leaf_reflectance, double minimum_gbw, double WindSpeedHeight,
    double beta_PSII);

double resp(double comp, double mrc, double temp);

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, double fieldc,
                     double wiltp, double soil_saturation_capacity, double soil_sand_content,
                     double Ks, double air_entry, double b);


double SoilEvapo(
    double LAI, double k, double air_temperature, double ppfd,
    double soil_water_content, double fieldc, double wiltp, double winds,
    double RelH, double rsec, double soil_clod_size, double soil_reflectance,
    double soil_transmission, double specific_heat_of_air,
    double par_energy_content);

struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth, double *depths,
        double soil_field_capacity, double soil_wilting_point, double soil_saturation_capacity, double soil_air_entry, double soil_saturated_conductivity,
        double soil_b_coefficient, double soil_sand_content, double phi1, double phi2, int wsFun,
        int layers, double rootDB, double LAI, double k, double AirTemp,
        double IRad, double winds, double RelH, int hydrDist, double rfl,
        double rsec, double rsdf, double soil_clod_size, double soil_reflectance, double soil_transmission,
        double specific_heat_of_air, double par_energy_content);

void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, int nlayers, double* wind_speed_profile);

double absorbed_from_incident_in_canopy(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
);

double absorbed_shortwave_from_incident_ppfd(
    double incident_ppfd,        // micromol / m^2 / s
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double leaf_reflectance,     // dimensionless
    double leaf_transmittance    // dimensionless
);
struct Light_profile sunML(
    double ambient_ppfd_beam,     // micromol / (m^2 beam) / s
    double ambient_ppfd_diffuse,  // micromol / m^2 / s
    double lai,                   // dimensionless from m^2 / m^2
    int nlayers,                  // dimensionless
    double cosine_zenith_angle,   // dimensionless
    double kd,                    // dimensionless
    double chil,                  // dimensionless from m^2 / m^2
    double absorptivity,          // dimensionless from mol / mol
    double heightf,               // m^-1 from m^2 leaf / m^2 ground / m height
    double par_energy_content,    // J / micromol
    double par_energy_fraction,   // dimensionless
    double leaf_transmittance,    // dimensionless
    double leaf_reflectance       // dimensionless
);

struct Light_model lightME(double cosine_zenith_angle, double atmospheric_pressure);

struct FL_str FmLcFun(double Lig, double Nit);

struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]);
double AbiotEff(double smoist, double stemp);

struct ET_Str EvapoTrans2(
    double absorbed_shortwave_radiation_et,
    double absorbed_shortwave_radiation_lt,
    double airTemp,
    double RH,
    double WindSpeed,
    double stomatal_conductance,
    double leaf_width,
    double specific_heat_of_air,
    double minimum_gbw,
    int eteq
);

struct ET_Str c3EvapoTrans(
    double absorbed_shortwave_radiation,
    double air_temperature,
    double RH,
    double WindSpeed,
    double CanopyHeight,
    double specific_heat_of_air,
    double stomatal_conductance,
    double minimum_gbw,
    double WindSpeedHeight
);

#endif

