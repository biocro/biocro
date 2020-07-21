#ifndef THERMAL_TIME_BILINEAR_H
#define THERMAL_TIME_BILINEAR_H

#include "../modules.h"
#include "../module_helper_functions.h"
#include "../state_map.h"

/**
 * @class thermal_time_bilinear
 * 
 * @brief Calculates the rate of thermal time accumulation using a bilinear model.
 * 
 * For an overview of the different methods that can be used for calculating thermal
 * time, see Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018).
 * 
 * This module implements a piecewise linear model that includes optimal and maximum
 * temperatures. Due to its shape, this model is sometimes called "triangular."
 * This model is discussed in many places, see e.g. section 2.9 of Campbell & Norman's
 * textbook "An Introduction to Environmental Biophysics" (1998) or the paper that may
 * have been the first to introduce it: Garcia-Huidobro, J., Monteith, J. L. & Squire,
 * G. R. "Time, Temperature and Germination of Pearl Millet (Pennisetum typhoides S. &
 * H.) I. CONSTANT TEMPERATURE." J Exp Bot 33, 288–296 (1982).
 * 
 * In this model, the rate of change of the thermal time TTc is given by:
 * 
 *  rate = 0                 :  when temp is below tbase
 * 
 *  rate = temp - tbase      :  when temp is between tbase and topt
 * 
 *  rate = (tmax - temp) * 
 *         (topt - tbase) /
 *         (tmax - topt)     :  when temp is between topt and tmax
 * 
 *  rate = 0                 :  when temp is above tmax
 * 
 * As written, TTc has units of degrees C * day and the rate has units of
 * degrees C * day / day = degrees C. This is a common formulation, reflecting the fact
 * that average daily temperatures are often used to calculate the increase in thermal
 * time during an entire day. However, time derivatives in BioCro are specified on a per
 * hour basis.
 * 
 * This model is based on the observation that development rate is maximum at some
 * optimum temperature and decreases to zero at exceptionally low or high temperatures.
 * It seems to be reasonably realistic for many plant species.
 * 
 * This model can be characterized as a piecewise linear model having 3 cardinal
 * temperatures. For plants that have a range of temperatures corresponding to their
 * maximum development rate, the thermal_time_trilinear model (which has 4 cardinal
 * temperatures) may be a more appropriate piecewise linear model. If optimum or maximum
 * temperatures are unavailable, the thermal_time_linear_extended or thermal_time_linear
 * models may be more appropriate piecewise linear models.
 */
class thermal_time_bilinear : public DerivModule
{
   public:
    thermal_time_bilinear(
        state_map const* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("thermal_time_bilinear"),

          // Get pointers to input parameters
          temp(get_input(input_parameters, "temp")),
          tbase(get_input(input_parameters, "tbase")),
          topt(get_input(input_parameters, "topt")),
          tmax(get_input(input_parameters, "tmax")),

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
    double const& tmax;

    // Pointers to output parameters
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> thermal_time_bilinear::get_inputs()
{
    return {
        "temp",   // degrees C
        "tbase",  // degrees C
        "topt",   // degrees C
        "tmax"    // degrees C
    };
}

std::vector<std::string> thermal_time_bilinear::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_bilinear::do_operation() const
{
    // Find the rate of change on a daily basis
    double rate_per_day;  // degrees C
    if (temp <= tbase) {
        rate_per_day = 0.0;
    } else if (temp <= topt) {
        rate_per_day = temp - tbase;
    } else if (temp <= tmax) {
        rate_per_day = (tmax - temp) * (topt - tbase) / (tmax - topt);
    } else {
        rate_per_day = 0.0;
    }

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output parameter list
    update(TTc_op, rate_per_hour);
}

#endif
