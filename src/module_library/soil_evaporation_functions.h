#ifndef SOIL_EVAPORATION_FUNCTIONS_H
#define SOIL_EVAPORATION_FUNCTIONS_H

#include <algorithm>                   // for std::min, std::max
#include <cmath>                       // for pow, cos
#include <stdexcept>                   // for std::range_error
#include "../framework/constants.h"    // for eps_zero, pi, stefan_boltzmann
#include "water_and_air_properties.h"  // for saturation_vapor_pressure

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
 *  The presence of mulch also alters the surface albedo, but it is not yet
 *  considered by this function.
 *
 *  Note: This function was originally based on the `ALBEDO_avg` subroutine of
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
 *              layer; m^3 / m^3.
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
    double constexpr albedo_frac = 0.55;    // dimensionless
    double constexpr canopy_albedo = 0.23;  // dimensionless
    double constexpr k_canopy = 0.75;       // dimensionless
    double constexpr mulch_albedo = 0.45;   // dimensionless (from MULCHLAYER subroutine)
    double constexpr mulch_cover = 0.0;     // dimensionless (disable mulch for now)
    double constexpr theta_min = 0.03;      // dimensionless

    // Upper threshold for water content (dimensionless)
    double const theta_max = theta_min + 2.0 * (theta_fc_surface - theta_min);

    // Slope of albedo vs. water content in linear range (dimensionless)
    double const slope =
        bare_soil_albedo_max * (albedo_frac - 1.0) / (theta_fc_surface - theta_min);

    // Minimum soil albedo (dimensionless)
    double const bare_soil_albedo_min =
        bare_soil_albedo_max + slope * (theta_max - theta_min);

    // Bare soil albedo, accounting for water content (dimensionless); in DSSAT,
    // this is called SWALB (the wet soil albedo)
    double const bare_soil_albedo =
        theta_surface < theta_min   ? bare_soil_albedo_max
        : theta_surface < theta_max ? bare_soil_albedo_max + slope * (theta_surface - theta_min)
                                    : bare_soil_albedo_min;

    // Albedo accounting for soil and mulch cover (dimensionless); in DSSAT,
    // this is called MSALB (mulch/soil abedo)
    double const mulch_soil_albedo =
        mulch_cover * mulch_albedo + (1.0 - mulch_cover) * bare_soil_albedo;

    // Fraction of light transmitted through canopy (dimensionless)
    double const canopy_transmittance = exp(-k_canopy * LAI);

    // Effective surface albedo including bare soil, mulch, and canopy
    // (dimensionless); in DSSAT, this is called CMSALB (the canopy/mulch/soil
    // albedo)
    return mulch_soil_albedo * canopy_transmittance +
           canopy_albedo * (1 - canopy_transmittance);
}

