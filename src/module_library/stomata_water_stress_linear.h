#ifndef STOMATA_WATER_STRESS_LINEAR_H
#define STOMATA_WATER_STRESS_LINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <algorithm>  // for std::min and std::max

/**
 *  @class stomata_water_stress_linear
 *
 *  @brief Calculates a stomatal water stress factor, where 0 indicates
 *  maximum stress (no stomatal opening) and 1 indicates minimum stress (full
 *  stomatal opening).
 *
 *  This is an empirical approach to modeling the influence of water stress on
 *  stomatal behavior and it is not based on a particular publication.
 *
 *  In this module, the stomatal water stress factor (`StomataWS`) is calculated
 *  as a linear function of the relative extractable soil water (`REW`):
 *
 *  > `StomataWS = StomataWS_intercept + StomataWS_gradient * REW` (Equation 1)
 *
 *  where
 *
 *  > `REW = (theta - theta_w) / (theta_c - theta_w)`
 *
 *  and `theta`, `theta_w`, and `theta_c` are the soil water content, the soil
 *  wilting point (minimum water content that can be extracted by the roots),
 *  and the soil field capacity (maximum water content that can be absorbed by
 *  the soil), respectively. These are all volumetric water contents, defined as
 *  (voume of water) / (volume of soil), and hence are dimensionless.
 *
 *  Following the application of Equation 1, the value of `StomataWS` is clamped
 *  to lie between 0 and 1.
 *
 *  NOTE: In previous versions of BioCro, this module simply used
 *  `StomataWS = REW`. This behavior can be reproduced by setting
 *  `StomataWS_gradient` to 1 and `StomataWS_intercept` to 0.
 */
namespace standardBML
{
class stomata_water_stress_linear : public direct_module
{
   public:
    stomata_water_stress_linear(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          StomataWS_gradient{get_input(input_quantities, "StomataWS_gradient")},
          StomataWS_intercept{get_input(input_quantities, "StomataWS_intercept")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_linear"; }

   private:
    // References to input quantities
    const double& soil_field_capacity;
    const double& soil_wilting_point;
    const double& soil_water_content;
    const double& StomataWS_gradient;
    const double& StomataWS_intercept;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_linear::get_inputs()
{
    return {
        "soil_field_capacity",  // dimensionless from (m^3 water) / (m^3 soil)
        "soil_wilting_point",   // dimensionless from (m^3 water) / (m^3 soil)
        "soil_water_content",   // dimensionless from (m^3 water) / (m^3 soil)
        "StomataWS_gradient",   // dimensionless
        "StomataWS_intercept"   // dimensionless
    };
}

string_vector stomata_water_stress_linear::get_outputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

void stomata_water_stress_linear::do_operation() const
{
    // Set limits for StomataWS
    constexpr double StomataWS_min = 0.0;  // dimensionless
    constexpr double StomataWS_max = 1.0;  // dimensionless

    // Apply Equation 1 (see above)
    double const REW = (soil_water_content - soil_wilting_point) /
                       (soil_field_capacity - soil_wilting_point);  // dimensionless

    double const StomataWS_raw = StomataWS_intercept + StomataWS_gradient * REW;  // dimensionless

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(StomataWS_raw, StomataWS_min), StomataWS_max));
}

}  // namespace standardBML
#endif
