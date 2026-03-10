#ifndef SOIL_WATER_UPTAKE_H
#define SOIL_WATER_UPTAKE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"

namespace standardBML
{
class soil_water_uptake : public direct_module
{
   public:
    soil_water_uptake(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},                  // cm
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},  // cm3 [water] / cm3 [soil]
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          soil_field_capacity_1{get_input(input_quantities, "soil_field_capacity_1")},

          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},
          soil_field_capacity_2{get_input(input_quantities, "soil_field_capacity_2")},

          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},
          soil_field_capacity_3{get_input(input_quantities, "soil_field_capacity_3")},

          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},
          soil_field_capacity_4{get_input(input_quantities, "soil_field_capacity_4")},

          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},
          soil_field_capacity_5{get_input(input_quantities, "soil_field_capacity_5")},

          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},
          soil_field_capacity_6{get_input(input_quantities, "soil_field_capacity_6")},

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},

          // pointers to output quantities
          uptake_layer_1_op{get_op(output_quantities, "uptake_layer_1")},
          uptake_layer_2_op{get_op(output_quantities, "uptake_layer_2")},
          uptake_layer_3_op{get_op(output_quantities, "uptake_layer_3")},
          uptake_layer_4_op{get_op(output_quantities, "uptake_layer_4")},
          uptake_layer_5_op{get_op(output_quantities, "uptake_layer_5")},
          uptake_layer_6_op{get_op(output_quantities, "uptake_layer_6")},
          unmet_demand_op{get_op(output_quantities, "unmet_demand")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_uptake"; }

   private:
    // references to input quantities

    double const& soil_depth_1;
    double const& soil_water_content_1;
    double const& soil_wilting_point_1;
    double const& soil_field_capacity_1;

    double const& soil_depth_2;
    double const& soil_water_content_2;
    double const& soil_wilting_point_2;
    double const& soil_field_capacity_2;

    double const& soil_depth_3;
    double const& soil_water_content_3;
    double const& soil_wilting_point_3;
    double const& soil_field_capacity_3;

    double const& soil_depth_4;
    double const& soil_water_content_4;
    double const& soil_wilting_point_4;
    double const& soil_field_capacity_4;

    double const& soil_depth_5;
    double const& soil_water_content_5;
    double const& soil_wilting_point_5;
    double const& soil_field_capacity_5;

    double const& soil_depth_6;
    double const& soil_water_content_6;
    double const& soil_wilting_point_6;
    double const& soil_field_capacity_6;

    double const& max_rooting_layer;
    double const& canopy_transpiration_rate;  // Mg/ha/hr

    // pointers to output quantities
    double* uptake_layer_1_op; //Mg/ha/hr
    double* uptake_layer_2_op;
    double* uptake_layer_3_op;
    double* uptake_layer_4_op;
    double* uptake_layer_5_op;
    double* uptake_layer_6_op;
    double* unmet_demand_op;

    // main operation
    void do_operation() const;
};

string_vector soil_water_uptake::get_inputs()
{
    return {

        "soil_depth_1",
        "soil_water_content_1",
        "soil_wilting_point_1",
        "soil_field_capacity_1",

        "soil_depth_2",
        "soil_water_content_2",
        "soil_wilting_point_2",
        "soil_field_capacity_2",

        "soil_depth_3",
        "soil_water_content_3",
        "soil_wilting_point_3",
        "soil_field_capacity_3",

        "soil_depth_4",
        "soil_water_content_4",
        "soil_wilting_point_4",
        "soil_field_capacity_4",

        "soil_depth_5",
        "soil_water_content_5",
        "soil_wilting_point_5",
        "soil_field_capacity_5",

        "soil_depth_6",
        "soil_water_content_6",
        "soil_wilting_point_6",
        "soil_field_capacity_6",

        "max_rooting_layer",
        "canopy_transpiration_rate"

    };
}

string_vector soil_water_uptake::get_outputs()
{
    return {

        "uptake_layer_1",
        "uptake_layer_2",
        "uptake_layer_3",
        "uptake_layer_4",
        "uptake_layer_5",
        "uptake_layer_6",
        "unmet_demand"

    };
}

