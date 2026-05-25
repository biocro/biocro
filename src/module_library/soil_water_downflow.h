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
 * * Based on DSSAT source files:
 *   - Soil/SoilWater/WATBAL.for, subroutine WATBAL
 *     computation of infiltrated_water (WINF) and potential_infiltration (PINF)
 *     from available_water (WATAVL) and surface_runoff (RUNOFF)
 *   - Soil/SoilWater/INFIL.for, subroutine INFIL
 *     layer-by-layer infiltration and drainage
 * Note: DSSAT rates are on a daily timestep (mm/d, cm/d);
 *       BioCro rates are on a hourly timestep (mm/hr, cm/hr).
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
          surface_runoff_rate{get_input(input_quantities, "surface_runoff_rate")},
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
          excess_water_rate_op{get_op(output_quantities, "excess_water_rate")},
          drainage_rate_op{get_op(output_quantities, "drainage_rate")},

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
    double const& surface_runoff_rate;
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
    double* excess_water_rate_op;
    double* drainage_rate_op;

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
        "available_water",                // mm / hr. Water available for infiltration or runoff (rainfall plus irrigation)
        "surface_runoff_rate",            // Mg / ha / hr
        "swcon",                          // 1 / hr. Soil water conductivity constant; whole profile drainage rate coefficient
        "soil_depth_1",                   // cm
        "soil_water_content_1",           // m^3 / m^3
        "soil_saturated_conductivity_1",  // kg s / m^3
        "soil_saturation_capacity_1",     // m^3 / m^3
        "soil_field_capacity_1",          // m^3 / m^3
        "soil_depth_2",                   // cm
        "soil_water_content_2",           // m^3 / m^3
        "soil_saturated_conductivity_2",  // kg s / m^3
        "soil_saturation_capacity_2",     // m^3 / m^3
        "soil_field_capacity_2",          // m^3 / m^3
        "soil_depth_3",                   // cm.
        "soil_water_content_3",           // m^3 / m^3
        "soil_saturated_conductivity_3",  // kg s / m^3
        "soil_saturation_capacity_3",     // m^3 / m^3
        "soil_field_capacity_3",          // m^3 / m^3
        "soil_depth_4",                   // cm
        "soil_water_content_4",           // m^3 / m^3
        "soil_saturated_conductivity_4",  // kg s / m^3
        "soil_saturation_capacity_4",     // m^3 / m^3
        "soil_field_capacity_4",          // m^3 / m^3
        "soil_depth_5",                   // cm
        "soil_water_content_5",           // m^3 / m^3
        "soil_saturated_conductivity_5",  // kg s / m^3
        "soil_saturation_capacity_5",     // m^3 / m^3
        "soil_field_capacity_5",          // m^3 / m^3
        "soil_depth_6",                   // cm
        "soil_water_content_6",           // m^3 / m^3
        "soil_saturated_conductivity_6",  // kg s / m^3
        "soil_saturation_capacity_6",     // m^3 / m^3
        "soil_field_capacity_6"           // m^3 / m^3
    };
}

string_vector soil_water_downflow::get_outputs()
{
    return {
        "infiltrated_water",  // mm / hr. Water available for infiltration - rainfall minus runoff plus net irrigation
        "excess_water_rate",  // Mg / ha / hr. Excess water to be added to runoff
        "drainage_rate",      // Mg / ha / hr. Drainage rate from soil profile.
        "deltaS_1",           // m^3 / m^3. Change in soil water content due to drainage in layer 1
        "deltaS_2",           // m^3 / m^3. Change in soil water content due to drainage in layer 2
        "deltaS_3",           // m^3 / m^3. Change in soil water content due to drainage in layer 3
        "deltaS_4",           // m^3 / m^3. Change in soil water content due to drainage in layer 4
        "deltaS_5",           // m^3 / m^3. Change in soil water content due to drainage in layer 5
        "deltaS_6",           // m^3 / m^3. Change in soil water content due to drainage in layer 6
        "drn_1",              // cm / hr. Drainage rate through soil layer 1
        "drn_2",              // cm / hr. Drainage rate through soil layer 2
        "drn_3",              // cm / hr. Drainage rate through soil layer 3
        "drn_4",              // cm / hr. Drainage rate through soil layer 4
        "drn_5",              // cm / hr. Drainage rate through soil layer 5
        "drn_6"               // cm / hr. Drainage rate through soil layer 6
    };
}

