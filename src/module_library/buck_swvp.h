#ifndef BUCK_SWVP_H
#define BUCK_SWVP_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "water_and_air_properties.h"  // for saturation_vapor_pressure

namespace standardBML
{
/**
 * @class buck_swvp
 *
 * @brief Determines the saturation water vapor pressure of atmospheric air
 * using the `saturation_vapor_pressure()` function, which implements the Arden
 * Buck equation.
 */
class buck_swvp : public direct_module
{
   public:
    buck_swvp(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          temp{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          saturation_water_vapor_pressure_atmosphere_op{get_op(output_quantities, "saturation_water_vapor_pressure_atmosphere")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "buck_swvp"; }

   private:
    // References to input quantities
    double const& temp;

    // Pointers to output quantities
    double* saturation_water_vapor_pressure_atmosphere_op;

    // Main operation
    void do_operation() const;
};

string_vector buck_swvp::get_inputs()
{
    return {
        "temp"  // degrees C
    };
}

string_vector buck_swvp::get_outputs()
{
    return {
        "saturation_water_vapor_pressure_atmosphere"  // Pa
    };
}

void buck_swvp::do_operation() const
{
    // Update the output quantity list
    update(saturation_water_vapor_pressure_atmosphere_op,
           saturation_vapor_pressure(temp));  // Pa
}

}  // namespace standardBML
#endif
