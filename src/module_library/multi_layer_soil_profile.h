#ifndef MULTI_LAYER_SOIL_PROFILE_H
#define MULTI_LAYER_SOIL_PROFILE_H

#include <cmath>  // for std::fmax
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class multi_layer_soil_profile
 *
 */

class multi_layer_soil_profile : public differential_module

{
   public:
    multi_layer_soil_profile(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities

          // soil_reflectance{get_input(input_quantities, "soil_reflectance")}, //Albedo
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},
          surface_runoff{get_input(input_quantities, "surface_runoff")},

          // Parameters for layer 1
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          deltaS_1{get_input(input_quantities, "deltaS_1")},
          deltaU_1{get_input(input_quantities, "deltaU_1")},
          deltaT_1{get_input(input_quantities, "deltaT_1")},
          uptake_layer_1{get_input(input_quantities, "uptake_layer_1")},

          // Parameters for layer 2
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          deltaS_2{get_input(input_quantities, "deltaS_2")},
          deltaU_2{get_input(input_quantities, "deltaU_2")},
          deltaT_2{get_input(input_quantities, "deltaT_2")},
          uptake_layer_2{get_input(input_quantities, "uptake_layer_2")},

          // Parameters for layer 3
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          deltaS_3{get_input(input_quantities, "deltaS_3")},
          deltaU_3{get_input(input_quantities, "deltaU_3")},
          deltaT_3{get_input(input_quantities, "deltaT_3")},
          uptake_layer_3{get_input(input_quantities, "uptake_layer_3")},

          // Parameters for layer 4
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          deltaS_4{get_input(input_quantities, "deltaS_4")},
          deltaU_4{get_input(input_quantities, "deltaU_4")},
          deltaT_4{get_input(input_quantities, "deltaT_4")},
          uptake_layer_4{get_input(input_quantities, "uptake_layer_4")},

          // Parameters for layer 5
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          deltaS_5{get_input(input_quantities, "deltaS_5")},
          deltaU_5{get_input(input_quantities, "deltaU_5")},
          deltaT_5{get_input(input_quantities, "deltaT_5")},
          uptake_layer_5{get_input(input_quantities, "uptake_layer_5")},

          // Parameters for layer 6
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          deltaS_6{get_input(input_quantities, "deltaS_6")},
          deltaU_6{get_input(input_quantities, "deltaU_6")},
          deltaT_6{get_input(input_quantities, "deltaT_6")},
          uptake_layer_6{get_input(input_quantities, "uptake_layer_6")},

          // Get pointers to output quantities - Change in water content of each layer
          soil_water_content_1_op{get_op(output_quantities, "soil_water_content_1")},
          soil_water_content_2_op{get_op(output_quantities, "soil_water_content_2")},
          soil_water_content_3_op{get_op(output_quantities, "soil_water_content_3")},
          soil_water_content_4_op{get_op(output_quantities, "soil_water_content_4")},
          soil_water_content_5_op{get_op(output_quantities, "soil_water_content_5")},
          soil_water_content_6_op{get_op(output_quantities, "soil_water_content_6")}

    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "multi_layer_soil_profile"; }

   private:
    // References to input quantities
    // double const& soil_reflectance;
    double const& soil_evaporation_rate;
    double const& surface_runoff;

    // Parameters for layer 1
    double const& soil_depth_1;
    double const& soil_water_content_1;
    double const& deltaS_1;
    double const& deltaU_1;
    double const& deltaT_1;
    double const& uptake_layer_1;

    // Parameters for layer 2
    double const& soil_depth_2;
    double const& soil_water_content_2;
    double const& deltaS_2;
    double const& deltaU_2;
    double const& deltaT_2;
    double const& uptake_layer_2;

    // Parameters for layer 3
    double const& soil_depth_3;
    double const& soil_water_content_3;
    double const& deltaS_3;
    double const& deltaU_3;
    double const& deltaT_3;
    double const& uptake_layer_3;

    // Parameters for layer 4
    double const& soil_depth_4;
    double const& soil_water_content_4;
    double const& deltaS_4;
    double const& deltaU_4;
    double const& deltaT_4;
    double const& uptake_layer_4;

    // Parameters for layer 5
    double const& soil_depth_5;
    double const& soil_water_content_5;
    double const& deltaS_5;
    double const& deltaU_5;
    double const& deltaT_5;
    double const& uptake_layer_5;

    // Parameters for layer 6
    double const& soil_depth_6;
    double const& soil_water_content_6;
    double const& deltaS_6;
    double const& deltaU_6;
    double const& deltaT_6;
    double const& uptake_layer_6;

    // Pointers to output parameters
    double* soil_water_content_1_op;
    double* soil_water_content_2_op;
    double* soil_water_content_3_op;
    double* soil_water_content_4_op;
    double* soil_water_content_5_op;
    double* soil_water_content_6_op;

    // Main operation
    void do_operation() const;
};

