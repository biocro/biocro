#ifndef SOIL_WATER_DYNAMIC_ROOTING_H
#define SOIL_WATER_DYNAMIC_ROOTING_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class soil_water_dynamic_rooting
 *
 *  @brief Allows max_rooting_layer to change over time based on Root biomass
 *  This is a very simple function to convert biomass to root depth, which is the number of
 *  soil layers that roots are able to access for root water uptake calculation.
 *  The constant conversion factor `rsdf` (0.44) is a legacy value inherited
 *  from early versions of BioCro, likely standing for Root Soil Depth Factor.
 *  Another key concept for root water uptake is the root distribution, which
 *  is handled in soil_water_uptake.h with an exponential decay function.
 *  The decay function simulates most roots accumulate at top layers so not all layers
 *  are extracted equally.
 *  This module is used with other DSSAT-based water modules,
 *  but is homemade (not based on any DSSAT codes).
 *
 */
class soil_water_dynamic_rooting : public direct_module
{
   public:
    soil_water_dynamic_rooting(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          min_root_depth{get_input(input_quantities, "min_root_depth")},
          rsdf{get_input(input_quantities, "rsdf")},
          Root{get_input(input_quantities, "Root")},
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},

          // Get pointers to output quantities
          max_rooting_layer_op{get_op(output_quantities, "max_rooting_layer")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_dynamic_rooting"; }

   private:
    // Pointers to input quantities
    double const& min_root_depth;
    double const& rsdf;
    double const& Root;
    double const& soil_depth_1;
    double const& soil_depth_2;
    double const& soil_depth_3;
    double const& soil_depth_4;
    double const& soil_depth_5;
    double const& soil_depth_6;

    // Pointers to output quantities
    double* max_rooting_layer_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_water_dynamic_rooting::get_inputs()
{
    return {
        "min_root_depth",  // cm
        "rsdf",            // m / Mg / ha. Constant conversion from biomass to depth
        "Root",            // Mg / ha
        "soil_depth_1",    // cm
        "soil_depth_2",    // cm
        "soil_depth_3",    // cm
        "soil_depth_4",    // cm
        "soil_depth_5",    // cm
        "soil_depth_6"     // cm
    };
}

string_vector soil_water_dynamic_rooting::get_outputs()
{
    return {
        "max_rooting_layer"  // dimensionless. number of layer the root can access
    };
}

void soil_water_dynamic_rooting::do_operation() const
{
    double constexpr m_to_cm = 100;
    double constexpr epsilon = 0.01;  // cm

    // Array of layer thicknesses for easy iteration
    double thicknesses[] = {
        soil_depth_1,  // cm
        soil_depth_2,  // cm
        soil_depth_3,  // cm
        soil_depth_4,  // cm
        soil_depth_5,  // cm
        soil_depth_6   // cm
    };

    // Calculate root depth
    // the minimal depth root can access is min_root_depth
    double rootDepth = std::max(min_root_depth + epsilon, rsdf * Root * m_to_cm);  // cm

    // Determine how many layers are within the root depth
    int layers_count = 0;           // dimensionless
    double cumulative_depth = 0.0;  // cm

    for (int i = 0; i < 6; ++i) {
        // If the top of the current layer is already beyond the root depth, stop.
        // However, usually we count a layer if the roots have entered it at all.
        if (rootDepth > cumulative_depth) {
            layers_count++;
        } else {
            break;
        }
        cumulative_depth += thicknesses[i];
    }
    update(max_rooting_layer_op, layers_count);
}

}  // namespace standardBML
#endif
