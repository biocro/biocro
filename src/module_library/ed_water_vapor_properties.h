#ifndef ED_WATER_VAPOR_PROPERTIES_H
#define ED_WATER_VAPOR_PROPERTIES_H

#include "../modules.h"
#include "../state_map.h"
#include "AuxBioCro.h"     // for TempToDdryA, TempToLHV, saturation_vapor_pressure, TempToSFS
#include "../constants.h"  // for ideal gas constant and celsius_to_kelvin

/**
 * @class ed_water_vapor_properties
 *
 * @brief Determines water vapor properties from the air temperature
 * and atmospheric H2O mole fraction. Currently only intended for use by Ed.
 */
class ed_water_vapor_properties : public SteadyModule
{
   public:
    ed_water_vapor_properties(
        state_map const& input_quantities,
        state_map& output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_water_vapor_properties"),
          // Get pointers to input quantities
          temperature_air_ip(get_ip(input_quantities, "temp")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          mole_fraction_h2o_atmosphere_ip(get_ip(input_quantities, "mole_fraction_h2o_atmosphere")),
          specific_heat_of_air_ip(get_ip(input_quantities, "specific_heat_of_air")),
          // Get pointers to output quantities
          latent_heat_vaporization_of_water_op(get_op(output_quantities, "latent_heat_vaporization_of_water")),
          slope_water_vapor_op(get_op(output_quantities, "slope_water_vapor")),
          saturation_water_vapor_pressure_op(get_op(output_quantities, "saturation_water_vapor_pressure")),
          water_vapor_pressure_op(get_op(output_quantities, "water_vapor_pressure")),
          vapor_density_deficit_op(get_op(output_quantities, "vapor_density_deficit")),
          psychrometric_parameter_op(get_op(output_quantities, "psychrometric_parameter"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* temperature_air_ip;
    const double* atmospheric_pressure_ip;
    const double* mole_fraction_h2o_atmosphere_ip;
    const double* specific_heat_of_air_ip;
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

string_vector ed_water_vapor_properties::get_inputs()
{
    return {
        "temp",                          // deg. C
        "atmospheric_pressure",          // Pa
        "mole_fraction_h2o_atmosphere",  // dimensionless from mol / mol
        "specific_heat_of_air"           // J / kg / K
    };
}

string_vector ed_water_vapor_properties::get_outputs()
{
    return {
        "latent_heat_vaporization_of_water",  // J / kg
        "slope_water_vapor",                  // kg / m^3 / K. It is also kg / m^3 / degrees C since it's a change in temperature.
        "saturation_water_vapor_pressure",    // Pa
        "water_vapor_pressure",               // Pa
        "vapor_density_deficit",              // kg / m^3
        "psychrometric_parameter"             // kg / m^3 / K
    };
}

void ed_water_vapor_properties::do_operation() const
{
    const double latent_heat_vaporization_of_water = TempToLHV(*temperature_air_ip);                  // J / kg
    const double slope_water_vapor = TempToSFS(*temperature_air_ip);                                  // kg / m^3 / K
    const double saturation_water_vapor_pressure = saturation_vapor_pressure(*temperature_air_ip);    // Pa
    const double water_vapor_pressure = *mole_fraction_h2o_atmosphere_ip * *atmospheric_pressure_ip;  // Pa

    // Convert from vapor pressure to vapor density using the ideal gas law.
    // This is approximately right for temperatures what won't kill plants.
    const double saturation_water_vapor_content = saturation_water_vapor_pressure /
                                                  physical_constants::ideal_gas_constant /
                                                  (*temperature_air_ip + conversion_constants::celsius_to_kelvin) *
                                                  physical_constants::molar_mass_of_water;  // kg / m^3

    const double rh = water_vapor_pressure / saturation_water_vapor_pressure;        // dimensionless from Pa / Pa
    const double vapor_density_deficit = saturation_water_vapor_content * (1 - rh);  // Pa

    const double density_of_dry_air = TempToDdryA(*temperature_air_ip);  // kg / m^3
    const double psychrometric_parameter = density_of_dry_air *
                                           *specific_heat_of_air_ip /
                                           latent_heat_vaporization_of_water;  // kg / m^3 / K

    // Update the output quantity list
    update(latent_heat_vaporization_of_water_op, latent_heat_vaporization_of_water);
    update(slope_water_vapor_op, slope_water_vapor);
    update(saturation_water_vapor_pressure_op, saturation_water_vapor_pressure);
    update(water_vapor_pressure_op, water_vapor_pressure);
    update(vapor_density_deficit_op, vapor_density_deficit);
    update(psychrometric_parameter_op, psychrometric_parameter);
}

#endif
