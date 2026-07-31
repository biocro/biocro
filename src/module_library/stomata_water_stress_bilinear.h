#ifndef STOMATA_WATER_STRESS_BILINEAR_H
#define STOMATA_WATER_STRESS_BILINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <algorithm>  // for std::min and std::max

/**
 *  @class stomata_water_stress_bilinear
 *
 *  @brief Calculates a stomatal water stress factor, where 0 indicates
 *  maximum stress (no stomatal opening) and 1 indicates minimum stress (full
 *  stomatal opening).
 *
 *  This is an empirical approach to modeling the influence of water stress on
 *  stomatal behavior and it is not based on a particular publication.
 *
 *  In this module, the stomatal water stress factor (`StomataWS`) is calculated
 *  as a bilinear function of the relative extractable soil water (`REW`), where
 *
 *  > `REW = (theta - theta_w) / (theta_c - theta_w)` (Equation 1)
 *
 *  and `theta`, `theta_w`, and `theta_c` are the soil water content, the soil
 *  wilting point (minimum water content that can be extracted by the roots),
 *  and the soil field capacity (maximum water content that can be absorbed by
 *  the soil), respectively. These are all volumetric water contents, defined as
 *  (voume of water) / (volume of soil), and hence are dimensionless.
 *
 *  When `REW` is between 0 and a threshold value (`REW_0`), `StomataWS` is
 *  given by
 *
 *  > `StomataWS = StomataWS_min + StomataWS_islope * REW` (Equation 2)
 *
 *  where `StomataWS_min` is the smallest possible value of `StomataWS` and
 *  `StomataWS_islope` is the initial slope of the bilinear relationship. When
 *  `REW` is above the threshold value, `StomataWS` is given by
 *
 *  > `StomataWS = StomataWS_intercept + StomataWS_gradient * REW` (Equation 3)
 *
 *  where `StomataWS_intercept` and `StomataWS_gradient` are the intercept and
 *  gradient of the second part of the bilinear relationship.
 *
 *  To ensure that `StomataWS` is a continuous function of `REW`, it is
 *  necessary that
 *
 *  > `StomataWS_min + StomataWS_islope * REW_0 =`
 *  >    `StomataWS_intercept + StomataWS_gradient * REW_0` (Equation 4)
 *
 *  This requires that the `StomataWS_islope` is given by
 *
 *  > `StomataWS_islope = StomataWS_gradient +`
 *  >    `(StomataWS_intercept - StomataWS_min) / REW_0` (Equation 5)
 *
 *  This is enforced in the code below.
 *
 *  Following the application of the above equations, the value of `StomataWS`
 *  is clamped to lie between `StomataWS_min` and 1.
 */
namespace standardBML
{
class stomata_water_stress_bilinear : public direct_module
{
   public:
    stomata_water_stress_bilinear(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          StomataWS_gradient{get_input(input_quantities, "StomataWS_gradient")},
          StomataWS_intercept{get_input(input_quantities, "StomataWS_intercept")},
          StomataWS_min{get_input(input_quantities, "StomataWS_min")},
          REW_0{get_input(input_quantities, "REW_0")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_bilinear"; }

   private:
    // References to input quantities
    double const& soil_field_capacity;
    double const& soil_wilting_point;
    double const& soil_water_content;
    double const& StomataWS_gradient;
    double const& StomataWS_intercept;
    double const& StomataWS_min;
    double const& REW_0;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_bilinear::get_inputs()
{
    return {
        "soil_field_capacity",  // dimensionless from (m^3 water) / (m^3 soil)
        "soil_wilting_point",   // dimensionless from (m^3 water) / (m^3 soil)
        "soil_water_content",   // dimensionless from (m^3 water) / (m^3 soil)
        "StomataWS_gradient",   // dimensionless
        "StomataWS_intercept",  // dimensionless
        "StomataWS_min",        // dimensionless
        "REW_0"                 // dimensionless
    };
}

string_vector stomata_water_stress_bilinear::get_outputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

void stomata_water_stress_bilinear::do_operation() const
{
    // Set upper limit for StomataWS
    double constexpr StomataWS_max = 1.0;  // dimensionless

    // Equation 1
    double const REW = (soil_water_content - soil_wilting_point) /
                       (soil_field_capacity - soil_wilting_point);  // dimensionless

    // Equation 5
    double const StomataWS_islope =
        StomataWS_gradient +
        (StomataWS_intercept - StomataWS_min) / REW_0;  // dimensionless

    // Equations 2 and 3
    double const StomataWS_raw =
        REW <= REW_0
            ? StomataWS_min + StomataWS_islope * REW
            : StomataWS_intercept + StomataWS_gradient * REW;  // dimensionless

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(StomataWS_raw, StomataWS_min), StomataWS_max));
}

}  // namespace standardBML
#endif
