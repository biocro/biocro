#ifndef SOIL_EVAPORATION_FUNCTIONS_H
#define SOIL_EVAPORATION_FUNCTIONS_H

#include <algorithm>  // for std::min, std::max
#include <stdexcept>  // for std::range_error

/**
 * @brief functions to be used in soil evaporation computation
 * for multilayer soil profile.
 */

const double par_energy_content = 0.219;

/**
 *  @brief Calculates the surface albedo, which is the ratio of reflected to
 *  incoming radiation.
 *
 *  This function implements some empirical relationships to estimate surface
 *  albedo from volumetric water content at the soil surface and crop cover.
 *
 *  The estimate for bare soil albedo is based on measurements reported in Idso
 *  et al. (1975). For Avondale loam with volumetric soil water content in the
 *  range from 0.0 to roughly 0.18, the albedo was found to depend linearly on
 *  water content in the top 20 cm of soil, with wetter soil having a smaller
 *  albedo. Completely dry soil reached an albedo of roughly 0.30, and as the
 *  soil water content approached the field capacity, the albedo approached a
 *  minimum value of approximately 0.14.
 *
 *  Here these observations are generalized to accomodate different soil types.
 *  When the soil water content in the upper layer (`theta_surface`) is at or
 *  below a minimum threshold water content (`theta_min`), the bare soil albedo
 *  is set to its maximum value (`bare_soil_albedo_max`), which may vary with
 *  soil type. At the field capacity (`theta_fc_surface`), which also varies
 *  with soil type, the bare soil albedo is set to a particular fraction of its
 *  maximum value (`albedo_frac * bare_soil_albedo_max`). These two points
 *  define a linear relationship that applies for soil water content above
 *  `theta_min` and below a maximum threshold (`theta_max`). For soil water
 *  content above `theta_max`, the albedo is set to the value it takes at
 *  `theta_max`.
 *
 *  The presence of a crop canopy above the soil alters the surface albedo from
 *  its bare soil value. A simple way to model this is to assume that a light
 *  ray is either intercepted by the canopy or passes through it, thereby
 *  reaching the soil surface. For light intercepted by the canopy, the fraction
 *  of reflected light is determined by the canopy albedo; for light reaching
 *  the soil, the fraction of reflected light is determined by the soil albedo.
 *  In other words, `soil_albedo = bare_soil_albedo * canopy_transmittance +
 *  canopy_albedo * (1 - canopy_transmittance)`, where `canopy_transmittance` is
 *  the fraction of incident light transmitted through the canopy. Note that
 *  this simple approach neglects "multiple scattering," which refers to the
 *  possibility that a light ray may be transmitted through the canopy and
 *  subsequently reflected by the soil, possibly scattering back and forth many
 *  times.
 *
 *  Here, a simple estimate for the canopy transmittance is made based on the
 *  total canopy leaf area index (`LAI`) and an exponential decay parameter
 *  (`k_canopy`), where `canopy_transmittance = exp(-k_canopy * LAI)`.
 *
 *  Note: This function was originally based on the `ALBEDO` subroutine of
 *  `SOILDYN.for` from DSSAT (https://github.com/DSSAT/dssat-csm-os).
 *
 *  References:
 *
 *  - [Idso, S. B., Jackson, R. D., Reginato, R. J., Kimball, B. A. & Nakayama, F. S. "The Dependence of
 *    Bare Soil Albedo on Soil Water Content" Journal of Applied Meteorology and Climatology 14, 109–113 (1975)]
 *    (https://doi.org/10.1175/1520-0450(1975)014<0109:TDOBSA>2.0.CO;2)
 *
 *  @param [in] LAI Total canopy leaf area index; dimensionless from
 *              (m^2 leaf) / (m^2 ground)
 *
 *  @param [in] bare_soil_albedo_max The maximum bare soil albedo, which occurs
 *              for completely dry soil; dimensionless from W / W
 *
 *  @param [in] theta_surface Volumetric soil water content in the upper soil
 *              layer; dimensionless from (m^3 water) / (m^3 soil)
 *
 *  @param [in] theta_fc_surface The field capacity of the upper soil level,
 *              expressed as a volumetric water content; dimensionless from
 *              (m^3 water) / (m^3 soil)
 *
 *  @return Surface albedo accounting for bare soil and canopy; dimensionless
 *          from W / W
 *
 */
double surface_albedo(
    double const LAI,                   // dimensionless
    double const bare_soil_albedo_max,  // dimensionless
    double const theta_surface,         // dimensionless from m^3 / m^3
    double const theta_fc_surface       // dimensionless from m^3 / m^3
)
{
    // Set constants
    double constexpr theta_min = 0.03;      // dimensionless
    double constexpr albedo_frac = 0.55;    // dimensionless
    double constexpr k_canopy = 0.75;       // dimensionless
    double constexpr canopy_albedo = 0.23;  // dimensionless

    // Upper threshold for water content (dimensionless)
    double const theta_max = theta_min + 2.0 * (theta_fc_surface - theta_min);

    // Slope of albedo vs. water content in linear range (dimensionless)
    double const slope =
        bare_soil_albedo_max * (albedo_frac - 1.0) / (theta_fc_surface - theta_min);

    // Effective soil water content, which is clamped to lie within the bounds
    // placed on soil water content (dimensionless)
    double const theta_effective =
        std::max(theta_min, std::min(theta_surface, theta_max));

    // Bare soil albedo, accounting for water content (dimensionless)
    double const bare_soil_albedo =
        bare_soil_albedo_max + slope * (theta_effective - theta_min);

    // Fraction of light transmitted through canopy (dimensionless)
    double const canopy_transmittance = exp(-k_canopy * LAI);

    // Effective surface albedo including bare soil and canopy (dimensionless)
    return bare_soil_albedo * canopy_transmittance +
           canopy_albedo * (1 - canopy_transmittance);
}

double potential_evapotranspiration(
    double solar,
    double temp,
    double lai,
    double wet_soil_albedo)
{
    using std::max;

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
    using std::max;

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
    using std::max;
    using std::min;

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
#endif
