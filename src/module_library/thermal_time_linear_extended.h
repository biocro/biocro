#ifndef THERMAL_TIME_LINEAR_EXTENDED_H
#define THERMAL_TIME_LINEAR_EXTENDED_H

#include "../modules.h"
#include "../module_helper_functions.h"
#include "../state_map.h"

/**
 * @class thermal_time_linear_extended
 * 
 * @brief Calculates the rate of thermal time accumulation using an extended linear
 * model.
 * 
 * For an overview of the different methods that can be used for calculating thermal
 * time, see Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018).
 * 
 * This module implements a basic model which is sometimes called either an "extended
 * linear" model or a "plateau" model. In this model, the rate of change of the thermal
 * time TTc is given by:
 * 
 *  rate = 0             :  when temp is below tbase
 * 
 *  rate = temp - tbase  :  when temp is between tbase and topt
 * 
 *  rate = topt - tbase  :  when temp is above topt
 * 
 * As written, TTc has units of degrees C * day and the rate has units of
 * degrees C * day / day = degrees C. This is a common formulation, reflecting the fact
 * that average daily temperatures are often used to calculate the increase in thermal
 * time during an entire day. However, time derivatives in BioCro are specified on a per
 * hour basis.
 * 
 * This model is based on the observation that although development proceeds linearly
 * after exceeding a threshold, it eventually reaches a maximum rate at some optimal
 * temperature. Although this model is more realistic than the pure linear model which
 * increases indefinitely, it still fails to account for the reduction in development
 * rate that often occurs at higher temperatures.
 * 
 * This model can be characterized as a piecewise linear model having 2 cardinal
 * temperatures. For more accurate piecewise linear models, see the
 * thermal_time_bilinear model (3 cardinal temperatures) or the
 * thermal_time_trilinear model (4 cardinal temperatures). If an optimal temperature
 * is unavailable, the thermal_time_linear model may be a more appropriate piecewise
 * linear model.
 */
class thermal_time_linear_extended : public DerivModule
{
   public:
    thermal_time_linear_extended(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("thermal_time_linear_extended"),

          // Get pointers to input parameters
          temp(get_input(input_parameters, "temp")),
          tbase(get_input(input_parameters, "tbase")),
          topt(get_input(input_parameters, "topt")),

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
    double const& topt;

    // Pointers to output parameters
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> thermal_time_linear_extended::get_inputs()
{
    return {
        "temp",   // degrees C
        "tbase",  // degrees C
        "topt"    // degrees C
    };
}

std::vector<std::string> thermal_time_linear_extended::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_linear_extended::do_operation() const
{
    // Find the rate of change on a daily basis
    double const rate_per_day = temp <= tbase ? 0.0
                              : temp <= topt  ? temp - tbase
                              :                 topt - tbase;  // degrees C

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output parameter list
    update(TTc_op, rate_per_hour);
}

#endif
