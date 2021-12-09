#ifndef SHORTWAVE_ATMOSPHERIC_SCATTERING_H
#define SHORTWAVE_ATMOSPHERIC_SCATTERING_H

#include "../modules.h"
#include "../constants.h"
#include "../state_map.h"

/**
 * @class shortwave_atmospheric_scattering
 *
 * @brief Calculates the amount of sunlight scattered out of the direct beam by the atmosphere.
 * The result is expressed in two ways: as atmospheric transmittances (which would be appropriate
 * for calculating the intensity of direct and diffuse light at the surface given a value for the
 * solar constant) or as a fraction (which would be appropriate for calculating the intensity of
 * direct and diffuse light given an experimental measurement of light intensity at the surface).
 */
class shortwave_atmospheric_scattering : public direct_module
{
   public:
    shortwave_atmospheric_scattering(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("shortwave_atmospheric_scattering"),
          // Get references to input quantities
          cosine_zenith_angle(get_input(input_quantities, "cosine_zenith_angle")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          atmospheric_transmittance(get_input(input_quantities, "atmospheric_transmittance")),
          atmospheric_scattering(get_input(input_quantities, "atmospheric_scattering")),
          // Get pointers to output quantities
          irradiance_direct_transmittance_op(get_op(output_quantities, "irradiance_direct_transmittance")),
          irradiance_diffuse_transmittance_op(get_op(output_quantities, "irradiance_diffuse_transmittance")),
          irradiance_direct_fraction_op(get_op(output_quantities, "irradiance_direct_fraction")),
          irradiance_diffuse_fraction_op(get_op(output_quantities, "irradiance_diffuse_fraction"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

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
    // Calculate diffuse and direct irradiance transmittance
    double irradiance_direct_transmittance;
    double irradiance_diffuse_transmittance;
    if (cosine_zenith_angle <= 0) {
        // In this case, the sun is at or below the horizon, so there is no direct irradiance
        irradiance_direct_transmittance = 0.0;
        irradiance_diffuse_transmittance = 1.0;
    } else {
        // The sun is above the horizon, so use its zenith angle to calculate transmittance
        // The direct transmission is calculated using Equation 11.11 from Campbell & Norman (1998), solving for
        //  irradiance_direct_transmittance = S_p / S_p0
        // The indirect (AKA diffuse) transmission is calculated using Equation 11.13 from Campbell & Norman (1998), solving for
        //  irradiance_diffuse_transmittance = S_d / S_p0
        const double pressure_ratio = atmospheric_pressure / physical_constants::atmospheric_pressure_at_sea_level;                 // dimensionless.
        irradiance_direct_transmittance = pow(atmospheric_transmittance, (pressure_ratio / cosine_zenith_angle));                   // Modified from equation 11.11 of Norman and Campbell.
        irradiance_diffuse_transmittance = atmospheric_scattering * (1.0 - irradiance_direct_transmittance) * cosine_zenith_angle;  // Modified from equation 11.13 of Norman and Campbell.
    }

    // Calculate the diffuse and direct fractions
    const double irradiance_direct_fraction = irradiance_direct_transmittance / (irradiance_direct_transmittance + irradiance_diffuse_transmittance);
    const double irradiance_diffuse_fraction = 1.0 - irradiance_direct_fraction;

    // Update the outputs
    update(irradiance_direct_transmittance_op, irradiance_direct_transmittance);
    update(irradiance_diffuse_transmittance_op, irradiance_diffuse_transmittance);
    update(irradiance_direct_fraction_op, irradiance_direct_fraction);
    update(irradiance_diffuse_fraction_op, irradiance_diffuse_fraction);
}

#endif
