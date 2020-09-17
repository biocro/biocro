#ifndef BIOCRO_H
#define BIOCRO_H

#include "AuxBioCro.h"

double cos_zenith_angle(const double latitude, const int day_of_year,
                        const double hour_of_day);

struct Light_model {
	double direct_irradiance_fraction;
	double diffuse_irradiance_fraction;
	double cosine_zenith_angle;
};

struct Can_Str CanAC(double LAI, int DOY, double hr, double solarR, double Temp,
		     double RH, double WindSpeed, double lat, int nlayers, double Vmax, double Alpha, 
		     double Kparm, double beta, double Rd, double Catm, double b0, 
		     double b1, double Gs_min, double theta, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun, double upperT,
		     double lowerT, const struct nitroParms &nitroP, double leafwidth, int eteq, double StomataWS, int water_stress_approach);
         
struct Can_Str c3CanAC(double LAI, int DOY, double hr, double solarR, double Temp,
                       double RH, double WindSpeed, double lat, int nlayers, double Vmax,
                       double Jmax, double tpu_rate_max, double Rd, double Catm, double o2, double b0,
                       double b1, double Gs_min, double theta, double kd, double heightf, double leafN,
                       double kpLN, double lnb0, double lnb1, int lnfun, double chil,
                       double StomataWS, double growth_respiration_fraction, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation);
                        

double resp(double comp, double mrc, double temp);

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, double fieldc,
                     double wiltp, double phi1, double phi2, double soil_saturation_capacity, double soil_sand_content,
                     double Ks, double air_entry, double b);


double SoilEvapo(double LAI, double k, double AirTemp, double DirectRad,
        double awc, double fieldc, double wiltp, double winds, double RelH, double rsec,
        double soil_clod_size, double soil_reflectance, double soil_transmission, double specific_heat, double stefan_boltzman);

struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth, double *depths,
        double soil_field_capacity, double soil_wilting_point, double soil_saturation_capacity, double soil_air_entry, double soil_saturated_conductivity,
        double soil_b_coefficient, double soil_sand_content, double phi1, double phi2, int wsFun,
        int layers, double rootDB, double LAI, double k, double AirTemp,
        double IRad, double winds, double RelH, int hydrDist, double rfl,
        double rsec, double rsdf, double soil_clod_size, double soil_reflectance, double soil_transmission,
        double specific_heat, double stefan_boltzman);

void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, int nlayers, double* wind_speed_profile);
struct Light_profile sunML(double Idir, double Idiff, double LAI, int nlayers, double cosTheta, double kd, double chil, double heightf);
struct Light_model lightME(double lat, int DOY, double td);

struct FL_str FmLcFun(double Lig, double Nit);

struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]);
double AbiotEff(double smoist, double stemp);

struct ET_Str EvapoTrans2(double Rad, double Iave, double Airtemperature, double RH, double WindSpeed,
        double LeafAreaIndex, double CanopyHeight, double stomatacond, double leafw, int eteq);

struct ET_Str c3EvapoTrans(double Itot, double Airtemperature, double RH, double WindSpeed, double CanopyHeight,
        double stomtal_conductance);

#endif

