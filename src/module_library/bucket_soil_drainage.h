#ifndef BUCKET_SOIL_DRAINAGE_H
#define BUCKET_SOIL_DRAINAGE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class bucket_soil_drainage : public differential_module
{
   public:
    bucket_soil_drainage(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},
          soil_saturation_capacity_ip{get_ip(input_quantities, "soil_saturation_capacity")},
          soil_depth_ip{get_ip(input_quantities, "soil_depth")},
          precipitation_rate_ip{get_ip(input_quantities, "precipitation_rate")},
          soil_saturated_conductivity_ip{get_ip(input_quantities, "soil_saturated_conductivity")},
          soil_air_entry_ip{get_ip(input_quantities, "soil_air_entry")},
          soil_b_coefficient_ip{get_ip(input_quantities, "soil_b_coefficient")},
          canopy_transpiration_rate_ip{get_ip(input_quantities, "canopy_transpiration_rate")},
          soil_evaporation_rate_ip{get_ip(input_quantities, "soil_evaporation_rate")},

          // Get pointers to output quantities
          soil_water_content_op{get_op(output_quantities, "soil_water_content")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "bucket_soil_drainage"; }

   private:
    // Pointers to input quantities
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;
    const double* soil_saturation_capacity_ip;
    const double* soil_depth_ip;
    const double* precipitation_rate_ip;
    const double* soil_saturated_conductivity_ip;
    const double* soil_air_entry_ip;
    const double* soil_b_coefficient_ip;
    const double* canopy_transpiration_rate_ip;
    const double* soil_evaporation_rate_ip;

    // Pointers to output quantities
    double* soil_water_content_op;

    // Main operation
    void do_operation() const;
};

string_vector bucket_soil_drainage::get_inputs()
{
    return {
        "soil_field_capacity",          //
        "soil_wilting_point",           //
        "soil_water_content",           //
        "soil_saturation_capacity",     //
        "soil_depth",                   //
        "precipitation_rate",           //
        "soil_saturated_conductivity",  //
        "soil_air_entry",               //
        "soil_b_coefficient",           //
        "canopy_transpiration_rate",    //
        "soil_evaporation_rate"         //
    };
}

string_vector bucket_soil_drainage::get_outputs()
{
    return {
        "soil_water_content"  //
    };
}

void bucket_soil_drainage::do_operation() const
{
    // Collect inputs and make calculations
    double soil_field_capacity = *soil_field_capacity_ip;            // m^3 / m^3.
    double soil_wilting_point = *soil_wilting_point_ip;              // m^3 / m^3.
    double soil_water_content = *soil_water_content_ip;              // m^3 / m^3.
    double soil_saturation_capacity = *soil_saturation_capacity_ip;  // m^3 / m^3.
    double soil_depth = *soil_depth_ip;                              // meters
    double precipitation_rate = *precipitation_rate_ip;              // m / s.
    double soil_saturated_conductivity = *soil_saturated_conductivity_ip;
    double soil_air_entry = *soil_air_entry_ip;
    double soil_b_coefficient = *soil_b_coefficient_ip;
    double canopy_transpiration_rate = *canopy_transpiration_rate_ip;
    double soil_evaporation_rate = *soil_evaporation_rate_ip;

    constexpr double g = 9.8;                                 // m / s^2. Acceleration due to gravity.
    constexpr double density_of_water_at_20_celcius = 998.2;  // kg m^-3.

    /* soil_matric_potential is calculated as per "Dynamic Simulation of Water Deficit Effects upon Maize Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
    double soil_matric_potential = -exp(log(0.033) + log(soil_field_capacity / soil_water_content) / log(soil_field_capacity / soil_wilting_point) * log(1.5 / 0.033)) * 1e3;  // This last term converts from MPa to kPa.
    double hydraulic_conductivity = soil_saturated_conductivity * pow(soil_air_entry / soil_matric_potential, 2 + 3 / soil_b_coefficient);                                     // kg s / m^3.

    double drainage = -hydraulic_conductivity * g / density_of_water_at_20_celcius;  // m / s.

    constexpr double runoff_rate = 1 / 3600;                                                                  // Runoff 1 m^3 / hr.
    double runoff = std::min(0.0, soil_water_content - soil_saturation_capacity) * runoff_rate * soil_depth;  // m / s.

    double transpiration_rate = canopy_transpiration_rate / density_of_water_at_20_celcius * 1000 / 10000 / 3600;  // m / s.
    double evaporation_rate = soil_evaporation_rate / density_of_water_at_20_celcius;                              // m / s.

    // Update the output quantity list
    update(soil_water_content_op, (precipitation_rate - transpiration_rate - evaporation_rate - runoff - drainage) / soil_depth * 3600);  // m^3 / m^3 / hr;
}

}  // namespace standardBML
#endif
