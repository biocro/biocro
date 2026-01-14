#ifndef SOIL_EVAPORATION_FUNCTIONS_H
#define SOIL_EVAPORATION_FUNCTIONS_H

#include <algorithm>                 // for std::min, std::max
#include "../framework/constants.h"  // for pi
#include <stdexcept>                 // for std::range_error

/**
 * @brief functions to be used in soil evaporation computation
 * for multilayer soil profile.
 */

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
 *  A typical value of canopy albedo for crops is 0.23; see the text following
 *  Equation 2 of Ritchie (1972).
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
 *  - [Ritchie, J. T. "Model for predicting evaporation from a row crop with incomplete cover."
 *    Water Resources Research 8, 1204–1213 (1972)]
 *    (https://doi.org/10.1029/WR008i005p01204)
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
    // Check for bad inputs
    if (LAI < 0) {
        throw std::range_error("Thrown in surface_albedo: LAI is negative.");
    }

    if (bare_soil_albedo_max < 0) {
        throw std::range_error("Thrown in surface_albedo: bare_soil_albedo_max is negative.");
    }

    if (theta_surface < 0) {
        throw std::range_error("Thrown in surface_albedo: theta_surface is negative.");
    }

    if (theta_fc_surface < 0) {
        throw std::range_error("Thrown in surface_albedo: theta_fc_surface is negative.");
    }

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

    // Minimum soil albedo (dimensionless)
    double const bare_soil_albedo_min =
        bare_soil_albedo_max + slope * (theta_max - theta_min);

    // Bare soil albedo, accounting for water content (dimensionless)
    double const bare_soil_albedo =
        theta_surface < theta_min   ? bare_soil_albedo_max
        : theta_surface < theta_max ? bare_soil_albedo_max + slope * (theta_surface - theta_min)
                                    : bare_soil_albedo_min;

    // Fraction of light transmitted through canopy (dimensionless)
    double const canopy_transmittance = exp(-k_canopy * LAI);

    // Effective surface albedo including bare soil and canopy (dimensionless)
    return bare_soil_albedo * canopy_transmittance +
           canopy_albedo * (1 - canopy_transmittance);
}

/**
 *  @brief Calculates the reference evapotranspiration rate from environmental
 *  conditions.
 *
 *  Here we generally follow the hourly "tall crop" calculations described in
 *  ASCE (2005), with a few key differences:
 *
 *  1. The atmospheric pressure is an input, so it is not necessarily calculated
 *     using Equation 3 from ASCE (2005). This is to accomodate weather data
 *     sets that include measured values of local atmospheric pressure.
 *     Otherwise, the `atmospheric_pressure_from_elevation` module enables the
 *     use of Equation 3.
 *
 *  2. Separate atmospheric transmittances for direct and diffuse radiation are
 *     used in place of the simplified transmittance defined in Equations 19 and
 *     47. Typically these are calculated by the
 *     `shortwave_atmospheric_scattering` module.
 *
 *  3. Equations 21 and 48 in ASCE (2005) calculate the total solar radiation
 *     incident on the Earth's upper atmosphere during periods of 24 and 1 hour,
 *     respectively. Ultimately, this is compared to the actual solar radiation
 *     incident at the Earth's surface, enabling an estimate of cloudiness.
 *     However, BioCro uses instantaneous measurements of incident light at the
 *     surface, so it is not appropriate to integrate over a time interval here.
 *     As explained in Duffie and Beckam (1980) (the source cited by ASCE 2005),
 *     the relevant instantaneous equation for `R_a` is Equation 1.10.1. When
 *     the solar zenith angle is negative, the sun is below the horizon, and
 *     hence `R_a` is zero.
 *
 *  The reference evapotranspiration rate (`ET_0`) depends on environmental
 *  conditions, and is the rate that would occur for the reference surface,
 *  which is described in Allen et al. (1998) as follows:
 *
 *  > The reference surface is a hypothetical grass reference crop with an
 *  > assumed crop height of 0.12 m, a fixed surface resistance of 70 s / m, and
 *  > an albedo of 0.23. The reference surface closely resembles an extensive
 *  > surface of green, well-watered grass of uniform height, actively growing
 *  > and completely shading the ground. The fixed surface resistance of
 *  > 70 s / m implies a moderately dry soil surface resulting from about a
 *  > weekly irrigation frequency.
 *
 *  References:
 *
 *  - [Allen, R. G., Pereira, L. S., Raes, D. & Smith, M. "FAO Irrigation and Drainage
 *    Paper No. 56." Food and Agriculture Organization of the United Nations, Rome, Italy (1998)]
 *    (http://www.climasouth.eu/sites/default/files/FAO%2056.pdf)
 *
 *  - ["Calculating Standardized Reference Crop Evapotranspiration" in "The ASCE
 *    Standardized Reference Evapotranspiration Equation" 7–45 (2005)]
 *    (https://doi.org/10.1061/9780784408056.ch04)
 *
 *  - [Duffie, J. A. & Beckman, W. A. Solar Engineering of Thermal Processes. (Wiley New York, 1980)]
 *    (http://les.edu.uy/FRS/duffie_beckman.pdf)
 */
