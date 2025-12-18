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

          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},

          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},

          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},

          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},

          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},

          // pointers to output quantities
          uptake_layer_1_op{get_op(output_quantities, "uptake_layer_1")},
          uptake_layer_2_op{get_op(output_quantities, "uptake_layer_2")},
          uptake_layer_3_op{get_op(output_quantities, "uptake_layer_3")},
          uptake_layer_4_op{get_op(output_quantities, "uptake_layer_4")},
          uptake_layer_5_op{get_op(output_quantities, "uptake_layer_5")},
          uptake_layer_6_op{get_op(output_quantities, "uptake_layer_6")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_uptake"; }

   private:
    // references to input quantities

    double const& soil_depth_1;
    double const& soil_water_content_1;

    double const& soil_depth_2;
    double const& soil_water_content_2;

    double const& soil_depth_3;
    double const& soil_water_content_3;

    double const& soil_depth_4;
    double const& soil_water_content_4;

    double const& soil_depth_5;
    double const& soil_water_content_5;

    double const& soil_depth_6;
    double const& soil_water_content_6;

    double const& max_rooting_layer;
    double const& canopy_transpiration_rate;  // Mg/ha/hr

    // pointers to output quantities
    double* uptake_layer_1_op;
    double* uptake_layer_2_op;
    double* uptake_layer_3_op;
    double* uptake_layer_4_op;
    double* uptake_layer_5_op;
    double* uptake_layer_6_op;

    // main operation
    void do_operation() const;
};

string_vector soil_water_uptake::get_inputs()
{
    return {

        "soil_depth_1",
        "soil_water_content_1",

        "soil_depth_2",
        "soil_water_content_2",

        "soil_depth_3",
        "soil_water_content_3",

        "soil_depth_4",
        "soil_water_content_4",

        "soil_depth_5",
        "soil_water_content_5",

        "soil_depth_6",
        "soil_water_content_6",

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
        "uptake_layer_6"

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

    // initialize root depth to be 0
    double root_depth = 0;

    // get total root depth
    for (int i = 0; i < max_rooting_layer; i++) {
        root_depth = root_depth + soil_depth[i];  // cm
    }

    // uptake in layer = ET*(depth of layer/total depth of root zone)
    // add constraint if soil_water_content_layer < uptake
    for (int i = 0; i < max_rooting_layer; i++) {
        uptake[i] = -canopy_transpiration_rate * (soil_depth[i] / root_depth);  // (Mg*hr/ha)*(cm/cm) = Mg/ha/hr
    }

    // update uptake
    update(uptake_layer_1_op, uptake[0]);
    update(uptake_layer_2_op, uptake[1]);
    update(uptake_layer_3_op, uptake[2]);
    update(uptake_layer_4_op, uptake[3]);
    update(uptake_layer_5_op, uptake[4]);
    update(uptake_layer_6_op, uptake[5]);
}

}  // namespace standardBML
#endif
