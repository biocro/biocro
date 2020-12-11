#ifndef THERMAL_TIME_LINEAR_H
#define THERMAL_TIME_LINEAR_H

#include "../modules.h"
#include "../module_helper_functions.h"
#include "../state_map.h"

/**
 * @class thermal_time_linear
 * 
 * @brief Calculates the rate of thermal time accumulation using a linear model.
 * 
 * ### Model overview
 * 
 * The thermal time `t_th` represents the accumulated exposure of a plant to temperatures
 * within some acceptable range that allows the plant to develop. The time rate of change
 * of `t_th` is often referred to as the development rate `DR`. For an overview of the
 * different methods that can be used for calculating thermal time and its development rate,
 * see the following references:
 * 
 * - [Campbell, G. S. & Norman, J. M. "Chapter 2: Temperature" in "An Introduction to
 *   Environmental Biophysics" (1998)]
 *   (http://dx.doi.org/10.1007/978-1-4612-1626-1_2)
 * 
 * - [Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 *   of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999)]
 *   (http://dx.doi.org/10.1006/anbo.1999.0955)
 * 
 * - [McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 *   Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018)]
 *   (http://dx.doi.org/10.1007/978-1-4939-2493-6_384-3)
 * 
 * This module implements the most basic model, which is discussed in many places,
 * e.g. section 2.7 of Campbell & Norman (1998). In this model, `DR` is determined
 * from the air temperature `T` according to:
 * 
 * | DR           | T range       |
 * | :----------: | :-----------: |
 * | `0`          | `T <= T_base` |
 * | `T - T_base` | `T_base < T`  |
 * 
 * Thermal time has units of `degrees C * day` and the development rate, as written
 * here, has units of `degrees C * day / day = degrees C`. This is a common formulation,
 * reflecting the fact that average daily temperatures are often used to calculate the
 * increase in thermal time during an entire day.
 * 
 * This model is based on the observation that once the air temperature exceeds a
 * threshold, development begins to proceed linearly. However, it is known that this
 * trend cannot continue indefinitely and this model tends to overestimate development
 * at high temperatures.
 * 
 * This model can be characterized as a piecewise linear model having one cardinal
 * temperature. For more realistic piecewise linear models, see the
 * thermal_time_linear_extended model (two cardinal temperatures), the
 * thermal_time_bilinear model (three cardinal temperatures), or the
 * thermal_time_trilinear model (four cardinal temperatures).
 * 
 * ### BioCro module implementation
 * 
 * In BioCro, we use the following names for this model's parameters:
 * - ``'TTc'`` for the thermal time `t_th`
 * - ``'temp'`` for the air temperature `T`
 * - ``'tbase'`` for the base temperature `T_base`
 * 
 * Also note that time derivatives in BioCro are specified on a per hour basis, so a
 * conversion factor of `24 hours per day` is required in the code as compared to the
 * formulas presented above.
 */
class thermal_time_linear : public DerivModule
{
   public:
    thermal_time_linear(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("thermal_time_linear"),

          // Get pointers to input parameters
          temp(get_input(input_parameters, "temp")),
          tbase(get_input(input_parameters, "tbase")),

          // Get pointers to output parameters
          TTc_op(get_op(output_parameters, "TTc"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to input parameters
    double const& temp;
    double const& tbase;

    // Pointers to output parameters
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> thermal_time_linear::get_inputs()
{
    return {
        "temp",  // degrees C
        "tbase"  // degrees C
    };
}

std::vector<std::string> thermal_time_linear::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_linear::do_operation() const
{
    // Find the rate of change on a daily basis
    double const rate_per_day = temp <= tbase ? 0.0 : temp - tbase;  // degrees C

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output parameter list
    update(TTc_op, rate_per_hour);
}

#endif
