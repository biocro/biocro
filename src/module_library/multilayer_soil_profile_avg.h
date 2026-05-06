#ifndef MULTILAYER_SOIL_PROFILE_AVG_H
#define MULTILAYER_SOIL_PROFILE_AVG_H

#include <cmath>  // for std::fmax
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class multilayer_soil_profile_avg
 * Calculate the average soil values based on max_rooting_layer.
 * Because max_rooting_layer changes throughout the season, this module
 * computes the mean values across those soil layers accessible to roots at a given time.
 * This allows us to represent soil water status with a single average soil water content value,
 * rather than multiple layer-specific values, when calculating stomatal water stress for photosynthesis.
 * This module is used with other DSSAT-based water modules,
 * but is homemade (not based on any DSSAT codes).
 */

class multilayer_soil_profile_avg : public direct_module

{
   public:
    multilayer_soil_profile_avg(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},

          // Inputs for layer 1
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_saturated_conductivity_1{get_input(input_quantities, "soil_saturated_conductivity_1")},
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},
          soil_field_capacity_1{get_input(input_quantities, "soil_field_capacity_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},

          // Inputs for layer 2
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_saturated_conductivity_2{get_input(input_quantities, "soil_saturated_conductivity_2")},
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},
          soil_field_capacity_2{get_input(input_quantities, "soil_field_capacity_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},

          // Inputs for layer 3
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_saturated_conductivity_3{get_input(input_quantities, "soil_saturated_conductivity_3")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},
          soil_field_capacity_3{get_input(input_quantities, "soil_field_capacity_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},

          // Inputs for layer 4
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_saturated_conductivity_4{get_input(input_quantities, "soil_saturated_conductivity_4")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},
          soil_field_capacity_4{get_input(input_quantities, "soil_field_capacity_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},

          // Inputs for layer 5
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_saturated_conductivity_5{get_input(input_quantities, "soil_saturated_conductivity_5")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},
          soil_field_capacity_5{get_input(input_quantities, "soil_field_capacity_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},

          // Inputs for layer 6
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_saturated_conductivity_6{get_input(input_quantities, "soil_saturated_conductivity_6")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},
          soil_field_capacity_6{get_input(input_quantities, "soil_field_capacity_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},

          // Get pointers to output quantities - Change in water content of each layer
          soil_water_content_op{get_op(output_quantities, "soil_water_content")},
          soil_saturated_conductivity_op{get_op(output_quantities, "soil_saturated_conductivity")},
          soil_saturation_capacity_op{get_op(output_quantities, "soil_saturation_capacity")},
          soil_field_capacity_op{get_op(output_quantities, "soil_field_capacity")},
          soil_wilting_point_op{get_op(output_quantities, "soil_wilting_point")}
    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "multilayer_soil_profile_avg"; }

   private:
    // References to input quantities

    double const& max_rooting_layer;

    // Parameters for layer 1
    double const& soil_depth_1;
    double const& soil_water_content_1;
    double const& soil_saturated_conductivity_1;
    double const& soil_saturation_capacity_1;
    double const& soil_field_capacity_1;
    double const& soil_wilting_point_1;

    // Parameters for layer 2
    double const& soil_depth_2;
    double const& soil_water_content_2;
    double const& soil_saturated_conductivity_2;
    double const& soil_saturation_capacity_2;
    double const& soil_field_capacity_2;
    double const& soil_wilting_point_2;

    // Parameters for layer 3
    double const& soil_depth_3;
    double const& soil_water_content_3;
    double const& soil_saturated_conductivity_3;
    double const& soil_saturation_capacity_3;
    double const& soil_field_capacity_3;
    double const& soil_wilting_point_3;

    // Parameters for layer 4
    double const& soil_depth_4;
    double const& soil_water_content_4;
    double const& soil_saturated_conductivity_4;
    double const& soil_saturation_capacity_4;
    double const& soil_field_capacity_4;
    double const& soil_wilting_point_4;

    // Parameters for layer 5
    double const& soil_depth_5;
    double const& soil_water_content_5;
    double const& soil_saturated_conductivity_5;
    double const& soil_saturation_capacity_5;
    double const& soil_field_capacity_5;
    double const& soil_wilting_point_5;

    // Parameters for layer 6
    double const& soil_depth_6;
    double const& soil_water_content_6;
    double const& soil_saturated_conductivity_6;
    double const& soil_saturation_capacity_6;
    double const& soil_field_capacity_6;
    double const& soil_wilting_point_6;

    // Pointers to output parameters
    double* soil_water_content_op;
    double* soil_saturated_conductivity_op;
    double* soil_saturation_capacity_op;
    double* soil_field_capacity_op;
    double* soil_wilting_point_op;

    // Main operation
    void do_operation() const;
};

string_vector multilayer_soil_profile_avg::get_inputs()
{
    return {
        "max_rooting_layer",              // dimensionless; number of layer the root can access
        "soil_depth_1",                   // cm
        "soil_water_content_1",           // m^3 / m^3
        "soil_saturated_conductivity_1",  // kg s / m^3
        "soil_saturation_capacity_1",     // m^3 / m^3
        "soil_field_capacity_1",          // m^3 / m^3
        "soil_wilting_point_1",           // m^3 / m^3
        "soil_depth_2",                   // cm
        "soil_water_content_2",           // m^3 / m^3
        "soil_saturated_conductivity_2",  // kg s / m^3
        "soil_saturation_capacity_2",     // m^3 / m^3
        "soil_field_capacity_2",          // m^3 / m^3
        "soil_wilting_point_2",           // m^3 / m^3
        "soil_depth_3",                   // cm
        "soil_water_content_3",           // m^3 / m^3
        "soil_saturated_conductivity_3",  // kg s / m^3
        "soil_saturation_capacity_3",     // m^3 / m^3
        "soil_field_capacity_3",          // m^3 / m^3
        "soil_wilting_point_3",           // m^3 / m^3
        "soil_depth_4",                   // cm
        "soil_water_content_4",           // m^3 / m^3
        "soil_saturated_conductivity_4",  // kg s / m^3
        "soil_saturation_capacity_4",     // m^3 / m^3
        "soil_field_capacity_4",          // m^3 / m^3
        "soil_wilting_point_4",           // m^3 / m^3
        "soil_depth_5",                   // cm
        "soil_water_content_5",           // m^3 / m^3
        "soil_saturated_conductivity_5",  // kg s / m^3
        "soil_saturation_capacity_5",     // m^3 / m^3
        "soil_field_capacity_5",          // m^3 / m^3
        "soil_wilting_point_5",           // m^3 / m^3
        "soil_depth_6",                   // cm
        "soil_water_content_6",           // m^3 / m^3
        "soil_saturated_conductivity_6",  // kg s / m^3
        "soil_saturation_capacity_6",     // m^3 / m^3
        "soil_field_capacity_6",          // m^3 / m^3
        "soil_wilting_point_6"            // m^3 / m^3
    };
}

string_vector multilayer_soil_profile_avg::get_outputs()
{
    return {
        "soil_water_content",           // m^3 / m^3
        "soil_saturated_conductivity",  // kg s / m^3
        "soil_saturation_capacity",     // m^3 / m^3
        "soil_field_capacity",          // m^3 / m^3
        "soil_wilting_point"            // m^3 / m^3
    };
}

void multilayer_soil_profile_avg::do_operation() const
{
    double soil_depth_arr[] = {
        soil_depth_1,  // cm
        soil_depth_2,  // cm
        soil_depth_3,  // cm
        soil_depth_4,  // cm
        soil_depth_5,  // cm
        soil_depth_6   // cm
    };

    double soil_water_content_arr[] = {
        soil_water_content_1,  // m^3 / m^3
        soil_water_content_2,  // m^3 / m^3
        soil_water_content_3,  // m^3 / m^3
        soil_water_content_4,  // m^3 / m^3
        soil_water_content_5,  // m^3 / m^3
        soil_water_content_6   // m^3 / m^3
    };

    double soil_saturated_conductivity_arr[] = {
        soil_saturated_conductivity_1,  // kg s / m^3
        soil_saturated_conductivity_2,  // kg s / m^3
        soil_saturated_conductivity_3,  // kg s / m^3
        soil_saturated_conductivity_4,  // kg s / m^3
        soil_saturated_conductivity_5,  // kg s / m^3
        soil_saturated_conductivity_6   // kg s / m^3
    };

    double soil_saturation_capacity_arr[] = {
        soil_saturation_capacity_1,  // m^3 / m^3
        soil_saturation_capacity_2,  // m^3 / m^3
        soil_saturation_capacity_3,  // m^3 / m^3
        soil_saturation_capacity_4,  // m^3 / m^3
        soil_saturation_capacity_5,  // m^3 / m^3
        soil_saturation_capacity_6   // m^3 / m^3
    };

    double soil_field_capacity_arr[] = {
        soil_field_capacity_1,  // m^3 / m^3
        soil_field_capacity_2,  // m^3 / m^3
        soil_field_capacity_3,  // m^3 / m^3
        soil_field_capacity_4,  // m^3 / m^3
        soil_field_capacity_5,  // m^3 / m^3
        soil_field_capacity_6   // m^3 / m^3
    };

    double soil_wilting_point_arr[] = {
        soil_wilting_point_1,  // m^3 / m^3
        soil_wilting_point_2,  // m^3 / m^3
        soil_wilting_point_3,  // m^3 / m^3
        soil_wilting_point_4,  // m^3 / m^3
        soil_wilting_point_5,  // m^3 / m^3
        soil_wilting_point_6   // m^3 / m^3
    };

    double tot_soil_depth = 0.0;                   // cm
    double tot_soil_water_content = 0.0;           // m^3 / m^3 * cm
    double tot_soil_saturated_conductivity = 0.0;  // kg s / m^3 * cm
    double tot_soil_saturation_capacity = 0.0;     // m^3 / m^3 * cm
    double tot_soil_field_capacity = 0.0;          // m^3 / m^3 * cm
    double tot_soil_wilting_point = 0.0;           // m^3 / m^3 * cm

    for (int l = 0; l < max_rooting_layer; l++) {
        tot_soil_depth += soil_depth_arr[l];
        tot_soil_water_content += soil_water_content_arr[l] * soil_depth_arr[l];
        tot_soil_saturated_conductivity += soil_saturated_conductivity_arr[l] * soil_depth_arr[l];
        tot_soil_saturation_capacity += soil_saturation_capacity_arr[l] * soil_depth_arr[l];
        tot_soil_field_capacity += soil_field_capacity_arr[l] * soil_depth_arr[l];
        tot_soil_wilting_point += soil_wilting_point_arr[l] * soil_depth_arr[l];
    }
    // Voulmetric average soil parametric values
    double soil_water_content = tot_soil_water_content / tot_soil_depth;
    double soil_saturated_conductivity = tot_soil_saturated_conductivity / tot_soil_depth;
    double soil_saturation_capacity = tot_soil_saturation_capacity / tot_soil_depth;
    double soil_field_capacity = tot_soil_field_capacity / tot_soil_depth;
    double soil_wilting_point = tot_soil_wilting_point / tot_soil_depth;

    update(soil_water_content_op, soil_water_content);
    update(soil_saturated_conductivity_op, soil_saturated_conductivity);
    update(soil_saturation_capacity_op, soil_saturation_capacity);
    update(soil_field_capacity_op, soil_field_capacity);
    update(soil_wilting_point_op, soil_wilting_point);
}
}  // namespace standardBML
#endif
