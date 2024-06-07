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
 *  @param [in] air_temperature The air temperature in degrees C
 *
 *  @param [in] delta_t The temperature difference between the leaf and air in
 *              degrees C
 *
 *  @param [in] ea The partial pressure of water vapor in the atmosphere in Pa
 *
 *  @param [in] gsv The stomatal conductance in m / s
 *
 *  @param [in] lw The characteristic leaf dimension in m
 *
 *  @param [in] windspeed The wind speed just outside the leaf boundary layer in
 *              m / s
 *
 *  @param [in] p The atmospheric pressure in Pa
 *
 *  @return The leaf boundary layer conductance in m / s
 */
double leaf_boundary_layer_conductance_nikolov(
    double air_temperature,  // degrees C
    double delta_t,          // degrees C
    double ea,               // Pa
    double gsv,              // m / s
    double lw,               // m
    double windspeed,        // m / s
    double p                 // Pa
)
{
    double const leaftemp = air_temperature + delta_t;                             // degrees C
    double const Tak = air_temperature + conversion_constants::celsius_to_kelvin;  // K
    double const Tlk = leaftemp + conversion_constants::celsius_to_kelvin;         // K

    double const esTl = saturation_vapor_pressure(leaftemp);  // Pa

    // Set constants
    double constexpr cf = 4.322e-3;   // for broad leaves
    double constexpr ce = 1.6361e-3;  // for broad leaves
    double constexpr ct = 0.378;

    // Equation 29
    double const gbv_forced = cf * pow(Tak, 0.56) *
                              sqrt((Tak + 120) * windspeed / (lw * p));  // m / s

    // The equations for free convection must be solved iteratively. First make
    // a starting guess for gbv_free.
    double gbv_free{gbv_forced};  // m / s

    // Initialize other loop variables; their values will be set during the loop
    double eb{};      // Pa
    double Tvdiff{};  // K

    // Run loop to find gbv_free. See code on page 229.
    double old_gbv_free{};   // m / s
    double change_in_gbv{};  // m / s
    int counter{0};

    do {
        // Store gbv_free from previous loop iteration
        old_gbv_free = gbv_free;  // m / s

        // Equation 35
        eb = (gsv * esTl + gbv_free * ea) / (gsv + gbv_free);  // Pa

        // Equation 34
        Tvdiff = (Tlk / (1.0 - ct * eb / p)) - (Tak / (1.0 - ct * ea / p));  // K

        // Equation 33
        gbv_free = ce * pow(Tlk, 0.56) * sqrt((Tlk + 120) / p) *
                   pow(std::abs(Tvdiff) / lw, 0.25);  // m / s

        // Get the change in gbv_free relative to the previous iteration
        change_in_gbv = std::abs(gbv_free - old_gbv_free);  // m / s

    } while ((++counter <= 12) && (change_in_gbv > 0.01));

    // The overall conductance is the larger one
    return std::max(gbv_forced, gbv_free);  // m / s
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
 *              boundary layer
 *
 *  @param [in] minimum_gbw The lowest possible value for boundary layer
 *              conductance in m / s that should be returned
 *
 *  @param [in] WindSpeedHeight The height in m at which the wind speed was
 *              measured
 *
 *  @return The canopy boundary layer conductance in m / s
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
