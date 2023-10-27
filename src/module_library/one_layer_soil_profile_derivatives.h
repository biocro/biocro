#ifndef ONE_LAYER_SOIL_PROFILE_DERIVATIVES_H
#define ONE_LAYER_SOIL_PROFILE_DERIVATIVES_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  // for molar_mass_of_water
#include <cmath>                     // for log

namespace standardBML
{
class one_layer_soil_profile_derivatives : public differential_module
{
   public:
    one_layer_soil_profile_derivatives(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          soil_depth{get_input(input_quantities, "soil_depth")},
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          soil_saturated_conductivity{get_input(input_quantities, "soil_saturated_conductivity")},
          soil_air_entry{get_input(input_quantities, "soil_air_entry")},
          soil_b_coefficient{get_input(input_quantities, "soil_b_coefficient")},
          acceleration_from_gravity{get_input(input_quantities, "acceleration_from_gravity")},
          precipitation_rate{get_input(input_quantities, "precipitation_rate")},
          soil_saturation_capacity{get_input(input_quantities, "soil_saturation_capacity")},
          soil_sand_content{get_input(input_quantities, "soil_sand_content")},
          evapotranspiration{get_input(input_quantities, "evapotranspiration")},

          // Get pointers to output quantities
          soil_water_content_op{get_op(output_quantities, "soil_water_content")},
          soil_n_content_op{get_op(output_quantities, "soil_n_content")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "one_layer_soil_profile_derivatives"; }

   private:
    // References to input quantities
    double const& soil_water_content;
    double const& soil_depth;
    double const& soil_field_capacity;
    double const& soil_wilting_point;
    double const& soil_saturated_conductivity;
    double const& soil_air_entry;
    double const& soil_b_coefficient;
    double const& acceleration_from_gravity;
    double const& precipitation_rate;
    double const& soil_saturation_capacity;
    double const& soil_sand_content;
    double const& evapotranspiration;

    // Pointers to output quantities
    double* soil_water_content_op;
    double* soil_n_content_op;

    // Main operation
    void do_operation() const;
};

string_vector one_layer_soil_profile_derivatives::get_inputs()
{
    return {
        "soil_water_content",
        "soil_depth",
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_saturated_conductivity",
        "soil_air_entry",
        "soil_b_coefficient",
        "acceleration_from_gravity",
        "precipitation_rate",
        "soil_saturation_capacity",
        "soil_sand_content",
        "evapotranspiration"};
}

string_vector one_layer_soil_profile_derivatives::get_outputs()
{
    return {
        "soil_water_content",
        "soil_n_content"};
}

void one_layer_soil_profile_derivatives::do_operation() const
{
    double soil_water_potential = -exp(log(0.033) + ((log(soil_field_capacity) - log(soil_water_content)) / (log(soil_field_capacity) - log(soil_wilting_point)) * (log(1.5) - log(0.033)))) * 1e3;  // kPa.

    double hydraulic_conductivity = soil_saturated_conductivity * pow(soil_air_entry / soil_water_potential, 2 + 3 / soil_b_coefficient);      // Kg s m^-3.
    double J_w = -hydraulic_conductivity * (-soil_water_potential / (soil_depth * 0.5)) - acceleration_from_gravity * hydraulic_conductivity;  // kg m^-2 s^-1. Campbell, pg 129. I multiply soil depth by 0.5 to calculate the average depth.

    double density_of_water_at_20_celcius = 998.2;  // kg m^-3

    double drainage = J_w / density_of_water_at_20_celcius;  // m s^-1.
    double precipitation_m_s = precipitation_rate * 1e-3;    // m s^-2.

    double second_per_hour = 3600;
    double runoff = (soil_water_content - soil_saturation_capacity) * soil_depth / second_per_hour;                                 // m s^-1. The previous model drained everything in a single time-step. By default the time step was one hour, so use a rate that drains everything in one hour.
    double n_leach = runoff / (1e3 * physical_constants::molar_mass_of_water) * (0.2 + 0.7 * soil_sand_content) / second_per_hour;  // Base the rate on an hour for the same reason as was used with `runoff`.

    double evapotranspiration_volume = evapotranspiration / density_of_water_at_20_celcius / 1e4 / second_per_hour;  // m^3 m^-2 s^-1

    // Update the output quantity list
    update(soil_water_content_op, (drainage + precipitation_m_s - runoff - evapotranspiration_volume) / soil_depth);
    update(soil_n_content_op, -n_leach);
}

}  // namespace standardBML
#endif