/**
 *  @brief Calculates the reference evapotranspiration rate from environmental
 *  conditions.
 *
 *  Inspired by DSSAT, here we use the method described in ASCE (2005) to
 *  calculate the reference evapotranspiration rate. ASCE (2005) defines the
 *  rate as follows:
 *
 *  > Reference evapotranspiration (`ET_ref`) is the rate at which readily
 *  > available soil water is vaporized from specified vegetated surfaces. For
 *  > convenience and reproducibility, the reference surface has recently been
 *  > expressed as a hypothetical crop (vegetative) surface with specific
 *  > characteristics. In the context of this standardization report, reference
 *  > evapotranspiration is defined as the ET rate from a uniform surface of
 *  > dense, actively growing vegetation having specified height and surface
 *  > resistance, not short of soil water, and representing an expanse of at
 *  > least 100 m of the same or similar vegetation.
 *
 *  The reference rate is typically modified by crop and soil coefficients to
 *  estimate the real evapotranspiration rate; see potential_soil_evaporation()`
 *  for more details. Although Equation 1 in ASCE (2005) is slightly different
 *  than the reference evapotranspiration calculations in FAO-56 (Allen et al.
 *  1998), it is nevertheless compatible with crop coefficients calculated as in
 *  FAO-56, according to Appendix B of ASCE (2005).
 *
 *  Slightly different equations and coefficients must be used depending on the
 *  time step (hourly, daily, or monthly) and the reference type (short or
 *  tall). While DSSAT takes daily steps and allows users to specify short or
 *  tall crops, here we take hourly steps and assume a tall reference crop.
 *
 *  Additionally, our approach deviates from ASCE (2005) in a few ways:
 *
 *  1. The atmospheric pressure is an input, so it is not necessarily calculated
 *     using Equation 34 from ASCE (2005). This is to accomodate weather data
 *     sets that include measured values of local atmospheric pressure.
 *     Otherwise, the `atmospheric_pressure_from_elevation` module enables the
 *     use of Equation 34.
 *
 *  2. Equations 21 and 48 in ASCE (2005) calculate the total solar radiation
 *     incident on the Earth's upper atmosphere during periods of 24 and 1 hour,
 *     respectively. Ultimately, this is compared to the actual solar radiation
 *     incident at the Earth's surface, enabling an estimate of cloudiness.
 *     However, BioCro uses instantaneous measurements of incident light at the
 *     surface, so it is not appropriate to integrate over a time interval here.
 *     As explained in Duffie and Beckam (1980) (the source cited by ASCE 2005),
 *     the relevant instantaneous equation for `R_a` is Equation 1.10.1. Using
 *     ASCE notation, this would be
 *     `R_a = solar_constant * dr * cosine_zenith_angle`. One extra
 *     consideration applies: when the solar zenith angle is negative, the sun
 *     is below the horizon, and hence `R_a` is zero.
 *
 *  3. Equation 47 from ASCE (2005) uses an atmospheric transmittance to
 *     calculate the incident light at the Earth's surface: `R_s = R_a * trans`,
 *     where `trans = 0.75 + 2e-5 * elevation`. Combining this with the equation
 *     discussed in (2) above produces
 *     `R_s = solar_constant * dr * trans * cosine_zenith_angle`. Here we extend
 *     this simple approach by using separate atmospheric transmittances for
 *     direct and diffuse radiation. Typically these are calculated by the
 *     `shortwave_atmospheric_scattering` module. In this approach, the direct
 *     beam radiation on a ground area basis at the Earth's surface is given by
 *     `solar_constant * dr * direct_transmittance * cosine_zenith_angle`, while
 *     the diffuse component is given by `solar_constant * diffuse_transmittance`.
 *     Thus, the overall transmittance (as applied to `solar_constant * dr`) is
 *     effectively
 *     `diffuse_transmittance + direct_transmittance * cosine_zenith_angle`.
 *
 *  4. We use the Arden-Buck equation to calculate the saturation water vapor
 *     pressure instead of Equation 37 from ASCE (2005); see
 *     `saturation_vapor_pressure()` for more information.
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
 *
 *
 *  @param [in] atmospheric_pressure The local atmospheric pressure; Pa
 *
 *  @param [in] cosine_zenith_angle The cosine of the solar zenith angle;
 *              dimensionless
 *
 *  @param [in] doy The day of the year, which can be fractional
 *
 *  @param [in] irradiance_diffuse_transmittance The atmospheric transmittance
 *              for direct sunlight - in other words, the ratio of direct beam
 *              light at the Earth's surface to the light incident on the upper
 *              atmosphere; dimensionless
 *
 *  @param [in] irradiance_direct_transmittance The atmospheric transmittance
 *              for diffuse sunlight - in other words, the ratio of diffuse
 *              light at the Earth's surface to the light incident on the upper
 *              atmosphere; dimensionless
 *
 *  @param [in] par_energy_content The energy of each mole of photons in the PAR
 *              band; J / micromol
 *
 *  @param [in] par_energy_fraction The fraction of total shortwave energy in
 *              the PAR band, where the remaining energy is assumed to lie in
 *              the NIR band; dimensionless
 *
 *  @param [in] rh The relative humidty expressed as a number between 0 and 1;
 *              dimensionless
 *
 *  @param [in] solar The incident photosynthetically active flux density on a
 *              ground area basis; micromol / m^2 / s
 *
 *  @param [in] temp The air temperature; degrees C
 *
 *  @param [in] windspeed The wind speed; m / s
 *
 *  @param [in] windspeed_height The height at which the wind speed was
 *              measured; m
 *
 *  @return The reference evapotranspiration rate; mm / hr
 */
