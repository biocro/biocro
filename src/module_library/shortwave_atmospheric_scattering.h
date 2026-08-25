#ifndef SHORTWAVE_ATMOSPHERIC_SCATTERING_H
#define SHORTWAVE_ATMOSPHERIC_SCATTERING_H

#include "../framework/module.h"
#include "../framework/constants.h"  // for atmospheric_pressure_at_sea_level
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class shortwave_atmospheric_scattering
 *
 *  @brief Calculates the amount of sunlight scattered out of the direct beam by
 *  the atmosphere.
 *
 *  The result is expressed in two ways:
 *
 *  1. As atmospheric transmittances, which are the ratios of direct light or
 *     diffuse light at the Earth's surface to incident light at the upper
 *     atmosphere. These would be appropriate for calculating the intensity of
 *     direct and diffuse light at the surface given a value for the solar
 *     constant. Note that these transmittances are different than the input
 *     parameter `atmospheric_transmittance`, which represents the transmittance
 *     of either type of light through a small volume of atmosphere.
 *
 *  2. As fractions of direct and diffuse light at the Earth's surface, which
 *     would be appropriate for calculating the intensity of direct and
 *     diffuse light given an experimental measurement of light intensity at the
 *     surface.
 *
 *  The basis for this function is given in chapter 11 of Norman and Campbell,
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
 *  ### BioCro module implementation
 *
 *  In BioCro, we use the following names for this model's input quantities:
 *
 *  - ``'cosine_zenith_angle'``: The cosine of the solar zenith angle; when the
 *    Sun is directly overhead, the angle is 0 and its cosine is 1; when the
 *    Sun's center is at the horizon, the angle is 90 and its cosine is 0.
 *
 *  - ``'atmospheric_pressure'``: The local atmospheric pressure in Pa
 *
 *  - ``'atmospheric_transmittance'``: Fraction of light transmitted through a
 *    small volume of atmosphere (dimensionless)
 *
 *  - ``'atmospheric_scattering'``: Atmospheric scattering factor
 *    (dimensionless)
 *
 *  We use the following names for the model's output quantities:
 *
 *  - ``'irradiance_direct_transmittance'``: Atmospheric transmittance to direct
 *    radiation (dimensionless)
 *
 *  - ``'irradiance_diffuse_transmittance'``: Atmospheric transmittance to
 *    diffuse radiation (dimensionless)
 *
 *  - ``'irradiance_direct_fraction'``: Fraction of direct irradiance at the Earth's
 *    surface (dimensionless)
 *
 *  - ``'irradiance_diffuse_fraction'``: Fraction of diffuse irradiance at the Earth's
 *    surface (dimensionless)
 */
class shortwave_atmospheric_scattering : public direct_module
{
   public:
    shortwave_atmospheric_scattering(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          atmospheric_transmittance{get_input(input_quantities, "atmospheric_transmittance")},
          atmospheric_scattering{get_input(input_quantities, "atmospheric_scattering")},

          // Get pointers to output quantities
          irradiance_direct_transmittance_op{get_op(output_quantities, "irradiance_direct_transmittance")},
          irradiance_diffuse_transmittance_op{get_op(output_quantities, "irradiance_diffuse_transmittance")},
          irradiance_direct_fraction_op{get_op(output_quantities, "irradiance_direct_fraction")},
          irradiance_diffuse_fraction_op{get_op(output_quantities, "irradiance_diffuse_fraction")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "shortwave_atmospheric_scattering"; }

   private:
    // References to input quantities
    double const& cosine_zenith_angle;
    double const& atmospheric_pressure;
    double const& atmospheric_transmittance;
    double const& atmospheric_scattering;

    // Pointers to output quantities
    double* irradiance_direct_transmittance_op;
    double* irradiance_diffuse_transmittance_op;
    double* irradiance_direct_fraction_op;
    double* irradiance_diffuse_fraction_op;

    // Main operation
    void do_operation() const;
};

string_vector shortwave_atmospheric_scattering::get_inputs()
{
    return {
        "cosine_zenith_angle",        // dimensionless
        "atmospheric_pressure",       // Pa
        "atmospheric_transmittance",  // dimensionless
        "atmospheric_scattering"      // dimensionless
    };
}

string_vector shortwave_atmospheric_scattering::get_outputs()
{
    return {
        "irradiance_direct_transmittance",   // dimensionless
        "irradiance_diffuse_transmittance",  // dimensionless
        "irradiance_direct_fraction",        // dimensionless
        "irradiance_diffuse_fraction"        // dimensionless
    };
}

void shortwave_atmospheric_scattering::do_operation() const
{
    using physical_constants::atmospheric_pressure_at_sea_level;

    // Dimensionless quantity used in later calculations.
    double const pressure_ratio =
        atmospheric_pressure / atmospheric_pressure_at_sea_level;

    // Equation 11.1 from Campbell & Norman, solving for
    // direct_transmittance = S_p / S_p0 (dimensionless).
    // If the sun is near the horizon, take the limit as cosine_zenith_angle
    // approaches 0 (which is 0).
    double const direct_transmittance =
        cosine_zenith_angle <= 0 ? 0
                                 : std::pow(atmospheric_transmittance,
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
    double const irradiance_direct_fraction =
        cosine_zenith_angle <= 0 ? 0
                                 : direct_transmittance /
                                       (direct_transmittance + diffuse_transmittance);

    // The remaining irradiance is diffuse (dimensionless).
    double const irradiance_diffuse_fraction = 1.0 - irradiance_direct_fraction;

    update(irradiance_direct_transmittance_op, direct_transmittance);
    update(irradiance_diffuse_transmittance_op, diffuse_transmittance);
    update(irradiance_direct_fraction_op, irradiance_direct_fraction);
    update(irradiance_diffuse_fraction_op, irradiance_diffuse_fraction);
}

}  // namespace standardBML

#endif
