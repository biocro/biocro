#ifndef SHORTWAVE_ATMOSPHERIC_SCATTERING_H
#define SHORTWAVE_ATMOSPHERIC_SCATTERING_H

#include "../framework/module.h"
#include "../framework/constants.h"
#include "../framework/state_map.h"
#include "lightME.h"  // for lightME

namespace standardBML
{
/**
 * @class shortwave_atmospheric_scattering
 *
 * @brief Uses `lightME()` to calculate the amount of sunlight scattered out of
 * the direct beam by the atmosphere.
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
    Light_model scattering = lightME(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    update(irradiance_direct_transmittance_op, scattering.direct_transmittance);
    update(irradiance_diffuse_transmittance_op, scattering.diffuse_transmittance);
    update(irradiance_direct_fraction_op, scattering.direct_fraction);
    update(irradiance_diffuse_fraction_op, scattering.diffuse_fraction);
}

}  // namespace standardBML

#endif