double reference_evapotranspiration(
    int doy,
    double solar,  // micromol / m^2 / s
    double temp,
    double windspeed,
    double rh,
    double wet_soil_albedo,
    double const par_energy_content,                // J / micromol
    double const par_energy_fraction,               // dimensionless
    double const atmospheric_pressure,              // Pa
    double const irradiance_direct_transmittance,   // dimensionless
    double const irradiance_diffuse_transmittance,  // dimensionless
    double const cosine_zenith_angle                // dimensionless
)
{
    using calculation_constants::eps_zero;
    using math_constants::pi;
    using std::max;

    // Set constants
    double constexpr kPa_per_Pa = 1e-3;      // kPa / Pa
    double constexpr MJ_per_J = 1e-6;        // MJ / J
    double constexpr s_per_hr = 3600;        // s / hr
    double constexpr solar_constant = 4.92;  // MJ / m^2 / hr

    // PET.for, line 228
    double tavg = temp;  // Mean daily temperature (°C)

    // Total incident shortwave energy (including PAR and NIR bands)
    double const srad =
        solar * par_energy_content / par_energy_fraction * MJ_per_J * s_per_hr;  // MJ / m^2 / hr

    // Psychrometric constant, ASCE (2005) Eq. 4
    double psychrometric_const = 0.000665 * atmospheric_pressure * kPa_per_Pa;  // kPa/deg C

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

    // Account for Earth's ellipical orbit; Equation 50 from ASCE (2005)
    double const dr = 1.0 + 0.033 * cos(2.0 * pi / 365.0 * doy);  // dimensionless

    // Extraterrestrial radiation; Equation 1.10.1 from Duffie and Beckam (1980)
    double const ra =
        cosine_zenith_angle <= eps_zero ? 0.0
                                        : solar_constant * dr * cosine_zenith_angle;  // MJ / m^2 / hr

    // Clear sky solar radiation, modified from ASCE (2005) Equation 47
    double const rso = (irradiance_direct_transmittance + irradiance_diffuse_transmittance) * ra;  // MJ / m^2 / hr

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

/**
 *  @brief Calculates the potential rate of evaporation of water from the soil
 *  surface in the presence of a crop.
 *
 *  The potential rate of evaporation of water from the soil surface (`ES_0`) is
 *  the highest rate possible given the contraints placed by the available solar
 *  energy and the crop itself. The availability and distribution of water
 *  within the soil may reduce the actual rate to a lower value, but these
 *  calculations are handled elsewhere (typically by the `soil_evaporation2`
 *  module).
 *
 *  This function was originally based on the `PETASCE` and `PSE` subroutines of
 *  `PET.for`, and the `SPAM` subroutine of `SPAM.for`, all of which are parts
 *  of DSSAT (https://github.com/DSSAT/dssat-csm-os). Here we follow the "T"
 *  method, described as "Standardized Reference Evapotranspiration Equation for
 *  the tall reference crop (50-cm alfalfa) with dual FAO-56 crop coefficient
 *  method (potential E and T calculated independently)."
 *
 *  The "dual crop coefficient" approach to calculating evapotranspiration is
 *  described in Chapter 7 of FAO-56 (Allen et al. 1998). In this approach, the
 *  actual crop evapotranspiration rate (`ET_c`) is determined from the
 *  reference evapotranspiration rate (`ET_0`) according to
 *
 *  `ET_c = (K_cb + K_e) * ET_0`,
 *
 *  where `K_cb` is the "basal crop coefficient" and `K_e` is the "soil
 *  evaporation coefficient." The reference rate depends on environmental
 *  conditions, and is the rate that would occur for the reference surface; see
 *  the `reference_evapotranspiration()` function for more information.
 *
 *  The dimensionless coefficients `K_cb` and `K_e` account for differences
 *  between the reference surface and the actual crop, which generally has
 *  different characteristics. They each describe one component of the crop
 *  evapotranspiration rate, which can be described as the "transpiration" and
 *  "evaporation" components. Thus, the term "crop evapotranspiration" is better
 *  understood as "evapotranspiration from cropland," since it includes both
 *  crop transpiration and soil evaporation. Allen et al. (1998) describes the
 *  coefficients in more detail as follows:
 *
 *  > The basal crop coefficient (`K_cb`) is defined as the ratio of the crop
 *  > evapotranspiration over the reference evapotranspiration (`ET_c / ET_0`)
 *  > when the soil surface is dry but transpiration is occurring at a potential
 *  > rate, i.e., water is not limiting transpiration (Figure 22). Therefore,
 *  > `K_cb * ET_0` represents primarily the transpiration component of `ET_c`.
 *  > The `K_cb * ET_0` does include a residual diffusive evaporation component
 *  > supplied by soil water below the dry surface and by soil water from
 *  > beneath dense vegetation.
 *
 *  and:
 *
 *  > The soil evaporation coefficient, `K_e`, describes the evaporation
 *  > component of `ET_c`. Where the topsoil is wet, following rain or
 *  > irrigation, `K_e` is maximal. Where the soil surface is dry, `K_e` is
 *  > small and even zero when no water remains near the soil surface for
 *  > evaporation.
 *
 *  A key constraint on the values of `K_cb` and `K_e` is that the overall crop
 *  coefficient `K_c = K_cb + K_e` cannot exceed a maximum value, `K_cmax`,
 *  which is determined by the energy available for evapotranspiration.
 *  Expressing this as a limitation on `K_e`, we see that `K_e` must be less
 *  than or equal to `K_cmax - K_cb`, with equality only occurring when the soil
 *  is fully wet. This can be expressed as `K_e = K_r * (K_cmax - K_cb)`, where
 *  `K_r` is an evaporation reduction coefficient that depends on the soil
 *  surface water content.
 *
 *  Another consideration is that soil evaporation tends to decrease as crop
 *  cover increases, because some of the available solar energy is intercepted
 *  by the crop. Thus, `K_e` actually depends on properties of the crop itself,
 *  despite being a coefficient that describes evaporation from the soil. This
 *  is expressed as another constraint: `K_e` must be less than or equal to
 *  `f_ew * K_cmax`, where `f_ew` is the fraction of soil that is both exposed
 *  and wetted. A larger canopy leaf area index tends to decrease `f_ew`.
 *
 *  As described in DeJonge & Thorp (2017), to calculate the potential rate of
 *  evaporation of water from the soil surface, we assume `K_r` is 1. We also
 *  assume that the soil is evenly wetted; in this case, `f_w` is also 1, where
 *  `f_w` is the fraction of soil that is wet. These assumptions enable the
 *  calculation of `K_e`, and then the potential evaporation rate is given by
 *  `ES_0 = K_e * ET_0`.
 *
 *  References:
 *
 *  - [Allen, R. G., Pereira, L. S., Raes, D. & Smith, M. "FAO Irrigation and Drainage
 *    Paper No. 56." Food and Agriculture Organization of the United Nations, Rome, Italy (1998)]
 *    (http://www.climasouth.eu/sites/default/files/FAO%2056.pdf)
 *
 *  - [DeJonge, K. C. & Thorp, K. R. "Implementing standardized reference evapotranspiration and dual
 *    crop coefficient approach in the DSSAT cropping system model." Transactions of the ASABE 60, 1965–1981 (2017)]
 *    (https://doi.org/10.13031/trans.12321)
 *
 *  @param [in] SK_c A shaping parameter that determines the shape of the K_cb
 *              versus LAI curve; dimensionless
 *
 *  @param [in] K_cb_max The maximum basal crop coefficient; dimensionless
 *
 *  @param [in] K_cb_min The minimum basal crop coefficient; dimensionless
 *
 *  @param [in] LAI Total canopy leaf area index; dimensionless from
 *              (m^2 leaf) / (m^2 ground)
 *
 *  @param [in] height The canopy height; m
 *
 *  @param [in] ET_0 The reference evapotranspiration rate; any acceptable units
 *              such as kg / m^2 ground / hr, mol / m^2 / hr, or mm / hr
 *
 *  @return The potential soil evaporation rate `ES_0`; same units as `ET_0`
 */
double potential_soil_evaporation(
    double const SK_c,      // dimensionless
    double const K_cb_max,  // dimensionless
    double const K_cb_min,  // dimensionless
    double const LAI,       // dimensionless
    double const height,    // m
    double const ET_0       // any transpiration rate units such as mm / hr
)
{
    // Check for bad inputs
    if (LAI < 0) {
        throw std::range_error("Thrown in potential_soil_evaporation: LAI is negative.");
    }

    if (SK_c < 0) {
        throw std::range_error("Thrown in potential_soil_evaporation: SK_c is negative.");
    }

    if (K_cb_min < 0) {
        throw std::range_error("Thrown in potential_soil_evaporation: K_cb_min is negative.");
    }

    if (K_cb_max < K_cb_min) {
        throw std::range_error("Thrown in potential_soil_evaporation: K_cb_max is less than K_cb_min.");
    }

    // Set constants
    double constexpr K_r = 1.0;  // dimensionless
    double constexpr f_w = 1.0;  // dimensionless

    // Equation 6 from DeJonge & Thorp (2017)
    double const K_cb = K_cb_min + (K_cb_max - K_cb_min) * (1.0 - exp(-1.0 * SK_c * LAI));  // dimensionless

    // Equation A7 from DeJonge & Thorp (2017). Note: if K_cb is greater than 1,
    // then K_cmax will be equal to 1, and hence K_cb > K_cmax.
    double const K_cmax = std::max(1.0, K_cb + 0.05);  // dimensionless

    // Equation 76 from FAO-56, or Equation A9 from DeJonge & Thorp (2017).
    // Note: if K_cb > K_cmax, then f_c will be larger than 1, indicating full
    // coverage by the canopy.
    double const f_c =
        K_cb > K_cb_min ? pow(((K_cb - K_cb_min) / (K_cmax - K_cb_min)), (1.0 + 0.5 * height))
                        : 0;

    // Equation 75 from FAO-56, or Equation A8 from DeJonge & Thorp (2017).
    // Note: it is possible that f_c > 1; in this case, we should set f_ew to
    // its minimum value (0.0).
    double const f_ew = f_c > 1 ? 0.0 : std::min(1.0 - f_c, f_w);  // dimensionless

    // Equation 71 from FAO-56, or Equation A5 from DeJonge & Thorp (2017).
    // Note: it is possible that K_cb > K_cmax; in this case, we should set
    // K_e_constraint_1 to its minimum possible value (0.0).
    double const K_e_constraint_1 = K_cb > K_cmax ? 0.0 : K_r * (K_cmax - K_cb);  // dimensionless
    double const K_e_constraint_2 = K_cmax * f_ew;                                // dimensionless
    double const K_e = std::min(K_e_constraint_1, K_e_constraint_2);              // dimensionless

    return K_e * ET_0;  // same units as ET_0
}

// Calculate stage 1 soil evaporation
//   If the sum for stage 1 soil evaporation (SUMES1) is larger than
//   stage 1 evaporation limit (evap_limit), start stage 2 soil evaporation (SUMES2)
//   and adjust soil evaporation (actual_soil_evap)
struct evap_str {
    double sumes1;
    double sumes2;
    double days_stage2;
    double actual_soil_evap;
};

evap_str supplemetal_evap_computation(
    double potential_soil_evap,
    double sumes1_temp,
    double sumes2_temp,
    double evap_limit,
    double soil_evaporation_alpha,
    double days_stage2_temp)
{
    evap_str return_value;
    double actual_soil_evap = 0.0;
    sumes1_temp += potential_soil_evap;
    if (sumes1_temp > evap_limit) {
        actual_soil_evap = potential_soil_evap - 0.4 * (sumes1_temp - evap_limit);
        sumes2_temp = 0.6 * (sumes1_temp - evap_limit);
        days_stage2_temp = pow((sumes2_temp / soil_evaporation_alpha), 2);
        sumes1_temp = evap_limit;
    } else
        actual_soil_evap = potential_soil_evap;

    return_value.sumes1 = sumes1_temp;
    return_value.sumes2 = sumes2_temp;
    return_value.days_stage2 = days_stage2_temp;
    return_value.actual_soil_evap = actual_soil_evap;
    return return_value;
}
#endif
