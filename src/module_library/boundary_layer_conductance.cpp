#include <cmath>                       // for std::max, std::min, pow, log
#include "../framework/constants.h"    // for celsius_to_kelvin
#include "water_and_air_properties.h"  // for saturation_vapor_pressure
#include "boundary_layer_conductance.h"

/**
 *  @brief Calculates the conductance for water vapor flow from the leaf across
 *  its boundary layer using a model described in Nikolov, Massman, and
 *  Schoettle (1995).
 *
 *  Note that for an isolated leaf, this conductance characterizes the entire
 *  path from the leaf surface to the ambient air. For a leaf within a canopy,
 *  there is an additional boundary layer separating the canopy from the
 *  atmosphere; this canopy boundary layer conductance must be calculated using
 *  a separate model.
 *
 *  In this model, two types of gas flow are considered: "forced" flow driven
 *  by wind-created eddy currents and "free" flow driven by temperature-related
 *  buoyancy effects. The overall conductance is determined to be the larger of
 *  the free and forced conductances.
 *
 *  In this function, we use equations 29, 33, 34, and 35 to calculate boundary
 *  layer conductance. This is the same approach taken in the `MLcan` model of
 *  Drewry et al. (2010).
 *
 *  In this model, the minimum possible boundary layer conductance that could
 *  occur is zero. This would happen if wind speed is zero and the air and leaf
 *  temperatures are the same. In realistic field conditions, boundary layer
 *  conductance can never truly be zero. To accomodate this, an option is
 *  provided for setting a minimum value for the boundary layer counductance.
 *
 *  References:
 *
 *  - [Nikolov, N. T., Massman, W. J. & Schoettle, A. W. "Coupling biochemical and biophysical processes at the
 *    leaf level: an equilibrium photosynthesis model for leaves of C3 plants" Ecological Modelling 80, 205–235 (1995)]
 *    (https://doi.org/10.1016/0304-3800(94)00072-P)
 *
 *  - [Drewry, D. T. et al. "Ecohydrological responses of dense canopies to environmental variability: 1. Interplay between
 *    vertical structure and photosynthetic pathway" Journal of Geophysical Research: Biogeosciences 115, (2010)]
 *    (https://doi.org/10.1029/2010JG001340)
 *
 *  @param [in] windspeed The wind speed in m / s
 *
 *  @param [in] leafwidth The characteristic leaf dimension in m
 *
 *  @param [in] air_temperature The air temperature in degrees C
 *
 *  @param [in] delta_t The temperature difference between the leaf and air in
 *              degrees C
 *
 *  @param [in] stomcond The stomatal conductance in m / s
 *
 *  @param [in] water_vapor_pressure The partial pressure of water vapor in the
 *              atmosphere in Pa
 *
 *  @param [in] minimum_gbw The lowest possible value for boundary layer
 *              conductance in m / s that should be returned
 *
 *  @return The boundary layer conductance in m / s
 */
double leaf_boundary_layer_conductance_nikolov(
    double windspeed,             // m / s
    double leafwidth,             // m
    double air_temperature,       // degrees C
    double delta_t,               // degrees C
    double stomcond,              // m / s
    double water_vapor_pressure,  // Pa
    double minimum_gbw            // m / s
)
{
    constexpr double p = physical_constants::atmospheric_pressure_at_sea_level;  // Pa

    double leaftemp = air_temperature + delta_t;                             // degrees C
    double gsv = stomcond;                                                   // m / s
    double Tak = air_temperature + conversion_constants::celsius_to_kelvin;  // K
    double Tlk = leaftemp + conversion_constants::celsius_to_kelvin;         // K
    double ea = water_vapor_pressure;                                        // Pa
    double lw = leafwidth;                                                   // m

    double esTl = saturation_vapor_pressure(leaftemp);  // Pa.

    // Forced convection
    constexpr double cf = 1.6361e-3;  // TODO: Nikolov et. al equation 29 use cf = 4.322e-3, not cf = 1.6e-3 as is used here.

    double gbv_forced = cf * pow(Tak, 0.56) * pow((Tak + 120) * ((windspeed / lw) / p), 0.5);  // m / s.

    // Free convection
    double gbv_free = gbv_forced;
    double eb = (gsv * esTl + gbv_free * ea) / (gsv + gbv_free);  // Pa. Eq 35

    double Tvdiff = (Tlk / (1 - 0.378 * eb / p)) - (Tak / (1 - 0.378 * ea / p));  // kelvin. It is also degrees C since it is a temperature difference. Eq. 34

    if (Tvdiff < 0) Tvdiff = -Tvdiff;

    gbv_free = cf * pow(Tlk, 0.56) * pow((Tlk + 120) / p, 0.5) * pow(Tvdiff / lw, 0.25);  // m / s. Eq. 33

    // Overall conductance
    double gbv = std::max(gbv_forced, gbv_free);  // m / s

    // Apply the minimum
    return std::max(gbv, minimum_gbw);  // m / s
}

