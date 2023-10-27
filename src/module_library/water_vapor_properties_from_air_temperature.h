#ifndef WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE_H
#define WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "water_and_air_properties.h"  // for saturation_vapor_pressure,
                                       // TempToSFS, TempToLHV, TempToDdryA
#include "../framework/constants.h"    // for ideal_gas_constant,
                                       // molar_mass_of_water, celsius_to_kelvin

namespace standardBML
{
class water_vapor_properties_from_air_temperature : public direct_module
{
   public:
    water_vapor_properties_from_air_temperature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          temp{get_input(input_quantities, "temp")},
          rh{get_input(input_quantities, "rh")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},

          // Get pointers to output quantities
          latent_heat_vaporization_of_water_op{get_op(output_quantities, "latent_heat_vaporization_of_water")},
          slope_water_vapor_op{get_op(output_quantities, "slope_water_vapor")},
          saturation_water_vapor_pressure_op{get_op(output_quantities, "saturation_water_vapor_pressure")},
          water_vapor_pressure_op{get_op(output_quantities, "water_vapor_pressure")},
          vapor_density_deficit_op{get_op(output_quantities, "vapor_density_deficit")},
          psychrometric_parameter_op{get_op(output_quantities, "psychrometric_parameter")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "water_vapor_properties_from_air_temperature"; }

   private:
    // References to input quantities
    double const& temp;
    double const& rh;
    double const& specific_heat_of_air;

    // Pointers to output quantities
    double* latent_heat_vaporization_of_water_op;
    double* slope_water_vapor_op;
    double* saturation_water_vapor_pressure_op;
    double* water_vapor_pressure_op;
    double* vapor_density_deficit_op;
    double* psychrometric_parameter_op;

    // Main operation
    void do_operation() const;
};

string_vector water_vapor_properties_from_air_temperature::get_inputs()
{
    return {
        "temp",                 // degrees C
        "rh",                   // dimensionless
        "specific_heat_of_air"  // J / kg / K
    };
}

string_vector water_vapor_properties_from_air_temperature::get_outputs()
{
    return {
        "latent_heat_vaporization_of_water",  // J / kg
        "slope_water_vapor",                  // kg / m^3 / K
        "saturation_water_vapor_pressure",    // Pa
        "water_vapor_pressure",               // Pa
        "vapor_density_deficit",              // kg / m^3
        "psychrometric_parameter"             // kg / m^3 / K
    };
}

void water_vapor_properties_from_air_temperature::do_operation() const
{
    // Collect input quantities and make calculations

    double density_of_dry_air = TempToDdryA(temp);                             // kg / m^3
    double latent_heat_vaporization_of_water = TempToLHV(temp);                // J / kg
    double saturation_water_vapor_pressure = saturation_vapor_pressure(temp);  // Pa

    // Convert saturation water vapor pressure to vapor density using the ideal
    // gas law. This is approximately right for temperatures what won't kill
    // plants.
    double saturation_water_vapor_content =
        saturation_water_vapor_pressure / physical_constants::ideal_gas_constant /
        (temp + conversion_constants::celsius_to_kelvin) *
        physical_constants::molar_mass_of_water;  // kg / m^3

    double vapor_density_deficit = saturation_water_vapor_content * (1 - rh);  // kg / m^3

    double psychrometric_parameter =
        density_of_dry_air * specific_heat_of_air /
        latent_heat_vaporization_of_water;  // kg / m^3 / K

    // Update the output quantity list
    update(latent_heat_vaporization_of_water_op, latent_heat_vaporization_of_water);  // J / kg
    update(slope_water_vapor_op, TempToSFS(temp));                                    // kg / m^3 / K
    update(saturation_water_vapor_pressure_op, saturation_water_vapor_pressure);      // Pa
    update(water_vapor_pressure_op, saturation_water_vapor_pressure * rh);            // Pa
    update(vapor_density_deficit_op, vapor_density_deficit);                          // kg / m^3
    update(psychrometric_parameter_op, psychrometric_parameter);                      // kg / m^3 / K
}

}  // namespace standardBML
#endif
