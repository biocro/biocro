#ifndef TOTAL_SOIL_WATER_H
#define TOTAL_SOIL_WATER_H

#include <numeric>  // for std::accumulate
#include <vector>   // for std::vector
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class total_soil_water
 *
 *  @brief Calculates the total soil water across all soil layers and returns it
 *  as ``'total_soil_water'``.
 */
class total_soil_water : public direct_module
{
   public:
    total_soil_water(
        const state_map& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input parameters
          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},

          // Get pointers to output parameters
          total_soil_water_op{get_op(output_quantities, "total_soil_water")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "total_soil_water"; }

   private:
    // Definition of a struct that represents a soil layer
    struct soil_layer {
        double const thickness;      // cm
        double const water_content;  // m^3 / m^3
    };

    // Pointers to input parameters
    double const& soil_depth_1;
    double const& soil_depth_2;
    double const& soil_depth_3;
    double const& soil_depth_4;
    double const& soil_depth_5;
    double const& soil_depth_6;
    double const& soil_water_content_1;
    double const& soil_water_content_2;
    double const& soil_water_content_3;
    double const& soil_water_content_4;
    double const& soil_water_content_5;
    double const& soil_water_content_6;

    // Pointers to output parameters
    double* total_soil_water_op;

    // Main operation
    void do_operation() const;
};

string_vector total_soil_water::get_inputs()
{
    return {
        "soil_depth_1",          // cm
        "soil_depth_2",          // cm
        "soil_depth_3",          // cm
        "soil_depth_4",          // cm
        "soil_depth_5",          // cm
        "soil_depth_6",          // cm
        "soil_water_content_1",  // m^3 / m^3
        "soil_water_content_2",  // m^3 / m^3
        "soil_water_content_3",  // m^3 / m^3
        "soil_water_content_4",  // m^3 / m^3
        "soil_water_content_5",  // m^3 / m^3
        "soil_water_content_6"   // m^3 / m^3
    };
}

string_vector total_soil_water::get_outputs()
{
    return {
        "total_soil_water"  // Mg / ha
    };
}

void total_soil_water::do_operation() const
{
    // Define constants
    double constexpr cm_to_mm = 10;         // mm / cm
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm

    // Consolidate soil properties
    std::vector<soil_layer> const soil_layers{
        {soil_depth_1, soil_water_content_1},
        {soil_depth_2, soil_water_content_2},
        {soil_depth_3, soil_water_content_3},
        {soil_depth_4, soil_water_content_4},
        {soil_depth_5, soil_water_content_5},
        {soil_depth_6, soil_water_content_6}};

    // Lambda for accumulating soil water
    auto accum_soil_water = [](double const acc, soil_layer const layer) {
        return acc + layer.water_content * layer.thickness;  // cm
    };

    // Total water in all layers
    double const total_water = std::accumulate(
        soil_layers.begin(), soil_layers.end(),
        0.0,
        accum_soil_water);  // cm

    update(total_soil_water_op,
           total_water * cm_to_mm * mm_to_Mg_per_ha);  // Mg / ha
}

}  // namespace standardBML
#endif
