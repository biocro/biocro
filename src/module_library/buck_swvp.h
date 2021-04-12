#ifndef BUCK_SWVP_H
#define BUCK_SWVP_H

#include "../modules.h"
#include "../state_map.h"
#include "AuxBioCro.h"  // for saturation_vapor_pressure

/**
 * @class buck_swvp
 *
 * @brief Determines the saturation water vapor pressure of atmospheric air
 * using the `saturation_vapor_pressure()` function, which implements the Arden
 * Buck equation.
 */
class buck_swvp : public SteadyModule
{
   public:
    buck_swvp(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("buck_swvp"),

          // Get references to input parameters
          temp{get_input(input_parameters, "temp")},

          // Get pointers to output parameters
          saturation_water_vapor_pressure_atmosphere_op{get_op(output_parameters, "saturation_water_vapor_pressure_atmosphere")}
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to input parameters
    double const& temp;

    // Pointers to output parameters
    double* saturation_water_vapor_pressure_atmosphere_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> buck_swvp::get_inputs()
{
    return {
        "temp"  // degrees C
    };
}

std::vector<std::string> buck_swvp::get_outputs()
{
    return {
        "saturation_water_vapor_pressure_atmosphere"  // Pa
    };
}

void buck_swvp::do_operation() const
{
    // Update the output parameter list
    update(saturation_water_vapor_pressure_atmosphere_op,
           saturation_vapor_pressure(temp));  // Pa
}

#endif
