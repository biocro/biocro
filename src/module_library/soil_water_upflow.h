#ifndef SOIL_WATER_UPFLOW_H
#define SOIL_WATER_UPFLOW_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"

namespace standardBML
{
/**
 * @class soil_water_upflow
 *
 * @brief Calculates water flow through the soil.
 * * Based on DSSAT source file:
 * - Soil/SoilWaterFlow/WBSUBS.for, subroutine UP_FLOW.
 * * Calculates upward movement of water through the soil profile.
 *
 */
class soil_water_upflow : public direct_module
{
   public:
    soil_water_upflow(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities

          // Inputs for layer 1
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          soil_field_capacity_1{get_input(input_quantities, "soil_field_capacity_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          deltaS_1{get_input(input_quantities, "deltaS_1")},

          // Inputs for layer 2
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},
          soil_field_capacity_2{get_input(input_quantities, "soil_field_capacity_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          deltaS_2{get_input(input_quantities, "deltaS_2")},

          // Inputs for layer 3
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},
          soil_field_capacity_3{get_input(input_quantities, "soil_field_capacity_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          deltaS_3{get_input(input_quantities, "deltaS_3")},

          // Inputs for layer 4
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},
          soil_field_capacity_4{get_input(input_quantities, "soil_field_capacity_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          deltaS_4{get_input(input_quantities, "deltaS_4")},

          // Inputs for layer 5
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},
          soil_field_capacity_5{get_input(input_quantities, "soil_field_capacity_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          deltaS_5{get_input(input_quantities, "deltaS_5")},

          // Inputs for layer 6
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},
          soil_field_capacity_6{get_input(input_quantities, "soil_field_capacity_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          deltaS_6{get_input(input_quantities, "deltaS_6")},

          // Get pointers to output quantities - Change in water content of each layer
          upflow_1_op{get_op(output_quantities, "upflow_1")},
          upflow_2_op{get_op(output_quantities, "upflow_2")},
          upflow_3_op{get_op(output_quantities, "upflow_3")},
          upflow_4_op{get_op(output_quantities, "upflow_4")},
          upflow_5_op{get_op(output_quantities, "upflow_5")},
          upflow_6_op{get_op(output_quantities, "upflow_6")},

          deltaU_1_op{get_op(output_quantities, "deltaU_1")},
          deltaU_2_op{get_op(output_quantities, "deltaU_2")},
          deltaU_3_op{get_op(output_quantities, "deltaU_3")},
          deltaU_4_op{get_op(output_quantities, "deltaU_4")},
          deltaU_5_op{get_op(output_quantities, "deltaU_5")},
          deltaU_6_op{get_op(output_quantities, "deltaU_6")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_upflow"; }

   private:
    // References to input quantities

    // Inputs for layer 1
    double const& soil_depth_1;
    double const& soil_saturation_capacity_1;
    double const& soil_wilting_point_1;
    double const& soil_field_capacity_1;
    double const& soil_water_content_1;
    double const& deltaS_1;

    // Inputs for layer 2
    double const& soil_depth_2;
    double const& soil_saturation_capacity_2;
    double const& soil_wilting_point_2;
    double const& soil_field_capacity_2;
    double const& soil_water_content_2;
    double const& deltaS_2;

    // Inputs for layer 3
    double const& soil_depth_3;
    double const& soil_saturation_capacity_3;
    double const& soil_wilting_point_3;
    double const& soil_field_capacity_3;
    double const& soil_water_content_3;
    double const& deltaS_3;

    // Inputs for layer 4
    double const& soil_depth_4;
    double const& soil_saturation_capacity_4;
    double const& soil_wilting_point_4;
    double const& soil_field_capacity_4;
    double const& soil_water_content_4;
    double const& deltaS_4;

    // Inputs for layer 5
    double const& soil_depth_5;
    double const& soil_saturation_capacity_5;
    double const& soil_wilting_point_5;
    double const& soil_field_capacity_5;
    double const& soil_water_content_5;
    double const& deltaS_5;

    // Inputs for layer 6
    double const& soil_depth_6;
    double const& soil_saturation_capacity_6;
    double const& soil_wilting_point_6;
    double const& soil_field_capacity_6;
    double const& soil_water_content_6;
    double const& deltaS_6;

    // Pointers to output quantities
    double* upflow_1_op;
    double* upflow_2_op;
    double* upflow_3_op;
    double* upflow_4_op;
    double* upflow_5_op;
    double* upflow_6_op;

    double* deltaU_1_op;
    double* deltaU_2_op;
    double* deltaU_3_op;
    double* deltaU_4_op;
    double* deltaU_5_op;
    double* deltaU_6_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_water_upflow::get_inputs()
{
    return {
        "soil_depth_1",                // cm. Thickness of soil layer 1
        "soil_saturation_capacity_1",  // m^3 / m^3
        "soil_wilting_point_1",        // m^3 / m^3
        "soil_field_capacity_1",       // m^3 / m^3
        "soil_water_content_1",        // m^3 / m^3
        "deltaS_1",                    // m^3 / m^3. Change in soil water content due to downward flow in layer 1.
        "soil_depth_2",                // cm. Thickness of soil layer 2
        "soil_saturation_capacity_2",  // m^3 / m^3
        "soil_wilting_point_2",        // m^3 / m^3
        "soil_field_capacity_2",       // m^3 / m^3
        "soil_water_content_2",        // m^3 / m^3
        "deltaS_2",                    // m^3 / m^3. Change in soil water content due to downward flow in layer 2.
        "soil_depth_3",                // cm. Thickness of soil layer 3
        "soil_saturation_capacity_3",  // m^3 / m^3
        "soil_wilting_point_3",        // m^3 / m^3
        "soil_field_capacity_3",       // m^3 / m^3
        "soil_water_content_3",        // m^3 / m^3
        "deltaS_3",                    // m^3 / m^3. Change in soil water content due to downward flow in layer 3.
        "soil_depth_4",                // cm. Thickness of soil layer 4
        "soil_saturation_capacity_4",  // m^3 / m^3
        "soil_wilting_point_4",        // m^3 / m^3
        "soil_field_capacity_4",       // m^3 / m^3
        "soil_water_content_4",        // m^3 / m^3
        "deltaS_4",                    // m^3 / m^3. Change in soil water content due to downward flow in layer 4.
        "soil_depth_5",                // cm. Thickness of soil layer 5
        "soil_saturation_capacity_5",  // m^3 / m^3
        "soil_wilting_point_5",        // m^3 / m^3
        "soil_field_capacity_5",       // m^3 / m^3
        "soil_water_content_5",        // m^3 / m^3
        "deltaS_5",                    // m^3 / m^3. Change in soil water content due to downward flow in layer 5.
        "soil_depth_6",                // cm. Thickness of soil layer 6
        "soil_saturation_capacity_6",  // m^3 / m^3
        "soil_wilting_point_6",        // m^3 / m^3
        "soil_field_capacity_6",       // m^3 / m^3
        "soil_water_content_6",        // m^3 / m^3
        "deltaS_6"                     // m^3 / m^3. Change in soil water content due to downward flow in layer 6.
    };
}

string_vector soil_water_upflow::get_outputs()
{
    return {
        "upflow_1",  // cm / hr. Upward flow of water
        "upflow_2",  // cm / hr
        "upflow_3",  // cm / hr
        "upflow_4",  // cm / hr
        "upflow_5",  // cm / hr
        "upflow_6",  // cm / hr
        "deltaU_1",  // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 1
        "deltaU_2",  // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 2
        "deltaU_3",  // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 3
        "deltaU_4",  // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 4
        "deltaU_5",  // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 5
        "deltaU_6"   // m^3 / m^3. Change in soil water content due to evaporation and/or upward flow in layer 6
    };
}

void soil_water_upflow::do_operation() const
{
    int constexpr nlayers = 6;
    double constexpr timestep = 1;  // hr

    double soil_depth[] = {
        soil_depth_1,  //cm
        soil_depth_2,  //cm
        soil_depth_3,  //cm
        soil_depth_4,  //cm
        soil_depth_5,  //cm
        soil_depth_6   //cm
    };

    double soil_saturation_capacity[] = {
        soil_saturation_capacity_1,  // m^3 / m^3
        soil_saturation_capacity_2,  // m^3 / m^3
        soil_saturation_capacity_3,  // m^3 / m^3
        soil_saturation_capacity_4,  // m^3 / m^3
        soil_saturation_capacity_5,  // m^3 / m^3
        soil_saturation_capacity_6   // m^3 / m^3
    };

    double soil_wilting_point[] = {
        soil_wilting_point_1,  // m^3 / m^3
        soil_wilting_point_2,  // m^3 / m^3
        soil_wilting_point_3,  // m^3 / m^3
        soil_wilting_point_4,  // m^3 / m^3
        soil_wilting_point_5,  // m^3 / m^3
        soil_wilting_point_6   // m^3 / m^3
    };

    double soil_field_capacity[] = {
        soil_field_capacity_1,  // m^3 / m^3
        soil_field_capacity_2,  // m^3 / m^3
        soil_field_capacity_3,  // m^3 / m^3
        soil_field_capacity_4,  // m^3 / m^3
        soil_field_capacity_5,  // m^3 / m^3
        soil_field_capacity_6   // m^3 / m^3
    };

    double soil_water_content[] = {
        soil_water_content_1,  // m^3 / m^3
        soil_water_content_2,  // m^3 / m^3
        soil_water_content_3,  // m^3 / m^3
        soil_water_content_4,  // m^3 / m^3
        soil_water_content_5,  // m^3 / m^3
        soil_water_content_6   // m^3 / m^3
    };

    double swdelts[] = {
        deltaS_1,  // m^3 / m^3
        deltaS_2,  // m^3 / m^3
        deltaS_3,  // m^3 / m^3
        deltaS_4,  // m^3 / m^3
        deltaS_5,  // m^3 / m^3
        deltaS_6   // m^3 / m^3
    };

    double sw_avail[nlayers];  // m^3 / m^3
    for (int l = 0; l < nlayers; l++) {
        sw_avail[l] = std::max(0.0, soil_water_content[l] + swdelts[l]);  // m^3 / m^3
    }

    // Calculate upward movement of water due to evaporation and root
    // extraction for each soil layer.
    upwardFlo_str const upFlow = up_flow(
        nlayers,
        sw_avail,
        soil_depth,
        soil_saturation_capacity,
        soil_wilting_point,
        soil_field_capacity,
        soil_water_content,
        timestep);

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