double reference_evapotranspiration(
    double const atmospheric_pressure,              // Pa
    double const cosine_zenith_angle,               // dimensionless
    double const doy,                               // day
    double const irradiance_diffuse_transmittance,  // dimensionless
    double const irradiance_direct_transmittance,   // dimensionless
    double const par_energy_content,                // J / micromol
    double const par_energy_fraction,               // dimensionless
    double const rh,                                // dimensionless
    double const solar,                             // micromol / m^2 / s
    double const temp,                              // degrees C
    double const windspeed,                         // m / s
    double const windspeed_height                   // m
)
{
    using calculation_constants::eps_zero;
    using conversion_constants::celsius_to_kelvin;
    using math_constants::pi;
    using physical_constants::stefan_boltzmann;  // W / m^2 / K^4

    // Set constants
    double constexpr et_coef = 0.408;          // m^2 mm / MJ
    double constexpr kPa_per_Pa = 1e-3;        // kPa / Pa
    double constexpr MJ_per_J = 1e-6;          // MJ / J
    double constexpr reference_albedo = 0.23;  // dimensionless
    double constexpr s_per_hr = 3600;          // s / hr
    double constexpr solar_constant = 4.92;    // MJ / m^2 / hr

    // Air temperature in Kelvin
    double const tk = temp + celsius_to_kelvin;  // K

    // Total incident shortwave energy (including PAR and NIR bands)
    double const srad =
        solar * par_energy_content / par_energy_fraction * MJ_per_J * s_per_hr;  // MJ / m^2 / hr

    // Psychrometric constant; Equation 35 from ASCE (2005)
    double const psychrometric_const =
        0.000665 * atmospheric_pressure * kPa_per_Pa;  // kPa / degrees C

    // Slope of the saturation vapor pressure-temperature curve; Equation 36
    // from ASCE (2005)
    double const udelta =
        2503.0 *
        pow(exp(17.27 * temp / (temp + 237.3)) / (temp + 237.3), 2.0);  // kPa / degrees C

    // Actual water vapor pressure; Equation 41 from ASCE (2005)
    double const sat_vap_pressure = saturation_vapor_pressure(temp) * kPa_per_Pa;  // kPa

    double const ea = sat_vap_pressure * rh;  // kPa

    // Net shortwave radiation; Equation 43 from ASCE (2005)
    double const rns = (1.0 - reference_albedo) * srad;  // MJ / m^2 / hr

    // Distance factor that accounts for the elliptical shape of the Earth's
    // orbit around the sun; Equation 50 from ASCE (2005)
    double const dr = 1.0 + 0.033 * cos(2.0 * pi / 365.0 * doy);  // dimensionless

    // Direct beam irradiance incident on the Earth's upper atmosphere
    double const r_extraterrestrial =
        cosine_zenith_angle <= eps_zero ? 0.0 : solar_constant * dr;  // MJ / (m^2 beam) / hr

    // Effective atmospheric transmittance including direct and diffuse
    // radiation
    double const trans = irradiance_direct_transmittance * cosine_zenith_angle +
                         irradiance_diffuse_transmittance;  // dimensionless

    // Clear-sky irradiance at the Earth's surface expressed on a ground area
    // basis
    double const rso = trans * r_extraterrestrial;  // MJ / m^2 / hr

    // Net longwave radiation; Equations 44 and 45 from ASCE (2005)
    double const cloudiness_ratio =
        rso <= eps_zero ? 1.0 : std::max(0.3, std::min(1.0, srad / rso));  // dimensionless

    double const fcd = 1.35 * cloudiness_ratio - 0.35;  // dimensionless

    double const net_emissivity = fcd * (0.34 - 0.14 * pow(ea, 0.5));  // dimensionless

    double const rnl =
        stefan_boltzmann * s_per_hr * MJ_per_J * net_emissivity * pow(tk, 4.0);  // MJ / m^2 / s

    // Net radiation; Equation 42 from ASCE (2005)
    double const rn = rns - rnl;  // MJ / m^2 / hr

    // Soil heat flux; Equation 66 from ASCE (2005)
    double const g = rn >= 0 ? 0.04 * rn : 0.2 * rn;  // MJ / m^2 / hr

    // Estimate the wind speed 2m above the ground; Equation 67 from ASCE (2005)
    double const wind2m =
        windspeed * (4.87 / log(67.8 * windspeed_height - 5.42));  // m / s

    // Aerodynamic roughness and surface resistance; hourly values for tall
    // reference from Table 1 of ASCE (2005)
    double constexpr Cn = 66.0;                 // K mm s^3 / Mg / hr
    double const Cd = rn >= 0.0 ? 0.25 : 0.17;  // m / s

    // Standardized reference evapotranspiration; Equation 1 from ASCE (2005)
    double const pm_top =
        et_coef * udelta * (rn - g) +
        psychrometric_const * (Cn / tk) * wind2m * (sat_vap_pressure - ea);  // mm * kPa / degrees C / hr

    double const pm_bottom = udelta + psychrometric_const * (1.0 + Cd * wind2m);  // kPa / degrees C

    return pm_top / pm_bottom;  // mm / hr
}

