#include <algorithm>  // for std::min, std::max
#include <math.h>
using namespace std;
/**
 * @brief functions to be used in soil evaporation computation
 * for multilayer soil profile.
 */

const double par_energy_content = 0.219;

double soil_albedo(
    double temp,
    double lai,
    double bare_soil_albedo,
    double soil_water_content[],
    double soil_field_capacity[])
{
    // SOILDYN.for, line 1574-156
    // line 1488 - SUBROUTINE ALBEDO(KTRANS, MEINF, MULCH, SOILPROP, SW1, XHLAI)
    // line 1510 - Calculate albedo changes with soil water content
    double ff = (soil_water_content[0] - 0.03) /
                (soil_field_capacity[0] - 0.03);
    ff = max(0.0, min(2.0, ff));
    double wet_soil_albedo = bare_soil_albedo * (1.0 - 0.45 * ff);

    // Compute albedo with canopy cover (SOILDYN.for, line 1613)
    // kd = Light extinction coefficient used for computation of plant
    //         transpiration
    // kd = 1.0;
    // double canopy_cover = 1.0 - exp(-kd * lai);
    // double canopy_albedo = canopy_cover * 0.23 +
    //               (1.0 - canopy_cover) * wet_soil_albedo;
    // Potential evapotranspiration (PET.for, line 891)
    double soil_albedo = (lai <= 0.0) ? wet_soil_albedo : 0.23 - (0.23 - wet_soil_albedo) * exp(-0.75 * lai);
    // Rprintf("soil_albedo is: %f \n", soil_albedo);
    return soil_albedo;
}

double potential_evapotranspiration(
    double solar,
    double temp,
    double lai,
    double wet_soil_albedo)
{
    // from PET.for, line 895
    double tavg = temp;                                      // deg C // average temperature. originally 0.60*TMAX+0.40*TMIN
    double srad = par_energy_content * 1e-6 * solar * 3600;  // micromole/m2/s to MJ/m2/hr.
    double solar_rad = srad * 23.923;
    // Rprintf("SOLAR is: %f (MJ/m2/hr)\n", srad);
    double equilibrium_evap = solar_rad * (2.04e-4 - 1.83e-4 * wet_soil_albedo) * (tavg + 29.0);
    // Rprintf("equilibrium_evap is: %f \n", equilibrium_evap);
    double potential_et = equilibrium_evap * 1.1;
    if (tavg > 35.0) {
        potential_et = equilibrium_evap * ((tavg - 35.0) * 0.05 + 1.1);
    } else if (tavg < 5.0) {
        potential_et = equilibrium_evap * 0.01 * exp(0.18 * (tavg + 20.0));
    }
    potential_et = max(potential_et, 0.0001);

    return potential_et;
}

double reference_evapotranspiration(
    int doy,
    double solar,
    double temp,
    double lat,  //latitude
    double elevation,
    double windspeed,
    double rh,
    double wet_soil_albedo)
{
    // PET.for, line 228
    double tavg = temp;                                      // Mean daily temperature (°C)
    double srad = par_energy_content * 1e-6 * solar * 3600;  // micromole/m2/s to MJ/m2/hr.
    // Atmospheric pressure, ASCE (2005) Eq. 3
    double atmos_pressure = 101.3 * pow(((293.0 - 0.0065 * elevation) / 293.0), 5.26);  // kPa

    // Psychrometric constant, ASCE (2005) Eq. 4
    double psychrometric_const = 0.000665 * atmos_pressure;  // kPa/deg C

    // Slope of the saturation vapor pressure-temperature curve
    // ASCE (2005) Eq. 5                                    !kPa/degC
    double udelta = 2503.0 * pow(exp(17.27 * tavg / (tavg + 237.3)) / (tavg + 237.3), 2.0);

    // Saturation vapor pressure, ASCE (2005) Eqs. 6 and 7
    double sat_vap_pressure = 0.6108 * exp((17.27 * tavg) / (tavg + 237.3));  // kPa
    // Actual vapor pressure, ASCE (2005) Table 3
    // double actual_vap_pressure = rh * sat_vap_pressure;
    // adjust actual evaporation rate based on conditions
    double ea;
    if (rh > 1.e-6)
        // ASCE (2005) Eq. 12
        ea = sat_vap_pressure * rh;  // kPa
    else
        // ASCE (2005) Appendix E, assume TDEW=TMIN-2.0
        ea = 0.6108 * exp((17.27 * (tavg - 2.0)) / ((tavg - 2.0) + 237.3));  // kPa

    // Net shortwave radiation, ASCE (2005) Eq. 16
    double rns = (1.0 - wet_soil_albedo) * srad;  //MJ/m2/hr

    // Extraterrestrial radiation, ASCE (2005) Eqs. 21,23,24,27
    double pie = 3.14159265359;
    double dr = 1.0 + 0.033 * cos(2.0 * pie / 365.0 * doy);         // Eq. 23
    double ldelta = 0.409 * sin(2.0 * pie / 365.0 * doy - 1.39);    // Eq. 24
    double ws = acos(-1.0 * tan(lat * pie / 180.0) * tan(ldelta));  // Eq. 27
    double ra1 = ws * sin(lat * pie / 180.0) * sin(ldelta);         // Eq. 21
    double ra2 = cos(lat * pie / 180.0) * cos(ldelta) * sin(ws);    // Eq. 21
    double ra = 24.0 / pie * 4.92 * dr * (ra1 + ra2);               // MJ/m2/hr Eq. 21

    // Clear sky solar radiation, ASCE (2005) Eq. 19
    double rso = (0.75 + 2E-5 * elevation) * ra;  // MJ/m2/hr

    // Net longwave radiation, ASCE (2005) Eqs. 17 and 18
    double ratio = srad / rso;
    if (ratio < 0.3)
        ratio = 0.3;
    else if (ratio > 1.0)
        ratio = 1.0;

    double fcd = 1.35 * ratio - 0.35;                                  // Eq 18
    double tk4 = pow((tavg + 273.16), 4.0);                            //Eq. 17
    double rnl = 4.901e-9 * fcd * (0.34 - 0.14 * pow(ea, 0.5)) * tk4;  // MJ/m2/hr Eq. 17

    // Net radiation, ASCE (2005) Eq. 15
    double rn = rns - rnl;  // MJ/m2/hr

    // Soil heat flux, ASCE (2005) Eq. 30
    double g = 0.0;  // MJ/m2/hr

    // Wind speed, ASCE (2005) Eq. 33 and Appendix E
    double wind2m = windspeed * (4.87 / log(67.8 * 2.0 - 5.42));

    // Aerodynamic roughness and surface resistance daily timestep constants
    // ASCE (2005) Table 1
    // Tall reference crop (50-cm alfalfa) for hourly during daytime
    double Cn = 66.0;  // K mm s^3 Mg^-1 d^-1
    double Cd = 0.95;  //s m^-1 (0.25 (day) - 1.7 (night) mm/hour)

    // Standardized reference evapotranspiration, ASCE (2005) Eq. 1
    double reference_et = 0.408 * udelta * (rn - g) + psychrometric_const *
                                                          (Cn / (tavg + 273.0)) * wind2m * (sat_vap_pressure - ea);
    reference_et = reference_et / (udelta + psychrometric_const * (1.0 + Cd * wind2m));  //mm/hr
    reference_et = max(0.0001, reference_et);

    return reference_et;
}

