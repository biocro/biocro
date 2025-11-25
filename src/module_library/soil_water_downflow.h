#ifndef SOIL_WATER_DOWNFLOW_H
#define SOIL_WATER_DOWNFLOW_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"

namespace standardBML 
{
/**
 * @class soil_water_downflow
 *
 * @brief Calculates water flow through the soil.
 *
 */
class soil_water_downflow : public direct_module
{
   public:
    soil_water_downflow(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          available_water{get_input(input_quantities, "available_water")},
          surface_runoff{get_input(input_quantities, "surface_runoff")},
          swcon{get_input(input_quantities, "swcon")},

          // Inputs for layer 1
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_saturated_conductivity_1{get_input(input_quantities, "soil_saturated_conductivity_1")},
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},
          soil_field_capacity_1{get_input(input_quantities, "soil_field_capacity_1")},

          // Inputs for layer 2
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_saturated_conductivity_2{get_input(input_quantities, "soil_saturated_conductivity_2")},
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},
          soil_field_capacity_2{get_input(input_quantities, "soil_field_capacity_2")},

          // Inputs for layer 3
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_saturated_conductivity_3{get_input(input_quantities, "soil_saturated_conductivity_3")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},
          soil_field_capacity_3{get_input(input_quantities, "soil_field_capacity_3")},

          // Inputs for layer 4
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_saturated_conductivity_4{get_input(input_quantities, "soil_saturated_conductivity_4")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},
          soil_field_capacity_4{get_input(input_quantities, "soil_field_capacity_4")},

          // Inputs for layer 5
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_saturated_conductivity_5{get_input(input_quantities, "soil_saturated_conductivity_5")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},
          soil_field_capacity_5{get_input(input_quantities, "soil_field_capacity_5")},

          // Inputs for layer 6
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_saturated_conductivity_6{get_input(input_quantities, "soil_saturated_conductivity_6")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},
          soil_field_capacity_6{get_input(input_quantities, "soil_field_capacity_6")},

          // Get pointers to output quantities
          infiltrated_water_op{get_op(output_quantities, "infiltrated_water")},
          potential_infiltration_op{get_op(output_quantities, "potential_infiltration")},
          excess_water_op{get_op(output_quantities, "excess_water")},
          drain_op{get_op(output_quantities, "drain")},

          deltaS_1_op{get_op(output_quantities, "deltaS_1")},
          deltaS_2_op{get_op(output_quantities, "deltaS_2")},
          deltaS_3_op{get_op(output_quantities, "deltaS_3")},
          deltaS_4_op{get_op(output_quantities, "deltaS_4")},
          deltaS_5_op{get_op(output_quantities, "deltaS_5")},
          deltaS_6_op{get_op(output_quantities, "deltaS_6")},

