#ifndef THERMAL_TIME_LINEAR_H
#define THERMAL_TIME_LINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
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
 *   (https://dx.doi.org/10.1007/978-1-4612-1626-1_2)
 *
 * - [Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 *   of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999)]
 *   (https://dx.doi.org/10.1006/anbo.1999.0955)
 *
 * - [McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 *   Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018)]
 *   (https://dx.doi.org/10.1007/978-1-4939-2493-6_384-3)
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
class thermal_time_linear : public differential_module
{
   public:
    thermal_time_linear(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          time{get_input(input_quantities, "time")},
          sowing_time{get_input(input_quantities, "sowing_time")},
          temp{get_input(input_quantities, "temp")},
          tbase{get_input(input_quantities, "tbase")},

          // Get pointers to output quantities
          TTc_op{get_op(output_quantities, "TTc")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_linear"; }

   private:
    // References to input quantities
    double const& time;
    double const& sowing_time;
    double const& temp;
    double const& tbase;

    // Pointers to output quantities
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

string_vector thermal_time_linear::get_inputs()
{
    return {
        "time",         // days
        "sowing_time",  // days
        "temp",         // degrees C
        "tbase"         // degrees C
    };
}

string_vector thermal_time_linear::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_linear::do_operation() const
{
    // Find the rate of change on a daily basis
    double const rate_per_day = time < sowing_time ? 0.0
                                : temp <= tbase    ? 0.0
                                                   : temp - tbase;  // degrees C

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output quantity list
    update(TTc_op, rate_per_hour);
}

}  // namespace standardBML
#endif
