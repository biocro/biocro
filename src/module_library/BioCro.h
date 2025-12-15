#ifndef BIOCRO_H
#define BIOCRO_H

#include <vector>
#include "AuxBioCro.h"

ws_str watstr(double precipit, double evapo, double cws, double soildepth, double fieldc,
                     double wiltp, double soil_saturation_capacity, double soil_sand_content,
                     double Ks, double air_entry, double b);


double SoilEvapo(
    double LAI, double k, double air_temperature, double ppfd,
    double soil_water_content, double fieldc, double wiltp, double winds,
    double RelH, double rsec, double soil_clod_size, double soil_reflectance,
    double soil_transmission, double specific_heat_of_air,
    double par_energy_content);

soilML_str soilML(double precipit, double transp, double *cws, double soildepth, double *depths,
        double soil_field_capacity, double soil_wilting_point, double soil_saturation_capacity, double soil_air_entry, double soil_saturated_conductivity,
        double soil_b_coefficient, double soil_sand_content, double phi1, double phi2, int wsFun,
        int layers, double rootDB, double LAI, double k, double AirTemp,
        double IRad, double winds, double RelH, int hydrDist, double rfl,
        double rsec, double rsdf, double soil_clod_size, double soil_reflectance, double soil_transmission,
        double specific_heat_of_air, double par_energy_content);

void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, std::vector<double>& wind_speed_profile);

double AbiotEff(double smoist, double stemp);

#endif

