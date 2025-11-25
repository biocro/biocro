#ifndef SURFACE_EXCESS_WATER_H
#define SURFACE_EXCESS_WATER_H

#include <cmath> // for std::fmax
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class surface_excess_water
 *
 */

class surface_excess_water: public differential_module

{
   public:
    surface_excess_water(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},

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
    static std::string get_name() { return "surface_excess_water"; }

   private:
    // References to input quantities

    double const& soil_saturation_capacity_1;
    double const& soil_water_content_1;

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

string_vector surface_excess_water::get_inputs()
{
  return {
      "soil_saturation_capacity_1",
      "soil_water_content_1"
  };
}

string_vector surface_excess_water::get_outputs()
{
  return {
      "soil_water_content_1",
      "soil_water_content_2",
      "soil_water_content_3",
      "soil_water_content_4",
      "soil_water_content_5",
      "soil_water_content_6"
  };
}

void surface_excess_water::do_operation() const
{
    //excess surface runoff when near satuation
    double sm_max = soil_saturation_capacity_1 * 0.7;
    double layer0_excess = std::max(0.0, soil_water_content_1 - sm_max);
    double delta_soil_water_content{0.0};
    if (layer0_excess > 0.0) {
        delta_soil_water_content = -layer0_excess;
    }
    update(soil_water_content_1_op, delta_soil_water_content);
}
}
#endif
