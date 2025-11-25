#ifndef SOIL_SURFACE_RUNOFF_H
#define SOIL_SURFACE_RUNOFF_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML 
{
/**
 * @class soil_surface_runoff
 *
 * @brief Calculates overland water runoff from the top two soil layers due to
 * inflow from precipitation and irrigation.
 *
 */
class soil_surface_runoff : public direct_module
{
   public:
    soil_surface_runoff(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          precip{get_input(input_quantities, "precip")},
          irrigation{get_input(input_quantities, "irrigation")},
          curve_number{get_input(input_quantities, "curve_number")},

          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},

          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},

          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},

          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},

          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},

          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},

          // Get pointers to output quantities
          surface_runoff_op{get_op(output_quantities, "surface_runoff")},
          soil_storage_op{get_op(output_quantities, "soil_storage")},
          pb_op{get_op(output_quantities, "pb")},
          soil_initial_abstraction_op{get_op(output_quantities, "soil_initial_abstraction")},
          available_water_op{get_op(output_quantities, "available_water")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_surface_runoff"; }

   private:
    // References to input quantities
    double const& precip;
    double const& irrigation;
    double const& curve_number;

    double const& soil_water_content_1;
    double const& soil_wilting_point_1;
    double const& soil_saturation_capacity_1;
    
    double const& soil_water_content_2;
    double const& soil_wilting_point_2;
    double const& soil_saturation_capacity_2;
    
    double const& soil_water_content_3;
    double const& soil_wilting_point_3;
    double const& soil_saturation_capacity_3;
    
    double const& soil_water_content_4;
    double const& soil_wilting_point_4;
    double const& soil_saturation_capacity_4;
    
    double const& soil_water_content_5;
    double const& soil_wilting_point_5;
    double const& soil_saturation_capacity_5;
    
    double const& soil_water_content_6;
    double const& soil_wilting_point_6;
    double const& soil_saturation_capacity_6;

    // Pointers to output quantities
    double* surface_runoff_op;
    double* soil_storage_op;
    double* pb_op;
    double* soil_initial_abstraction_op;
    double* available_water_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_surface_runoff::get_inputs()
{
    return {
        "precip",                      // Precipitation depth for current hour (mm)
        "irrigation",                  // Irrigation amount in an hour (mm/hr)
        "curve_number",                // Runoff Curve Number (unitless) - measure of runoff potential based on 
                                       //  soil type and current soil water content.
        "soil_water_content_1",
        "soil_wilting_point_1",
        "soil_saturation_capacity_1",
        
        "soil_water_content_2",
        "soil_wilting_point_2",
        "soil_saturation_capacity_2",
        
        "soil_water_content_3",
        "soil_wilting_point_3",
        "soil_saturation_capacity_3",
        
        "soil_water_content_4",
        "soil_wilting_point_4",
        "soil_saturation_capacity_4",
        
        "soil_water_content_5",
        "soil_wilting_point_5",
        "soil_saturation_capacity_5",
        
        "soil_water_content_6",
        "soil_wilting_point_6",
        "soil_saturation_capacity_6"        // Volumetric soil water content in soil layer 2 at wilting point limit (cm3 [water] / cm3 [soil])
    };
}

string_vector soil_surface_runoff::get_outputs()
{
    return {
        "surface_runoff",            // Calculated runoff (mm/hr)
        "soil_storage",              // units?
        "pb",                        // units?
        "soil_initial_abstraction",  // units?
        "available_water"            // units?
    };
}

void soil_surface_runoff::do_operation() const
{
    int nlayers = 6;
    double available_water = precip + irrigation;  // mm
    double soil_storage = 254.0 * (100.0 / curve_number - 1.0); // in to mm

    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    double soil_wilting_point[] = {
        soil_wilting_point_1,
        soil_wilting_point_2,
        soil_wilting_point_3,
        soil_wilting_point_4,
        soil_wilting_point_5,
        soil_wilting_point_6};

    double soil_saturation_capacity[] = {
        soil_saturation_capacity_1,
        soil_saturation_capacity_2,
        soil_saturation_capacity_3,
        soil_saturation_capacity_4,
        soil_saturation_capacity_5,
        soil_saturation_capacity_6}; 

    for (int l = 0; l < nlayers; l++){
        if (soil_water_content[l] < soil_wilting_point[l]){
          if (l == 0){
              double soil_water_air_dry = 0.30 * soil_wilting_point[l];
              if (soil_water_content[l] < soil_water_air_dry)
                  soil_water_content[l] = soil_water_air_dry;
          } else
              soil_water_content[l] = soil_wilting_point[l];
        }
    }       
    // Initial abstraction ratio
    // Runoff is related to the average soil water content of the top two layers of soil
    double soil_initial_abstraction =
        0.15 * ((soil_saturation_capacity[0] - soil_water_content[0]) /
                    (soil_saturation_capacity[0] - soil_wilting_point[0] * 0.5) +
                (soil_saturation_capacity[1] - soil_water_content[1]) /
                    (soil_saturation_capacity[1] - soil_wilting_point[1] * 0.5));

    soil_initial_abstraction = std::max(0.0, soil_initial_abstraction);

    // Determine threshold amount of rainfall that will occur before
    //      runoff starts (mm)
    double pb = available_water - soil_initial_abstraction * soil_storage;

    double surface_runoff = 0.0;
    if (available_water > 0.001) {
        if (pb > 0.0) {
            surface_runoff = std::pow(pb, 2) / (available_water + 
              (1.0 - soil_initial_abstraction) * soil_storage);
        }
    }
   
    //excess surface runoff when near satuation
    //double sm_max = soil_saturation_capacity[0] * 0.7;
    //double layer0_excess = std::max(0.0, soil_water_content[0] - sm_max);
    //if (layer0_excess > 0.0) {
    //    // Add the excess as additional surface runoff
    //    surface_runoff += layer0_excess;
    //}

    // Update the output quantity list
    update(surface_runoff_op, surface_runoff);
    update(soil_storage_op, soil_storage);
    update(pb_op, pb);
    update(soil_initial_abstraction_op, soil_initial_abstraction);
    update(available_water_op, available_water);
}

}  // namespace standardBML 
#endif