          drn_1_op{get_op(output_quantities, "drn_1")},
          drn_2_op{get_op(output_quantities, "drn_2")},
          drn_3_op{get_op(output_quantities, "drn_3")},
          drn_4_op{get_op(output_quantities, "drn_4")},
          drn_5_op{get_op(output_quantities, "drn_5")},
          drn_6_op{get_op(output_quantities, "drn_6")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_downflow"; }

   private:
    // References to input quantities
    double const& available_water;
    double const& surface_runoff;
    double const& swcon;

    // Inputs for layer 1
    double const& soil_depth_1;
    double const& soil_water_content_1;
    double const& soil_saturated_conductivity_1;
    double const& soil_saturation_capacity_1;
    double const& soil_field_capacity_1;

    // Inputs for layer 2
    double const& soil_depth_2;
    double const& soil_water_content_2;
    double const& soil_saturated_conductivity_2;
    double const& soil_saturation_capacity_2;
    double const& soil_field_capacity_2;

    // Inputs for layer 3
    double const& soil_depth_3;
    double const& soil_water_content_3;
    double const& soil_saturated_conductivity_3;
    double const& soil_saturation_capacity_3;
    double const& soil_field_capacity_3;

    // Inputs for layer 4
    double const& soil_depth_4;
    double const& soil_water_content_4;
    double const& soil_saturated_conductivity_4;
    double const& soil_saturation_capacity_4;
    double const& soil_field_capacity_4;

    // Inputs for layer 5
    double const& soil_depth_5;
    double const& soil_water_content_5;
    double const& soil_saturated_conductivity_5;
    double const& soil_saturation_capacity_5;
    double const& soil_field_capacity_5;

    // Inputs for layer 6
    double const& soil_depth_6;
    double const& soil_water_content_6;
    double const& soil_saturated_conductivity_6;
    double const& soil_saturation_capacity_6;
    double const& soil_field_capacity_6;

    // Pointers to output quantities
    double* infiltrated_water_op;
    double* potential_infiltration_op;
    double* excess_water_op;
    double* drain_op;

    double* deltaS_1_op;
    double* deltaS_2_op;
    double* deltaS_3_op;
    double* deltaS_4_op;
    double* deltaS_5_op;
    double* deltaS_6_op;

    double* drn_1_op;
    double* drn_2_op;
    double* drn_3_op;
    double* drn_4_op;
    double* drn_5_op;
    double* drn_6_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_water_downflow::get_inputs()
{
    return {
        "available_water",  // Water available for infiltration or runoff (rainfall plus irrigation) (mm/hr)
        "surface_runoff",   // Calculated runoff per hour (mm)
        "swcon",            // Soil water conductivity constant; whole profile drainage rate coefficient (1/hr)

        "soil_depth_1",
        "soil_water_content_1",
        "soil_saturated_conductivity_1",
        "soil_saturation_capacity_1",
        "soil_field_capacity_1",

        "soil_depth_2",
        "soil_water_content_2",
        "soil_saturated_conductivity_2",
        "soil_saturation_capacity_2",
        "soil_field_capacity_2",

        "soil_depth_3",
        "soil_water_content_3",
        "soil_saturated_conductivity_3",
        "soil_saturation_capacity_3",
        "soil_field_capacity_3",

        "soil_depth_4",
        "soil_water_content_4",
        "soil_saturated_conductivity_4",
        "soil_saturation_capacity_4",
        "soil_field_capacity_4",

        "soil_depth_5",
        "soil_water_content_5",
        "soil_saturated_conductivity_5",
        "soil_saturation_capacity_5",
        "soil_field_capacity_5",

        "soil_depth_6",
        "soil_water_content_6",
        "soil_saturated_conductivity_6",
        "soil_saturation_capacity_6",
        "soil_field_capacity_6"  //
    };
}

string_vector soil_water_downflow::get_outputs()
{
    return {
        "infiltrated_water",       // Water available for infiltration - rainfall minus runoff plus net irrigation (mm / hr)
        "potential_infiltration",  // Potential water available for infiltration (cm)
        "excess_water",            // Excess water to be added to runoff (cm/hr)
        "drain",                   // Drainage rate from soil profile (mm/hr)

        "deltaS_1",  // Change in soil water content due to drainage in layer 1 (cm3 [water] / cm3 [soil])
        "deltaS_2",  // Change in soil water content due to drainage in layer 2 (cm3 [water] / cm3 [soil])
        "deltaS_3",  // Change in soil water content due to drainage in layer 3 (cm3 [water] / cm3 [soil])
        "deltaS_4",  // Change in soil water content due to drainage in layer 4 (cm3 [water] / cm3 [soil])
        "deltaS_5",  // Change in soil water content due to drainage in layer 5 (cm3 [water] / cm3 [soil])
        "deltaS_6",  // Change in soil water content due to drainage in layer 6 (cm3 [water] / cm3 [soil])

        "drn_1",  // Drainage rate through soil layer 1 (cm/hr)
        "drn_2",  // Drainage rate through soil layer 2 (cm/hr)
        "drn_3",  // Drainage rate through soil layer 3 (cm/hr)
        "drn_4",  // Drainage rate through soil layer 4 (cm/hr)
        "drn_5",  // Drainage rate through soil layer 5 (cm/hr)
        "drn_6"   // Drainage rate through soil layer 6 (cm/hr)
    };
}

void soil_water_downflow::do_operation() const
{
    int nlayers = 6;

    double soil_depth[] = {
        soil_depth_1,
        soil_depth_2,
        soil_depth_3,
        soil_depth_4,
        soil_depth_5,
        soil_depth_6};

    double soil_saturation_capacity[] = {
        soil_saturation_capacity_1,
        soil_saturation_capacity_2,
        soil_saturation_capacity_3,
        soil_saturation_capacity_4,
        soil_saturation_capacity_5,
        soil_saturation_capacity_6};

    double soil_field_capacity[] = {
        soil_field_capacity_1,
        soil_field_capacity_2,
        soil_field_capacity_3,
        soil_field_capacity_4,
        soil_field_capacity_5,
        soil_field_capacity_6};

    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    double soil_sat_conductivity_1 =  3620.1 * soil_saturated_conductivity_1 - 0.0104; // Units conversion from J s m^-3 to cm/hr
    double soil_sat_conductivity_2 =  3620.1 * soil_saturated_conductivity_2 - 0.0104; // The data was plotted with values in both units to obtain a linear realationship
    double soil_sat_conductivity_3 =  3620.1 * soil_saturated_conductivity_3 - 0.0104; // Y = 3620.1 * X - 0.0104
    double soil_sat_conductivity_4 =  3620.1 * soil_saturated_conductivity_4 - 0.0104;
    double soil_sat_conductivity_5 =  3620.1 * soil_saturated_conductivity_5 - 0.0104;
    double soil_sat_conductivity_6 =  3620.1 * soil_saturated_conductivity_6 - 0.0104;   

    double soil_saturated_conductivity[] = {
        soil_sat_conductivity_1, 
        soil_sat_conductivity_2, 
        soil_sat_conductivity_3, 
        soil_sat_conductivity_4,
        soil_sat_conductivity_5,
        soil_sat_conductivity_6};


    double infiltrated_water = available_water - surface_runoff;    
    // Potential for infilitration
    double potential_infiltration = infiltrated_water * 0.1;  // mm to cm

    infilWater_str infilWater;

    // Call INFIL to calculate infiltration rates on days with irrigation or rainfall.
    // Call SATFLO on days with no irrigation or rain to calculate saturated flow.
    // double new_surface_runoff;
    if (potential_infiltration > 0.0001) {
        infilWater = infil(
            nlayers,
            potential_infiltration,
            swcon,
            soil_depth,
            soil_saturation_capacity,
            soil_field_capacity,
            soil_water_content,
            soil_saturated_conductivity);
        // if(infilWater.excess_water > 0) {
        //   new_surface_runoff = surface_runoff + infilWater.excess_water * 10.0;
        // } else {
        //   new_surface_runoff = surface_runoff;
        // }
    } else {
        infilWater = satflo(
            nlayers,
            potential_infiltration,
            swcon,
            soil_depth,
            soil_saturation_capacity,
            soil_field_capacity,
            soil_water_content,
            soil_saturated_conductivity);
        // new_surface_runoff = surface_runoff;
    }

    // Update the output quantity list
    // update(surface_runoff_op, new_surface_runoff);
    update(infiltrated_water_op, infiltrated_water);
    update(potential_infiltration_op, potential_infiltration);
    update(excess_water_op, infilWater.excess_water);
    update(drain_op, infilWater.drain);

    update(deltaS_1_op, infilWater.sw_delta_S[0]);
    update(deltaS_2_op, infilWater.sw_delta_S[1]);
    update(deltaS_3_op, infilWater.sw_delta_S[2]);
    update(deltaS_4_op, infilWater.sw_delta_S[3]);
    update(deltaS_5_op, infilWater.sw_delta_S[4]);
    update(deltaS_6_op, infilWater.sw_delta_S[5]);

    update(drn_1_op, infilWater.drn[0]);
    update(drn_2_op, infilWater.drn[1]);
    update(drn_3_op, infilWater.drn[2]);
    update(drn_4_op, infilWater.drn[3]);
    update(drn_5_op, infilWater.drn[4]);
    update(drn_6_op, infilWater.drn[5]);
}

}  // namespace standardBML 
#endif