/**
 *  @brief Calculates the potential rate of evaporation of water from the soil
 *  surface in the presence of a crop.
 *
 *  The potential rate of evaporation of water from the soil surface (`ES_0`) is
 *  the highest rate possible given the contraints placed by the available solar
 *  energy and the crop itself. The availability and distribution of water
 *  within the soil may reduce the actual rate to a lower value, but these
 *  calculations are handled elsewhere (typically by the
 *  `soil_evaporation_ritchie` module).
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

/**
 *  @brief A structure for holding the outputs from
 *  `ritchie_s1_to_s2()`
 */
struct evap_str {
    double days_stage2_next;
    double ES;
    double sumes1_next;
    double sumes2_next;
};

/**
 *  @brief Calculates key soil water variables when there is a (potential)
 *  transition from Stage 1 to Stage 2 evaporation.
 *
 *  This function is based on the `ESUP` subroutine from DSSAT (found in the
 *  file `SOILEV.for`), which implements part of the model described in
 *  Ritchie (1972). It is called by the `soil_evaporation_ritchie` module.
 *
 *  These calculations are described in the paper as follows:
 *
 *  > The equation given in the flow diagram (Figure 1) at B is used to predict
 *  > `ES` on the day when the cumulative evaporation is in transition between
 *  > Stage 1 drying and Stage 2 drying. On this day, `ES = EOS` until
 *  > `sumes1 = evap_limit`; for the rest of the day, `ES = 0.6 * EOS`.
 *
 *  References:
 *
 *  - [Ritchie, J. T. "Model for predicting evaporation from a row crop with incomplete cover."
 *    Water Resources Research 8, 1204–1213 (1972)]
 *    (https://doi.org/10.1029/WR008i005p01204)
 *
 *  - DSSAT Fortran source code:
 *    github.com/DSSAT/dssat-csm-os/blob/develop/SPAM/SOILEV.for
 */
evap_str ritchie_s1_to_s2(
    double const days_stage2,             // day
    double const EOS,                     // mm / hr
    double const evap_limit,              // mm
    double const soil_evaporation_alpha,  // mm / day^(0.5)
    double const sumes1,                  // mm
    double const sumes2,                  // mm
    double const timestep                 // hr
)
{
    // Determine the cumulative evaporation that would occur over the next time
    // step if evaporation proceeds at its potential rate
    double const potential_new_evap = EOS * timestep;  // mm

    // Determine the amount of excess Stage 1 evaporation that would result if
    // the potential evaporation were to occur
    double const excess_s1_evap = sumes1 + potential_new_evap - evap_limit;  // mm

    // Determine the values key parameters depending on whether we are
    // transitioning to Stage 2
    if (excess_s1_evap > 0) {
        // The cumulative evaporation during Stage 1 will exceed the limit, so
        // we are transitioning to Stage 2
        double constexpr evap_frac = 0.6;                       // dimensionless
        double const sumes2_next = evap_frac * excess_s1_evap;  // mm

        return evap_str{
            /* .days_stage2_next = */ pow((sumes2_next / soil_evaporation_alpha), 2),  // day
            /* .ES = */ EOS - (1.0 - evap_frac) * excess_s1_evap / timestep,           // mm / hr
            /* .sumes1_next = */ evap_limit,                                           // mm
            /* .sumes2_next = */ sumes2_next                                           // mm
        };
    } else {
        // We are still in Stage 1 so there is no transition to Stage 2
        return evap_str{
            /* .days_stage2_next = */ days_stage2,             // day
            /* .ES = */ EOS,                                   // mm / hr
            /* .sumes1_next = */ sumes1 + potential_new_evap,  // mm
            /* .sumes2_next = */ sumes2                        // mm
        };
    }
}
#endif
