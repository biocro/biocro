#ifndef SOIL_WATER_UPTAKE_H
#define SOIL_WATER_UPTAKE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"

namespace standardBML
{
/**
 *  @class soil_water_uptake
 *
 *  @brief Computes the root water uptake for each soil layer
 *  using canopy_transpiration_rate as proxy.
 *  This module is used with other DSSAT-based water modules,
 *  but is homemade (not based on any DSSAT codes).
 *
 */
class soil_water_uptake : public direct_module
{
   public:
    soil_water_uptake(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},

          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},

          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},

          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},

          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},

          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},

          // pointers to output quantities
          uptake_layer_1_op{get_op(output_quantities, "uptake_layer_1")},
          uptake_layer_2_op{get_op(output_quantities, "uptake_layer_2")},
          uptake_layer_3_op{get_op(output_quantities, "uptake_layer_3")},
          uptake_layer_4_op{get_op(output_quantities, "uptake_layer_4")},
          uptake_layer_5_op{get_op(output_quantities, "uptake_layer_5")},
          uptake_layer_6_op{get_op(output_quantities, "uptake_layer_6")},
          unmet_demand_op{get_op(output_quantities, "unmet_demand_rate")}
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

    double const& soil_depth_2;
    double const& soil_water_content_2;
    double const& soil_wilting_point_2;

    double const& soil_depth_3;
    double const& soil_water_content_3;
    double const& soil_wilting_point_3;

    double const& soil_depth_4;
    double const& soil_water_content_4;
    double const& soil_wilting_point_4;

    double const& soil_depth_5;
    double const& soil_water_content_5;
    double const& soil_wilting_point_5;

    double const& soil_depth_6;
    double const& soil_water_content_6;
    double const& soil_wilting_point_6;

    double const& max_rooting_layer;
    double const& canopy_transpiration_rate;

    // pointers to output quantities
    double* uptake_layer_1_op;  // Mg / ha /hr
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
        "soil_depth_1",              // cm
        "soil_water_content_1",      // m^3 / m^3. Volumetric water content
        "soil_wilting_point_1",      // m^3 / m^3
        "soil_depth_2",              // cm
        "soil_water_content_2",      // m^3 / m^3
        "soil_wilting_point_2",      // m^3 / m^3
        "soil_depth_3",              // cm
        "soil_water_content_3",      // m^3 / m^3
        "soil_wilting_point_3",      // m^3 / m^3
        "soil_depth_4",              // cm
        "soil_water_content_4",      // m^3 / m^3
        "soil_wilting_point_4",      // m^3 / m^3
        "soil_depth_5",              // cm
        "soil_water_content_5",      // m^3 / m^3
        "soil_wilting_point_5",      // m^3 / m^3
        "soil_depth_6",              // cm
        "soil_water_content_6",      // m^3 / m^3
        "soil_wilting_point_6",      // m^3 / m^3
        "max_rooting_layer",         // dimensionless
        "canopy_transpiration_rate"  // Mg / ha / hr
    };
}

string_vector soil_water_uptake::get_outputs()
{
    return {
        "uptake_layer_1",    // Mg / ha / hr
        "uptake_layer_2",    // Mg / ha / hr
        "uptake_layer_3",    // Mg / ha / hr
        "uptake_layer_4",    // Mg / ha / hr
        "uptake_layer_5",    // Mg / ha / hr
        "uptake_layer_6",    // Mg / ha / hr
        "unmet_demand_rate"  // Mg / ha / hr
    };
}

