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
 * For an overview of the different methods that can be used for calculating thermal
 * time, see Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018).
 * 
 * This module implements the most basic model, which is discussed in many places,
 * e.g. section 2.7 of Campbell & Norman's textbook "An Introduction to Environmental
 * Biophysics" (1998). In this model, the rate of change of the thermal time TTc is
 * given by:
 * 
 *  rate = 0             : when temp is below tbase
 * 
 *  rate = temp - tbase  : when temp is above tbase
 * 
 * As written, TTc has units of degrees C * day and the rate has units of
 * degrees C * day / day = degrees C. This is a common formulation, reflecting the fact
 * that average daily temperatures are often used to calculate the increase in thermal
 * time during an entire day. However, time derivatives in BioCro are specified on a per
 * hour basis.
 * 
 * This model is based on the observation that once the air temperature exceeds a
 * threshold, development begins to proceed linearly. However, it is known that this
 * trend cannot continue indefinitely and this model tends to overestimate development
 * at high temperatures.
 * 
 * This model can be characterized as a piecewise linear model having 1 cardinal
 * temperature. For more accurate piecewise linear models, see the
 * thermal_time_linear_extended model (2 cardinal temperatures), the
 * thermal_time_bilinear model (3 cardinal temperatures), or the
 * thermal_time_trilinear model (4 cardinal temperatures).
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
    double rate_per_day;  // degrees C
    if (temp <= tbase) {
        rate_per_day = 0.0;
    } else {
        rate_per_day = temp - tbase;
    }

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output parameter list
    update(TTc_op, rate_per_hour);
}

#endif