string_vector multi_layer_soil_profile::get_inputs()
{
    return {
        // "soil_reflectance",
        "soil_evaporation_rate",  // Mg/ha/hr
        "surface_runoff",

        "soil_depth_1",
        "soil_water_content_1",
        "deltaS_1",
        "deltaU_1",
        "deltaT_1",
        "uptake_layer_1",

        "soil_depth_2",
        "soil_water_content_2",
        "deltaS_2",
        "deltaU_2",
        "deltaT_2",
        "uptake_layer_2",

        "soil_depth_3",
        "soil_water_content_3",
        "deltaS_3",
        "deltaU_3",
        "deltaT_3",
        "uptake_layer_3",

        "soil_depth_4",
        "soil_water_content_4",
        "deltaS_4",
        "deltaU_4",
        "deltaT_4",
        "uptake_layer_4",

        "soil_depth_5",
        "soil_water_content_5",
        "deltaS_5",
        "deltaU_5",
        "deltaT_5",
        "uptake_layer_5",

        "soil_depth_6",
        "soil_water_content_6",
        "deltaS_6",
        "deltaU_6",
        "deltaT_6",
        "uptake_layer_6",
    };
}

string_vector multi_layer_soil_profile::get_outputs()
{
    return {
        "soil_water_content_1",
        "soil_water_content_2",
        "soil_water_content_3",
        "soil_water_content_4",
        "soil_water_content_5",
        "soil_water_content_6"};
}

void multi_layer_soil_profile::do_operation() const
{
    int nlayers = 6;
    double soil_depth[] = {
        soil_depth_1,
        soil_depth_2,
        soil_depth_3,
        soil_depth_4,
        soil_depth_5,
        soil_depth_6};
    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    double swdeltS[] = {
        deltaS_1,
        deltaS_2,
        deltaS_3,
        deltaS_4,
        deltaS_5,
        deltaS_6};
    double swdeltU[] = {
        deltaU_1,
        deltaU_2,
        deltaU_3,
        deltaU_4,
        deltaU_5,
        deltaU_6};
    double swdeltT[] = {
        deltaT_1,
        deltaT_2,
        deltaT_3,
        deltaT_4,
        deltaT_5,
        deltaT_6};
    double uptake[] = {
        uptake_layer_1,
        uptake_layer_2,
        uptake_layer_3,
        uptake_layer_4,
        uptake_layer_5,
        uptake_layer_6};

    // Calculate total change in soil water content
    double delta_soil_water_content[nlayers];
    //tttt
    for (int l = 0; l < nlayers; l++) {
        // adding uptake because value is negative
        delta_soil_water_content[l] = swdeltS[l] + swdeltU[l] + swdeltT[l] + (uptake[l] / (100 * soil_depth[l]));
        if (soil_water_content[l] + delta_soil_water_content[l] < 0) {
            delta_soil_water_content[l] = -soil_water_content[l];
        }
    }

    // Mg/ha/hr
    // double soil_evap = soil_evaporation_rate/ 10.0; // Mg/ha/hr to mm/hr
    // remove soil evaporation from first layer, but don't let water content go
    // negative. This is a crude, and hopefully temporary, fix. -mlm
    // soil_evaporation_rate from the new soil evapo function is in mm/hr? -YH
    delta_soil_water_content[0] = delta_soil_water_content[0] - soil_evaporation_rate / (10.0 * soil_depth[0]);

    update(soil_water_content_1_op, delta_soil_water_content[0]);
    update(soil_water_content_2_op, delta_soil_water_content[1]);
    update(soil_water_content_3_op, delta_soil_water_content[2]);
    update(soil_water_content_4_op, delta_soil_water_content[3]);
    update(soil_water_content_5_op, delta_soil_water_content[4]);
    update(soil_water_content_6_op, delta_soil_water_content[5]);
}
}  // namespace standardBML
#endif
