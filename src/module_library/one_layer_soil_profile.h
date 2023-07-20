#ifndef ONE_LAYER_SOIL_PROFILE_H
#define ONE_LAYER_SOIL_PROFILE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "AuxBioCro.h"  // For ws_str
#include "BioCro.h"     // For watstr

namespace standardBML
{
class one_layer_soil_profile : public differential_module
{
   public:
    one_layer_soil_profile(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          soil_evaporation_rate_ip{get_ip(input_quantities, "soil_evaporation_rate")},
          canopy_transpiration_rate_ip{get_ip(input_quantities, "canopy_transpiration_rate")},
          precip_ip{get_ip(input_quantities, "precip")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},
          soil_depth_ip{get_ip(input_quantities, "soil_depth")},
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_saturation_capacity_ip{get_ip(input_quantities, "soil_saturation_capacity")},
          soil_sand_content_ip{get_ip(input_quantities, "soil_sand_content")},
          soil_saturated_conductivity_ip{get_ip(input_quantities, "soil_saturated_conductivity")},
          soil_air_entry_ip{get_ip(input_quantities, "soil_air_entry")},
          soil_b_coefficient_ip{get_ip(input_quantities, "soil_b_coefficient")},

          // Get pointers to output quantities
          soil_water_content_op{get_op(output_quantities, "soil_water_content")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "one_layer_soil_profile"; }

   private:
    // Pointers to input quantities
    const double* soil_evaporation_rate_ip;
    const double* canopy_transpiration_rate_ip;
    const double* precip_ip;
    const double* soil_water_content_ip;
    const double* soil_depth_ip;
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_saturation_capacity_ip;
    const double* soil_sand_content_ip;
    const double* soil_saturated_conductivity_ip;
    const double* soil_air_entry_ip;
    const double* soil_b_coefficient_ip;

    // Pointers to output quantities
    double* soil_water_content_op;

    // Main operation
    void do_operation() const;
};

string_vector one_layer_soil_profile::get_inputs()
{
    return {
        "soil_evaporation_rate",
        "canopy_transpiration_rate",
        "precip",
        "soil_water_content",
        "soil_depth",
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_saturation_capacity",
        "soil_sand_content",
        "soil_saturated_conductivity",
        "soil_air_entry",
        "soil_b_coefficient"};
}

string_vector one_layer_soil_profile::get_outputs()
{
    return {
        "soil_water_content"};
}

void one_layer_soil_profile::do_operation() const
{
    // Collect inputs and make calculations
    double soilEvap = (*soil_evaporation_rate_ip);  // Mg / ha / hr
    double TotEvap = soilEvap + (*canopy_transpiration_rate_ip);

    // watstr(...) is located in AuxBioCro.cpp
    struct ws_str WaterS = watstr(*precip_ip, TotEvap, *soil_water_content_ip, *soil_depth_ip, *soil_field_capacity_ip,
                                  *soil_wilting_point_ip, *soil_saturation_capacity_ip, *soil_sand_content_ip,
                                  *soil_saturated_conductivity_ip, *soil_air_entry_ip, *soil_b_coefficient_ip);

    // Update the output quantity list
    update(soil_water_content_op, WaterS.awc - (*soil_water_content_ip));
}

}  // namespace standardBML
#endif