double potential_soil_evaporation(
    double skc,
    double kcbmax,
    double kd,
    double lai,
    double canopyHeight,
    double potential_et,
    double reference_et)  //latitude
{
    // double kcan = 0.85;// CSCER048.SPE, line 82
    double part = 0.07;
    double sradt = 0.25;
    double ksevap = (kd / (1.0 - part)) * (1.0 - sradt);
    // Rprintf("ksevap is: %f \n", ksevap);
    double potential_soil_evap = 0.0;
    double kcb = 0.0;
    double kcbmin = 0.0;

    if (lai > 0.0) {
        kcb = max(0.0, kcbmin + (kcbmax - kcbmin) * (1.0 - exp(-1.0 * skc * lai)));
    }
    // Maximum crop coefficient (Kcmax) (FAO-56 Eq. 72)
    double kcmax = max(1.0, kcb + 0.05);
    double fc = 0.0;  // effective canopy cover (FAO-56 Eq. 76)
    if (kcb > kcbmin) {
        fc = pow(((kcb - kcbmin) / (kcmax - kcbmin)), (1.0 + 0.5 * canopyHeight));
    }
    double fw = 1.0;                 // Wetted soil fraction (FAO-56 Eq. 75)
    double few = min(1.0 - fc, fw);  // Exposed and wetted soil fraction (FAO-56 Eq. 75)
    // KE = potential evaporation coefficient (FAO-56 Eq. 71)
    double ke = max(0.0, min(1.0 * (kcmax - kcb), few * kcmax));
    // double reference_et = 0.5;
    //if (ke >= 0.0) {
    //    potential_soil_evap = ke * reference_et;
    //} else if (ksevap <= 0.0) {
    //    if (lai <= 0.0)
    //      potential_soil_evap = potential_et * (1.0 - 0.39*lai);
    //    else
    //      potential_soil_evap = potential_et / 1.1 * exp(-0.4*lai);
    //}else {
    //    potential_soil_evap = potential_et * exp(-ksevap * lai);
    //}
    double attenuation = exp(-ksevap * lai);  // stronger LAI sensitivity
    potential_soil_evap = ke * reference_et * attenuation;
    potential_soil_evap = max(potential_soil_evap, 0.0);

    return potential_soil_evap;
}

// Calculate stage 1 soil evaporation
//   If the sum for stage 1 soil evaporation (SUMES1) is larger than
//   stage 1 evaporation limit (evap_limit), start stage 2 soil evaporation (SUMES2)
//   and adjust soil evaporation (actual_soil_evap)
struct evap_str {
    double sumes1;
    double sumes2;
    double time_factor;
    double actual_soil_evap;
};

evap_str supplemetal_evap_computation(
    double potential_soil_evap,
    double sumes1_temp,
    double sumes2_temp,
    double evap_limit,
    double time_factor_temp)
{
    evap_str return_value;
    double actual_soil_evap = 0.0;
    sumes1_temp += potential_soil_evap;
    if (sumes1_temp > evap_limit) {
        actual_soil_evap = potential_soil_evap - 0.4 * (sumes1_temp - evap_limit);
        sumes2_temp = 0.6 * (sumes1_temp - evap_limit);
        time_factor_temp = pow((sumes2_temp / 3.5), 2);
        sumes1_temp = evap_limit;
    } else
        actual_soil_evap = potential_soil_evap;

    return_value.sumes1 = sumes1_temp;
    return_value.sumes2 = sumes2_temp;
    return_value.time_factor = time_factor_temp;
    return_value.actual_soil_evap = actual_soil_evap;
    return return_value;
}