void soil_water_downflow::do_operation() const
{
    // Define hard-coded parameter values
    double constexpr eps_sw = 0.0001;       // cm - small threshold value of soil water
    double constexpr mm_to_cm = 0.1;        // cm / mm
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm
    double constexpr timestep = 1.0;        // hr
    int nlayers = 6;

    double soil_depth[] = {
        soil_depth_1,  // cm
        soil_depth_2,  // cm
        soil_depth_3,  // cm
        soil_depth_4,  // cm
        soil_depth_5,  // cm
        soil_depth_6   // cm
    };

    double soil_saturation_capacity[] = {
        soil_saturation_capacity_1,  // m^3 / m^3
        soil_saturation_capacity_2,  // m^3 / m^3
        soil_saturation_capacity_3,  // m^3 / m^3
        soil_saturation_capacity_4,  // m^3 / m^3
        soil_saturation_capacity_5,  // m^3 / m^3
        soil_saturation_capacity_6   // m^3 / m^3
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

    // Convert saturated conductivity from kg s / m^3 to cm / hr
    // The data was plotted with values in both units to obtain a linear relationship
    // cm / hr = 3620.1 * (kg s / m^3) - 0.0104
    double constexpr kg_s_per_m3_to_cm_per_hr = 3620.1;  // (cm / hr) / (kg s / m^3)
    double constexpr sat_conductivity_intercept = 0.0104;
    double soil_sat_conductivity_1 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_1 - sat_conductivity_intercept;  // cm / hr
    double soil_sat_conductivity_2 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_2 - sat_conductivity_intercept;  // cm / hr
    double soil_sat_conductivity_3 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_3 - sat_conductivity_intercept;  // cm / hr
    double soil_sat_conductivity_4 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_4 - sat_conductivity_intercept;  // cm / hr
    double soil_sat_conductivity_5 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_5 - sat_conductivity_intercept;  // cm / hr
    double soil_sat_conductivity_6 = kg_s_per_m3_to_cm_per_hr * soil_saturated_conductivity_6 - sat_conductivity_intercept;  // cm / hr

    double soil_saturated_conductivity[] = {
        soil_sat_conductivity_1,  // cm / hr
        soil_sat_conductivity_2,  // cm / hr
        soil_sat_conductivity_3,  // cm / hr
        soil_sat_conductivity_4,  // cm / hr
        soil_sat_conductivity_5,  // cm / hr
        soil_sat_conductivity_6   // cm / hr
    };

    double infiltrated_water =
        available_water - surface_runoff_rate / mm_to_Mg_per_ha;  // mm / hr

    // Convert units
    double const infiltrated_water_cm = infiltrated_water * mm_to_cm;  // cm / hr

    infilWater_str infilWater;

    // Call INFIL to calculate infiltration rates on days with irrigation or rainfall.
    // Call SATFLO on days with no irrigation or rain to calculate saturated flow.
    if (infiltrated_water_cm * timestep > eps_sw) {
        infilWater = infil(
            nlayers,
            infiltrated_water_cm,
            swcon,
            soil_depth,
            soil_saturation_capacity,
            soil_field_capacity,
            soil_water_content,
            soil_saturated_conductivity,
            timestep);
    } else {
        infilWater = satflo(
            nlayers,
            swcon,
            soil_depth,
            soil_saturation_capacity,
            soil_field_capacity,
            soil_water_content,
            soil_saturated_conductivity,
            timestep);
    }

    // Update the output quantity list
    update(infiltrated_water_op, infiltrated_water);
    update(excess_water_rate_op, infilWater.excess_water_rate * mm_to_Mg_per_ha);
    update(drainage_rate_op, infilWater.overall_drainage_rate * mm_to_Mg_per_ha);

    update(deltaS_1_op, infilWater.sw_delta_S[0]);
    update(deltaS_2_op, infilWater.sw_delta_S[1]);
    update(deltaS_3_op, infilWater.sw_delta_S[2]);
    update(deltaS_4_op, infilWater.sw_delta_S[3]);
    update(deltaS_5_op, infilWater.sw_delta_S[4]);
    update(deltaS_6_op, infilWater.sw_delta_S[5]);

    update(drn_1_op, infilWater.downward_flux[0]);
    update(drn_2_op, infilWater.downward_flux[1]);
    update(drn_3_op, infilWater.downward_flux[2]);
    update(drn_4_op, infilWater.downward_flux[3]);
    update(drn_5_op, infilWater.downward_flux[4]);
    update(drn_6_op, infilWater.downward_flux[5]);
}

}  // namespace standardBML
#endif
