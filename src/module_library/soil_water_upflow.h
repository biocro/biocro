#ifndef SOIL_WATER_UPFLOW_H
#define SOIL_WATER_UPFLOW_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"
#include <algorithm> // for std::max

namespace standardBML
{
/**
 * @class soil_water_downflow
 *
 * @brief Calculates water flow through the soil.
 *
 */
class soil_water_upflow : public direct_module
{
    static const int num_layers = 6;
    struct soil_layer {
        const double& depth;
        const double& saturation_capacity;
        const double& wilting_point;
        const double& field_capacity;
        const double& water_content;
        const double& deltaS;
        double water_avail;

        double* deltaS_op;        
        double* deltaU_op;


        soil_layer(state_map const& input_quantities, state_map* output_quantities, int layer) :
        depth{get_input(input_quantities, "soil_depth_" + std::to_string(layer))},
        saturation_capacity{get_input(input_quantities, "soil_saturation_capacity_" + std::to_string(layer))},
        wilting_point{get_input(input_quantities, "soil_wilting_point_" + std::to_string(layer))},
        field_capacity{get_input(input_quantities, "soil_field_capacity_" + std::to_string(layer))},
        water_content{get_input(input_quantities, "soil_water_content_" + std::to_string(layer))},        
        deltaS{get_input(input_quantities, "deltaS_" + std::to_string(layer))},
        water_avail{std::max(0.0, water_content + deltaS)},
        deltaS_op{get_op(output_quantities, "deltaS_" + std::to_string(layer))},
        deltaU_op{get_op(output_quantities, "deltaU_" + std::to_string(layer))} {
                }
    };
   public:
    soil_water_upflow(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          surface_runoff{get_input(input_quantities, "surface_runoff")},

    {
        for (int i = 1; i < num_layers + 1; ++i)
            layers.emplace_back(soil_layer(input_quantities, output_quantities, i));
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_upflow"; }

   private:
    // References to input quantities
    double const& surface_runoff;
    std::vector<soil_layer> layers;
    // Main operation
    void do_operation() const;
};

string_vector soil_water_upflow::get_inputs()
{
    return {
        "surface_runoff",  // Excess water (cm/hr) to check for the flooded conditions

        "soil_depth_1",
        "soil_saturation_capacity_1",
        "soil_wilting_point_1",
        "soil_field_capacity_1",
        "soil_water_content_1",
        "deltaS_1",

        "soil_depth_2",
        "soil_saturation_capacity_2",
        "soil_wilting_point_2",
        "soil_field_capacity_2",
        "soil_water_content_2",
        "deltaS_2",

        "soil_depth_3",
        "soil_saturation_capacity_3",
        "soil_wilting_point_3",
        "soil_field_capacity_3",
        "soil_water_content_3",
        "deltaS_3",

        "soil_depth_4",
        "soil_saturation_capacity_4",
        "soil_wilting_point_4",
        "soil_field_capacity_4",
        "soil_water_content_4",
        "deltaS_4",

        "soil_depth_5",
        "soil_saturation_capacity_5",
        "soil_wilting_point_5",
        "soil_field_capacity_5",
        "soil_water_content_5",
        "deltaS_5",

        "soil_depth_6",
        "soil_saturation_capacity_6",
        "soil_wilting_point_6",
        "soil_field_capacity_6",
        "soil_water_content_6",
        "deltaS_6"};
}

string_vector soil_water_upflow::get_outputs()
{
    return {
        "upflow_1",
        "upflow_2",
        "upflow_3",
        "upflow_4",
        "upflow_5",
        "upflow_6",
        "deltaU_1",  // Change in soil water content due to evaporation and/or upward flow in layer 1 (cm3 [water] / cm3 [soil])
        "deltaU_2",  // Change in soil water content due to evaporation and/or upward flow in layer 2 (cm3 [water] / cm3 [soil])
        "deltaU_3",  // Change in soil water content due to evaporation and/or upward flow in layer 3 (cm3 [water] / cm3 [soil])
        "deltaU_4",  // Change in soil water content due to evaporation and/or upward flow in layer 4 (cm3 [water] / cm3 [soil])
        "deltaU_5",  // Change in soil water content due to evaporation and/or upward flow in layer 5 (cm3 [water] / cm3 [soil])
        "deltaU_6"   // Change in soil water content due to evaporation and/or upward flow in layer 6 (cm3 [water] / cm3 [soil])
    };
}

void soil_water_upflow::do_operation() const
{
   

    upwardFlo_str upFlow;
    // if flood <= 0??? // Current depth of flooding (mm)
    // if (surface_runoff <= 0) {
    double sw_avail;
    for (int l = 0; l < num_layers; ++l) {
        sw_avail = std::max(0.0, layers[l].water_content + swdelts[l]);
    }
    // Calculate upward movement of water due to evaporation and root
    // extraction for each soil layer.

    upFlow = up_flow(
        num_layers,
        sw_avail,
        soil_depth,
        soil_saturation_capacity,
        soil_wilting_point,
        soil_field_capacity,
        soil_water_content);
    // } else {
    //     for (int l = 0; l < nlayers; l++){
    //           upFlow.sw_delta_U[l] = 0.0;
    //       }
    // }

    // Update the output quantity list
    update(upflow_1_op, upFlow.upwardFlo[0]);
    update(upflow_2_op, upFlow.upwardFlo[1]);
    update(upflow_3_op, upFlow.upwardFlo[2]);
    update(upflow_4_op, upFlow.upwardFlo[3]);
    update(upflow_5_op, upFlow.upwardFlo[4]);
    update(upflow_6_op, upFlow.upwardFlo[5]);

    update(deltaU_1_op, upFlow.sw_delta_U[0]);
    update(deltaU_2_op, upFlow.sw_delta_U[1]);
    update(deltaU_3_op, upFlow.sw_delta_U[2]);
    update(deltaU_4_op, upFlow.sw_delta_U[3]);
    update(deltaU_5_op, upFlow.sw_delta_U[4]);
    update(deltaU_6_op, upFlow.sw_delta_U[5]);
}

}  // namespace standardBML
#endif
