#include "lightME.h"
#include "../framework/constants.h"  // for atmospheric_pressure_at_sea_level

using physical_constants::atmospheric_pressure_at_sea_level;

/**
 * @brief Calculates the "light macro environment"; in other words, the amount
 * of sunlight scattered out of the direct beam by the atmosphere.
 *
 * The result is expressed in two ways:

 * 1. As atmospheric transmittances, which are the ratios of direct light or
 *    diffuse light at the Earth's surface to incident light at the upper
 *    atmosphere. These would be appropriate for calculating the intensity of
 *    direct and diffuse light at the surface given a value for the solar
 *    constant. Note that these transmittances are different than the input
 *    parameter `atmospheric_transmittance`, which represents the transmittance
 *    of either type of light through a small volume of atmosphere.
 *
 * 2. As fractions of direct and diffuse light at the Earth's surface, which
 *    would be appropriate for calculating the intensity of direct and
 *    diffuse light given an experimental measurement of light intensity at the
 *    surface.
 *
 * The basis for this function is given in chapter 11 of Norman and Campbell,
 *  _An Introduction to Environmental Biophysics_, 2nd edition. The book states
 *  that this model is technically only applicable for solar zenith angles less
 *  than 80 degrees (which approximately corresponds to `cosine_zenith_angle`
 *  greater than 0.173). When the Sun is closer to the horizon (or even below
 *  it), atmospheric refraction plays a large role in determining the relative
 *  amounts of direct and diffuse light at the Earth's surface. The effect of
 *  refraction depends strongly on dust, air temperature, and clouds, making it
 *  difficult or impossible to accurately model. Here, we choose to use the
 *  model outside this range, extending it to `cosine_zenith_angle = 0`. For
 *  even smaller values, we simply use the limits as `cosine_zenith_angle`
 *  approaches zero. The resulting inaccuracies are expected to play a small
 *  role in a biological simulation, because light intensities at the surface
 *  tend to be low when the Sun is near the horizon.
 *
 * @param [in] cosine_zenith_angle The cosine of the solar zenith angle; when
 *             the Sun is directly overhead, the angle is 0 and its cosine is 1;
 *             when the Sun's center is at the horizon, the angle is 90 and its
 *             cosine is 0.
 *
 * @param [in] atmospheric_pressure Local atmospheric pressure in Pa
 *
 * @param [in] atmospheric_transmittance Fraction of light transmitted through a
               small volume of atmosphere (dimensionless)
 *
 * @param [in] atmospheric_scattering Atmospheric scattering factor
 *             (dimensionless)
 *
 * @return A structure containing values of the transmittances and fractions of
 *         direct and diffuse light just above the canopy.
 */
Light_model lightME(
    double cosine_zenith_angle,        // dimensionless
    double atmospheric_pressure,       // Pa
    double atmospheric_transmittance,  // dimensionless
    double atmospheric_scattering      // dimensionless
)
{
    // Dimensionless quantity used in later calculations.
    double const pressure_ratio =
        atmospheric_pressure / atmospheric_pressure_at_sea_level;

    // Equation 11.1 from Campbell & Norman, solving for
    // direct_transmittance = S_p / S_p0 (dimensionless).
    // If the sun is near the horizon, take the limit as cosine_zenith_angle
    // approaches 0 (which is 0).
    double const direct_transmittance =
        cosine_zenith_angle <= 0 ? 0
                                 : pow(atmospheric_transmittance,
                                       (pressure_ratio / cosine_zenith_angle));

    // Equation 11.13 from Campbell & Norman, solving for
    // diffuse_transmittance = S_p / S_p0 (dimensionless).
    // If the sun is near the horizon, take the limit as cosine_zenith_angle
    // approaches 0 (which is 0).
    double const diffuse_transmittance =
        cosine_zenith_angle <= 0 ? 0
                                 : atmospheric_scattering *
                                       (1 - direct_transmittance) *
                                       cosine_zenith_angle;

    // The fraction of direct irradiance just above the canopy is the ratio of
    // the direct transmittance to the total transmittance (dimensionless).
    // If the sun is near the horizon, take the limit as cosine_zenith_angle
    // approaches 0 (which is 0).
    double const direct_fraction =
        cosine_zenith_angle <= 0 ? 0
                                 : direct_transmittance /
                                       (direct_transmittance + diffuse_transmittance);

    // The remaining irradiance is diffuse (dimensionless).
    double const diffuse_fraction = 1.0 - direct_fraction;

    return Light_model{
        /* .direct_transmittance = */ direct_transmittance,    // dimensionless
        /* .diffuse_transmittance = */ diffuse_transmittance,  // dimensionless
        /* .direct_fraction = */ direct_fraction,              // dimensionless
        /* .diffuse_fraction = */ diffuse_fraction             // dimensionless
    };
}
