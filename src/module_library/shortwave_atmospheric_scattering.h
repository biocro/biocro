#ifndef SHORTWAVE_ATMOSPHERIC_SCATTERING_H
#define SHORTWAVE_ATMOSPHERIC_SCATTERING_H

#include "../modules.h"
#include "../constants.h"

/**
 * @class shortwave_atmospheric_scattering
 * 
 * @brief Calculates the amount of sunlight scattered out of the direct beam by the atmosphere.
 * The result is expressed in two ways: as atmospheric transmittances (which would be appropriate
 * for calculating the intensity of direct and diffuse light at the surface given a value for the
 * solar constant) or as a fraction (which would be appropriate for calculating the intensity of
 * direct and diffuse light given an experimental measurement of light intensity at the surface).
 */
class shortwave_atmospheric_scattering : public SteadyModule
{
   public:
    shortwave_atmospheric_scattering(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("shortwave_atmospheric_scattering"),
          // Get references to input parameters
          cosine_zenith_angle(get_input(input_parameters, "cosine_zenith_angle")),
          atmospheric_pressure(get_input(input_parameters, "atmospheric_pressure")),
          atmospheric_transmittance(get_input(input_parameters, "atmospheric_transmittance")),
          atmospheric_scattering(get_input(input_parameters, "atmospheric_scattering")),
          // Get pointers to output parameters
          irradiance_direct_transmittance_op(get_op(output_parameters, "irradiance_direct_transmittance")),
          irradiance_diffuse_transmittance_op(get_op(output_parameters, "irradiance_diffuse_transmittance")),
          irradiance_direct_fraction_op(get_op(output_parameters, "irradiance_direct_fraction")),
          irradiance_diffuse_fraction_op(get_op(output_parameters, "irradiance_diffuse_fraction"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    static std::string get_description();

   private:
    // References to input parameters
    double const& cosine_zenith_angle;
    double const& atmospheric_pressure;
    double const& atmospheric_transmittance;
    double const& atmospheric_scattering;
    // Pointers to output parameters
    double* irradiance_direct_transmittance_op;
    double* irradiance_diffuse_transmittance_op;
    double* irradiance_direct_fraction_op;
    double* irradiance_diffuse_fraction_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> shortwave_atmospheric_scattering::get_inputs()
{
    return {
        "cosine_zenith_angle",        // dimensionless
        "atmospheric_pressure",       // Pa
        "atmospheric_transmittance",  // dimensionless
        "atmospheric_scattering"      // dimensionless
    };
}

std::vector<std::string> shortwave_atmospheric_scattering::get_outputs()
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
        const double pressure_ratio = atmospheric_pressure / physical_constants::pressure_at_sea_level;                             // dimensionless.
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
