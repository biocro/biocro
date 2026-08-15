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
 * @brief Collects all Deltas of water mass for each layer and
 * updates layer-wise soil water content.
 * The surface soil water content is particularly updated with soil evaporation.
 * This module is used with other DSSAT-based water modules,
 * but is homemade (not based on any DSSAT codes).
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

          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},

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
    double const& soil_evaporation_rate;

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
        "soil_evaporation_rate",  // Mg / ha / hr
        "soil_depth_1",           // cm
        "soil_water_content_1",   // m^3 / m^3
        "deltaS_1",               // m^3 / m^3
        "deltaU_1",               // m^3 / m^3
        "deltaT_1",               // m^3 / m^3
        "uptake_layer_1",         // Mg / ha / hr
        "soil_depth_2",           // cm
        "soil_water_content_2",   // m^3 / m^3
        "deltaS_2",               // m^3 / m^3
        "deltaU_2",               // m^3 / m^3
        "deltaT_2",               // m^3 / m^3
        "uptake_layer_2",         // Mg / ha / hr
        "soil_depth_3",           // cm
        "soil_water_content_3",   // m^3 / m^3
        "deltaS_3",               // m^3 / m^3
        "deltaU_3",               // m^3 / m^3
        "deltaT_3",               // m^3 / m^3
        "uptake_layer_3",         // Mg / ha / hr
        "soil_depth_4",           // cm
        "soil_water_content_4",   // m^3 / m^3
        "deltaS_4",               // m^3 / m^3
        "deltaU_4",               // m^3 / m^3
        "deltaT_4",               // m^3 / m^3
        "uptake_layer_4",         // Mg / ha / hr
        "soil_depth_5",           // cm
        "soil_water_content_5",   // m^3 / m^3
        "deltaS_5",               // m^3 / m^3
        "deltaU_5",               // m^3 / m^3
        "deltaT_5",               // m^3 / m^3
        "uptake_layer_5",         // Mg / ha / hr
        "soil_depth_6",           // cm
        "soil_water_content_6",   // m^3 / m^3
        "deltaS_6",               // m^3 / m^3
        "deltaU_6",               // m^3 / m^3
        "deltaT_6",               // m^3 / m^3
        "uptake_layer_6",         // Mg / ha / hr
    };
}

string_vector multi_layer_soil_profile::get_outputs()
{
    return {
        "soil_water_content_1",  // m^3 / m^3 / hr
        "soil_water_content_2",  // m^3 / m^3 / hr
        "soil_water_content_3",  // m^3 / m^3 / hr
        "soil_water_content_4",  // m^3 / m^3 / hr
        "soil_water_content_5",  // m^3 / m^3 / hr
        "soil_water_content_6"   // m^3 / m^3 / hr
    };
}

void multi_layer_soil_profile::do_operation() const
{
    int constexpr nlayers = 6;

    double soil_depth[] = {
        soil_depth_1,  // cm
        soil_depth_2,  // cm
        soil_depth_3,  // cm
        soil_depth_4,  // cm
        soil_depth_5,  // cm
        soil_depth_6   // cm
    };

    double soil_water_content[] = {
        soil_water_content_1,  // m^3 / m^3
        soil_water_content_2,  // m^3 / m^3
        soil_water_content_3,  // m^3 / m^3
        soil_water_content_4,  // m^3 / m^3
        soil_water_content_5,  // m^3 / m^3
        soil_water_content_6   // m^3 / m^3
    };

    double swdeltS[] = {
        deltaS_1,  // m^3 / m^3/hr
        deltaS_2,  // m^3 / m^3/hr
        deltaS_3,  // m^3 / m^3/hr
        deltaS_4,  // m^3 / m^3/hr
        deltaS_5,  // m^3 / m^3/hr
        deltaS_6   // m^3 / m^3/hr
    };

    double swdeltU[] = {
        deltaU_1,  // m^3 / m^3/hr
        deltaU_2,  // m^3 / m^3/hr
        deltaU_3,  // m^3 / m^3/hr
        deltaU_4,  // m^3 / m^3/hr
        deltaU_5,  // m^3 / m^3/hr
        deltaU_6   // m^3 / m^3/hr
    };

    double swdeltT[] = {
        deltaT_1,  // m^3 / m^3/hr
        deltaT_2,  // m^3 / m^3/hr
        deltaT_3,  // m^3 / m^3/hr
        deltaT_4,  // m^3 / m^3/hr
        deltaT_5,  // m^3 / m^3/hr
        deltaT_6   // m^3 / m^3/hr
    };

    double uptake[] = {
        uptake_layer_1,  // Mg / ha / hr
        uptake_layer_2,  // Mg / ha / hr
        uptake_layer_3,  // Mg / ha / hr
        uptake_layer_4,  // Mg / ha / hr
        uptake_layer_5,  // Mg / ha / hr
        uptake_layer_6   // Mg / ha / hr
    };

    constexpr double cm_to_mm = 10.0;
    constexpr double MG_HA_to_mm = 0.1;  // Mg/ha of water = 0.1 mm
    constexpr double timestep = 1;       // hr
    // Calculate total change in soil water content
    double delta_soil_water_content[nlayers];  // m^3 / m^3
    for (int l = 0; l < nlayers; l++) {
        // adding uptake because value is negative
        delta_soil_water_content[l] = swdeltS[l] + swdeltU[l] + swdeltT[l] + (uptake[l] * timestep * MG_HA_to_mm / (cm_to_mm * soil_depth[l]));  // m^3 / m^3
        if (soil_water_content[l] + delta_soil_water_content[l] < 0) {
            delta_soil_water_content[l] = -soil_water_content[l];
        }
    }

    // convert evaporation to water cotent: (E(Mg / ha / hr)*MG_HA_to_mm)/depth(mm)
    const double evaporated_water_cotent = (soil_evaporation_rate * timestep * MG_HA_to_mm) / (soil_depth[0] * cm_to_mm);  // m^3 / m^3
    delta_soil_water_content[0] = delta_soil_water_content[0] - evaporated_water_cotent;

    update(soil_water_content_1_op, delta_soil_water_content[0] / timestep);  // m^3 / m^3 / hr
    update(soil_water_content_2_op, delta_soil_water_content[1] / timestep);  // m^3 / m^3 / hr
    update(soil_water_content_3_op, delta_soil_water_content[2] / timestep);  // m^3 / m^3 / hr
    update(soil_water_content_4_op, delta_soil_water_content[3] / timestep);  // m^3 / m^3 / hr
    update(soil_water_content_5_op, delta_soil_water_content[4] / timestep);  // m^3 / m^3 / hr
    update(soil_water_content_6_op, delta_soil_water_content[5] / timestep);  // m^3 / m^3 / hr
}
}  // namespace standardBML
#endif