/**
 *  @brief Calculates the conductance for water vapor flow from the canopy
 *  across its boundary layer using a model described in Thornley and Johnson
 *  (1990).
 *
 *  Note that when gas exchange is considered at the canopy level, this
 *  conductance characterizes the entire path to the ambient air. For a leaf
 *  within a canopy, there is an additional boundary layer separating the leaf
 *  from the canopy boundary layer; this leaf boundary layer conductance must be
 *  calculated using a separate model.
 *
 *  This model considers gas flow due to wind-driven eddy currents. Here, the
 *  conductance is calculated using Equation 14.9 from pages 414 - 416 of the
 *  Thornley textbook. Unfortunately, an electronic version of this reference is
 *  not available.
 *
 *  In this model, the minimum possible boundary layer conductance that could
 *  occur is zero, which would correspond to zero wind speed or canopy height.
 *  In realistic field conditions, boundary layer conductance can never truly be
 *  zero because of other transport processes such as buoyancy effects. To
 *  accomodate this, an option is provided for setting a minimum value for the
 *  boundary layer counductance.
 *
 *  This model contains two singularities, which occur when either of the
 *  following conditions are met:
 *
 *  - `WindSpeedHeight + Zeta - d = 0`, which is equivalent to `CanopyHeight =
 *     WindSpeedHeight / (dCoef - ZetaCoef) = WindSpeedHeight * 1.34`
 *
 *  - `WindSpeedHeight + ZetaM - d = 0`, which is equivalent to `CanopyHeight =
 *     WindSpeedHeight / (dCoef - ZetaMCoef) = WindSpeedHeight * 1.56`
 *
 *  So, as the canopy height approaches or exceeds the height at which wind
 *  speed was measured, the calculated boundary layer conductance becomes
 *  unbounded. For even larger canopy heights, the conductance eventually begins
 *  to decrease. For tall crops, this is a severe limitation to this model. Here
 *  we address this issue by limiting the canopy height to
 *  `0.98 * WindSpeedHeight`.
 *
 *  References:
 *
 *  - Thornley, J. H. M. & Johnson, I. R. "Plant and Crop Modelling: A
 *    Mathematical Approach to Plant and Crop Physiology" (1990)
 *
 *  @param [in] CanopyHeight The height of the canopy above the ground in m
 *
 *  @param [in] WindSpeed The wind speed in m / s as measured above the canopy
 *
 *  @param [in] minimum_gbw The lowest possible value for boundary layer
 *              conductance in m / s that should be returned
 *
 *  @param [in] WindSpeedHeight The height in m at which the wind speed was
 *              measured
 *
 *  @return The boundary layer conductance in m / s
 */
double canopy_boundary_layer_conductance_thornley(
    double CanopyHeight,    // m
    double WindSpeed,       // m / s
    double minimum_gbw,     // m / s
    double WindSpeedHeight  // m
)
{
    // Define constants used in the model. `kappa` is von Karmon's constant.
    // In the original text, the value of `dCoef` is reported as 0.64. In the
    // 2000 reprinting of the text, the authors state that this value should be
    // 0.77. See "Errata to the 2000 printing" on the page after the preface of
    // the 2000 reprinting of the 1990 text.
    constexpr double kappa = 0.41;      // dimensionless, Thornley and Johnson pgs 414 and 416.
    constexpr double ZetaCoef = 0.026;  // dimensionless, Thornley and Johnson 1990, Eq. 14.9o
    constexpr double ZetaMCoef = 0.13;  // dimensionless, Thornley and Johnson 1990, Eq. 14.9o
    constexpr double dCoef = 0.77;      // dimensionless, Thornley and Johnson 1990, Eq. 14.9o

    // Apply the height limit
    CanopyHeight = std::min(CanopyHeight, 0.98 * WindSpeedHeight);  // meters

    // Calculate terms that depend on the canopy height (Eq. 14.9o)
    const double Zeta = ZetaCoef * CanopyHeight;    // meters
    const double Zetam = ZetaMCoef * CanopyHeight;  // meters
    const double d = dCoef * CanopyHeight;          // meters

    // Calculate the boundary layer conductance `ga` according to Thornley and
    // Johnson Eq. 14.9n, pg. 416
    const double ga0 = pow(kappa, 2) * WindSpeed;                   // m / s
    const double ga1 = log((WindSpeedHeight + Zeta - d) / Zeta);    // dimensionless
    const double ga2 = log((WindSpeedHeight + Zetam - d) / Zetam);  // dimensionless
    const double gbv = ga0 / (ga1 * ga2);                           // m / s

    // Apply the minimum
    return std::max(gbv, minimum_gbw);  // m / s
}