void soil_water_uptake::do_operation() const
{
    // define empty array for uptake
    double uptake[6] = {0};  // Mg / ha / hr

    // create array of soil depths
    double soil_depth[] = {
        soil_depth_1,  // cm
        soil_depth_2,  // cm
        soil_depth_3,  // cm
        soil_depth_4,  // cm
        soil_depth_5,  // cm
        soil_depth_6   // cm
    };

    double soil_wilting_point[] = {
        soil_wilting_point_1,  // m^3 / m^3
        soil_wilting_point_2,  // m^3 / m^3
        soil_wilting_point_3,  // m^3 / m^3
        soil_wilting_point_4,  // m^3 / m^3
        soil_wilting_point_5,  // m^3 / m^3
        soil_wilting_point_6   // m^3 / m^3
    };

    double soil_water_content[] = {
        soil_water_content_1,  // m^3 / m^3
        soil_water_content_2,  // m^3 / m^3
        soil_water_content_3,  // m^3 / m^3
        soil_water_content_4,  // m^3 / m^3
        soil_water_content_5,  // m^3 / m^3
        soil_water_content_6   // m^3 / m^3
    };

    // Calculate the weight of roots in each layer
    double root_weight[6] = {0.0};   // dimensionless
    double total_weight = 0.0;       // dimensionless
    double current_top_depth = 0.0;  // cm. top depth of the rooting layer
    constexpr double timestep = 1;   // hr

    // 1 Mg/ha = 0.1  kg/m^2 = 0.1 mm of water because 1 mm of water on a 1 m2 area equals 1 kg.
    // Or 1 mm of water equals 10 Mg/ha.
    // For soil_water_content theta (cm3/cm3) = theta (mm/mm) in a D (cm)-thick soil,
    // Total water is theta * 10D (mm) = theta * 10D * 10 (Mg/ha) = theta * 100 * D
    double const volume_to_mass = 100;  // Mg / ha / cm

    // get total root depth
    for (int i = 0; i < max_rooting_layer; i++) {
        // Find the middle depth of the current layer
        double mid_depth = current_top_depth + (soil_depth[i] / 2.0);  // cm
        // Root fraction drop off fast. The number 0.1 controls how fast.
        // A bigger number makes the surface roots gets more weights.
        // Root fraction profile is known to decrease exponentially with depth.
        // Jackson, R.B., Canadell, J., Ehleringer, J.R. et al.
        // A global analysis of root distributions for terrestrial biomes.
        // Oecologia 108, 389–411 (1996). https://doi.org/10.1007/BF00333714
        double base_weight = exp(-0.1 * mid_depth) * soil_depth[i];  // dimensionless
        root_weight[i] = base_weight;                                // dimensionless

        total_weight = total_weight + root_weight[i];
        current_top_depth = current_top_depth + soil_depth[i];
    }

    // uptake in layer = ET*(weighted root fraction in that layer)
    // we track the water demand for each layer.
    // If the demand cannot be met in one layer, try the next layer upto max_rooting_layer
    // The unmet_demand_rate can still be there after exhausting all layers
    // This should be a feedback to the leaf level to lower gs/ET to make sure
    // water is balanced. However, it's difficult to solve this canopy to leaf feedback
    // Also, one should check how often unmet_demand_rate becomes non-zeros for diagnosis
    double unmet_demand_rate = 0.0;
    for (int i = 0; i < max_rooting_layer; i++) {
        // Check if all dirt is totally dry
        if (total_weight > 0.0) {
            double base_request = -canopy_transpiration_rate * (root_weight[i] / total_weight);  // Mg / ha / hr
            double requested_uptake = base_request + unmet_demand_rate;                          // Mg / ha / hr
            double max_water = 0.0;
            // Only calculate volume if we are above the WP
            if (soil_water_content[i] > soil_wilting_point[i]) {
                max_water = (soil_water_content[i] - soil_wilting_point[i]) * volume_to_mass * soil_depth[i];  // Mg / ha
            }
            // Make it negative because uptake is negative
            double max_negative_uptake = -max_water / timestep;  // Mg / ha / hr
            if (requested_uptake < max_negative_uptake) {
                // The plant takes all the available water
                uptake[i] = max_negative_uptake;
                // The plant saves the missing amount for the next layer
                unmet_demand_rate = requested_uptake - max_negative_uptake;
            } else {
                // The dirt has enough water
                uptake[i] = requested_uptake;
                // The plant has no missing demand
                unmet_demand_rate = 0.0;
            }
        } else {
            uptake[i] = 0.0;
        }
    }

    // update uptake, they are negative
    update(uptake_layer_1_op, uptake[0]);        // Mg / ha /hr
    update(uptake_layer_2_op, uptake[1]);        // Mg / ha /hr
    update(uptake_layer_3_op, uptake[2]);        // Mg / ha /hr
    update(uptake_layer_4_op, uptake[3]);        // Mg / ha /hr
    update(uptake_layer_5_op, uptake[4]);        // Mg / ha /hr
    update(uptake_layer_6_op, uptake[5]);        // Mg / ha /hr
    update(unmet_demand_op, unmet_demand_rate);  // Mg / ha /hr
}

}  // namespace standardBML
#endif