void soil_water_uptake::do_operation() const
{
    // define empty array for uptake
    double uptake[6] = {0};
    // create array of soil depths
    double soil_depth[] = {
        soil_depth_1,
        soil_depth_2,
        soil_depth_3,
        soil_depth_4,
        soil_depth_5,
        soil_depth_6};

    double soil_field_capacity[] = {
        soil_field_capacity_1,
        soil_field_capacity_2,
        soil_field_capacity_3,
        soil_field_capacity_4,
        soil_field_capacity_5,
        soil_field_capacity_6};

    double soil_wilting_point[] = {
        soil_wilting_point_1,
        soil_wilting_point_2,
        soil_wilting_point_3,
        soil_wilting_point_4,
        soil_wilting_point_5,
        soil_wilting_point_6};

    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    // Calculate the weight of roots in each layer
    double root_weight[6] = {0.0};
    double total_weight = 0.0;
    double current_top_depth = 0.0; // top depth of the rooting layer 

    // get total root depth
    for (int i = 0; i < max_rooting_layer; i++) {
        // Find the middle depth of the current layer
        double mid_depth = current_top_depth + (soil_depth[i] / 2.0);
        // Root fraction drop off fast. The number 0.1 controls how fast. 
        // A bigger number makes the surface roots gets more weights.
        // Root fraction profile is known to decrease exponentially with depth. 
        // Jackson, R.B., Canadell, J., Ehleringer, J.R. et al. 
        // A global analysis of root distributions for terrestrial biomes.
        // Oecologia 108, 389–411 (1996). https://doi.org/10.1007/BF00333714
        double base_weight = exp(-0.1 * mid_depth) * soil_depth[i];
        root_weight[i] = base_weight;
 
        total_weight = total_weight + root_weight[i];
        current_top_depth = current_top_depth + soil_depth[i];
    }

    // uptake in layer = ET*(depth of layer/total depth of root zone)
    // we track the water demand for each layer.
    // If the demand cannot be met in one layer, try the next layer upto max_rooting_layer
    // The unmet_demand can still be there after exhausting all layers 
    // This should be a feedback to the leaf level to lower gs/ET to make sure 
    // water is balanced. However, it's difficult to solve this canopy to leaf feedback
    // Also, one should check how often unmet_demand becomes non-zeros for diagnostic
    double unmet_demand = 0.0;
    for (int i = 0; i < max_rooting_layer; i++) {
       // Check if all dirt is totally dry
        if (total_weight > 0.0) {
            double base_request = -canopy_transpiration_rate * (root_weight[i] / total_weight);
            double requested_uptake = base_request + unmet_demand;
            double max_water_volume = 0.0;
            // Only calculate volume if we are above the WP 
            if (soil_water_content[i] > soil_wilting_point[i]) {
               max_water_volume = (soil_water_content[i] - soil_wilting_point[i]) * 100* soil_depth[i];//Mg/ha
            }
            // Make it negative because uptake is negative
            double max_negative_uptake = -max_water_volume;
            if (requested_uptake < max_negative_uptake) {
                // The plant takes all the available water
                uptake[i] = max_negative_uptake;
                // The plant saves the missing amount for the next layer
                unmet_demand = requested_uptake - max_negative_uptake;
            } else {
                // The dirt has enough water
                uptake[i] = requested_uptake;
                // The plant has no missing demand
                unmet_demand = 0.0; 
            }
        } else {
            uptake[i] = 0.0; 
        }
    }

    // update uptake
    update(uptake_layer_1_op, uptake[0]);
    update(uptake_layer_2_op, uptake[1]);
    update(uptake_layer_3_op, uptake[2]);
    update(uptake_layer_4_op, uptake[3]);
    update(uptake_layer_5_op, uptake[4]);
    update(uptake_layer_6_op, uptake[5]);
    update(unmet_demand_op, unmet_demand);
}

}  // namespace standardBML
#endif
