#ifndef THERMAL_TIME_TRILINEAR_H
#define THERMAL_TIME_TRILINEAR_H

#include "../modules.h"

/**
 * @class thermal_time_trilinear
 * 
 * @brief Calculates the rate of thermal time accumulation using a trilinear model.
 * 
 * For an overview of the different methods that can be used for calculating thermal
 * time, see Yan, W. & Hunt, L. A. "An Equation for Modelling the Temperature Response
 * of Plants using only the Cardinal Temperatures" Ann Bot 84, 607–614 (1999) and
 * McMaster, G. S. & Moragues, M. "Crop Development Related to Temperature and
 * Photoperiod" in "Encyclopedia of Sustainability Science and Technology" (2018).
 * 
 * This module implements a piecewise linear model that includes a range of optimal
 * temperatures in addition to base and maximum temperatures. Due to its shape, this
 * model is sometimes called "trapezoidal." This model is implemented in several other
 * crop growth simulators, e.g. CROPSIM and CROPGRO, which are described in Hunt, L. A.
 * & Pararajasingham, S. "CROPSIM — WHEAT: A model describing the growth and development
 * of wheat." Can. J. Plant Sci. 75, 619–632 (1995) and Piper et al. "Comparison of Two
 * Phenology Models for Predicting Flowering and Maturity Date of Soybean." Crop Science
 * 36, 1606–1614 (1996), respectively.
 * 
 * In this model, the rate of change of the thermal time TTc is given by:
 * 
 *  rate = 0                       :  when temp is below tbase
 * 
 *  rate = temp - tbase            :  when temp is between tbase and topt_lower
 * 
 *  rate = topt_lower - tbase      :  when temp is between topt_lower and topt_upper
 * 
 *  rate = (tmax - temp) * 
 *         (topt_lower - tbase) / 
 *         (tmax - topt_upper)     :  when temp is between topt_upper and tmax
 * 
 *  rate = 0                       :  when temp is above tmax
 * 
 * As written, TTc has units of degrees C * day and the rate has units of
 * degrees C * day / day = degrees C. This is a common formulation, reflecting the fact
 * that average daily temperatures are often used to calculate the increase in thermal
 * time during an entire day. However, time derivatives in BioCro are specified on a per
 * hour basis.
 * 
 * This model is based on the observation that development rate is maximum at some
 * optimum range of temperatures and decreases to zero at exceptionally low or high
 * temperatures. It seems to be reasonably realistic for many plant species.
 * 
 * This model can be characterized as a piecewise linear model having 4 cardinal
 * temperatures. If some of these temperatures are unavailable, other piecewise
 * linear models are available that require fewer cardinal temperatures:
 * thermal_time_linear, thermal_time_linear_extended, and thermal_time_bilinear.
 */
class thermal_time_trilinear : public DerivModule
{
   public:
    thermal_time_trilinear(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("thermal_time_trilinear"),
          // Get pointers to input parameters
          temp(get_input(input_parameters, "temp")),
          tbase(get_input(input_parameters, "tbase")),
          topt_lower(get_input(input_parameters, "topt_lower")),
          topt_upper(get_input(input_parameters, "topt_upper")),
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
    double const& topt_lower;
    double const& topt_upper;
    double const& tmax;

    // Pointers to output parameters
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> thermal_time_trilinear::get_inputs()
{
    return {
        "temp",        // deg. C
        "tbase",       // deg. C
        "topt_lower",  // deg. C
        "topt_upper",  // deg. C
        "tmax"         // deg. C
    };
}

std::vector<std::string> thermal_time_trilinear::get_outputs()
{
    return {
        "TTc"  // deg. C * day / hr
    };
}

void thermal_time_trilinear::do_operation() const
{
    // Find the rate of change on a daily basis
    double rate_per_day;  // deg. C
    if (temp <= tbase) {
        rate_per_day = 0.0;
    } else if (temp <= topt_lower) {
        rate_per_day = temp - tbase;
    } else if (temp <= topt_upper) {
        rate_per_day = topt_lower - tbase;
    } else if (temp <= tmax) {
        rate_per_day = (tmax - temp) * (topt_lower - tbase) / (tmax - topt_upper);
    } else {
        rate_per_day = 0.0;
    }

    // Convert to an hourly rate
    const double rate_per_hour = rate_per_day / 24.0;  // deg. C * day / hr

    // Update the output parameter list
    update(TTc_op, rate_per_hour);
}

#endif
