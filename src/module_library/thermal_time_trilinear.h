#ifndef THERMAL_TIME_TRILINEAR_H
#define THERMAL_TIME_TRILINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class thermal_time_trilinear
 *
 * @brief Calculates the rate of thermal time accumulation using a trilinear model.
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
 * This module implements a piecewise linear model that includes a range of optimal
 * temperatures in addition to base and maximum temperatures. Due to its shape, this
 * model is sometimes called "trapezoidal." This model is implemented in several other
 * crop growth simulators, e.g:
 *
 * - `CROPSIM`, described in [Hunt, L. A. & Pararajasingham, S. "CROPSIM — WHEAT: A model
 *   describing the growth and development of wheat." Can. J. Plant Sci. 75, 619–632 (1995)]
 *   (https://dx.doi.org/10.4141/cjps95-107)
 *
 * - `CROPGRO`, described in [Piper et al. "Comparison of Two Phenology Models for Predicting
 *   Flowering and Maturity Date of Soybean." Crop Science 36, 1606–1614 (1996)]
 *   (https://dx.doi.org/10.2135/cropsci1996.0011183X003600060033x)
 *
 *
 * In this model, `DR` is determined from the air temperature `T` according to:
 *
 * | DR                                                             | T range                         |
 * | :------------------------------------------------------------: | :-----------------------------: |
 * | `0`                                                            | `T <= T_base`                   |
 * | `T - T_base`                                                   | `T_base < T <= T_opt_lower`     |
 * | `T_opt_lower - T_base`                                         | `T_opt_lower < T <= T_opt_upper`|
 * | `(T_max - T) * (T_opt_lower - T_base) / (T_max - T_opt_upper)` | `T_opt_upper < T <= T_max`      |
 * | `0`                                                            | `T_max < T`                     |
 *
 * Thermal time has units of `degrees C * day` and the development rate, as written
 * here, has units of `degrees C * day / day = degrees C`. This is a common formulation,
 * reflecting the fact that average daily temperatures are often used to calculate the
 * increase in thermal time during an entire day.
 *
 * This model is based on the observation that development rate is maximum at some
 * optimum range of temperatures and decreases to zero at exceptionally low or high
 * temperatures. It seems to be reasonably realistic for many plant species.
 *
 * This model can be characterized as a piecewise linear model having four cardinal
 * temperatures. If some of these temperatures are unavailable, other piecewise
 * linear models are available that require fewer cardinal temperatures:
 * thermal_time_linear, thermal_time_linear_extended, and thermal_time_bilinear.
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's parameters:
 * - ``'TTc'`` for the thermal time `t_th`
 * - ``'temp'`` for the air temperature `T`
 * - ``'tbase'`` for the base temperature `T_base`
 * - ``'topt_lower'`` for the lower optimal temperature `T_opt_lower`
 * - ``'topt_upper'`` for the upper optimal temperature `T_opt_upper`
 * - ``'tmax'`` for the maximum temperature `T_max`
 *
 * Also note that time derivatives in BioCro are specified on a per hour basis, so a
 * conversion factor of `24 hours per day` is required in the code as compared to the
 * formulas presented above.
 */
class thermal_time_trilinear : public differential_module
{
   public:
    thermal_time_trilinear(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          time{get_input(input_quantities, "time")},
          sowing_time{get_input(input_quantities, "sowing_time")},
          temp{get_input(input_quantities, "temp")},
          tbase{get_input(input_quantities, "tbase")},
          topt_lower{get_input(input_quantities, "topt_lower")},
          topt_upper{get_input(input_quantities, "topt_upper")},
          tmax{get_input(input_quantities, "tmax")},

          // Get pointers to output quantities
          TTc_op{get_op(output_quantities, "TTc")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_trilinear"; }

   private:
    // References to input quantities
    double const& time;
    double const& sowing_time;
    double const& temp;
    double const& tbase;
    double const& topt_lower;
    double const& topt_upper;
    double const& tmax;

    // Pointers to output quantities
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

string_vector thermal_time_trilinear::get_inputs()
{
    return {
        "time",         // days
        "sowing_time",  // days
        "temp",         // degrees C
        "tbase",        // degrees C
        "topt_lower",   // degrees C
        "topt_upper",   // degrees C
        "tmax"          // degrees C
    };
}

string_vector thermal_time_trilinear::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_trilinear::do_operation() const
{
    // Find the rate of change on a daily basis
    double const rate_per_day = time < sowing_time   ? 0.0
                                : temp <= tbase      ? 0.0
                                : temp <= topt_lower ? temp - tbase
                                : temp <= topt_upper ? topt_lower - tbase
                                : temp <= tmax       ? (tmax - temp) * (topt_lower - tbase) / (tmax - topt_upper)
                                                     : 0.0;  // degrees C

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output quantity list
    update(TTc_op, rate_per_hour);
}

}  // namespace standardBML
#endif
