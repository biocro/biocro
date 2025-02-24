#ifndef THERMAL_TIME_BILINEAR_H
#define THERMAL_TIME_BILINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class thermal_time_bilinear
 *
 * @brief Calculates the rate of thermal time accumulation using a bilinear model.
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
 * This module implements a piecewise linear model that includes optimal and maximum
 * temperatures. Due to its shape, this model is sometimes called "triangular."
 * This model is discussed in many places, see e.g. section 2.9 of Campbell & Norman (1998)
 * or the paper that may have been the first to introduce it:
 * [Garcia-Huidobro, J., Monteith, J. L. & Squire, G. R. "Time, Temperature and Germination of
 * Pearl Millet (Pennisetum typhoides S. & H.) I. CONSTANT TEMPERATURE." J Exp Bot 33, 288–296 (1982)]
 * (https://dx.doi.org/10.1093/jxb/33.2.288).
 *
 * In this model, `DR` is determined from the air temperature `T` according to:
 *
 * | DR                                                 | T range               |
 * | :------------------------------------------------: | :-------------------: |
 * | `0`                                                | `T <= T_base`         |
 * | `T - T_base`                                       | `T_base < T <= T_opt` |
 * | `(T_max - T) * (T_opt - T_base) / (T_max - T_opt)` | `T_opt < T <= T_max`  |
 * | `0`                                                | `T_max < T`           |
 *
 * Thermal time has units of `degrees C * day` and the development rate, as written
 * here, has units of `degrees C * day / day = degrees C`. This is a common formulation,
 * reflecting the fact that average daily temperatures are often used to calculate the
 * increase in thermal time during an entire day.
 *
 * This model is based on the observation that development rate is maximum at some
 * optimum temperature and decreases to zero at exceptionally low or high temperatures.
 * It seems to be reasonably realistic for many plant species.
 *
 * This model can be characterized as a piecewise linear model having three cardinal
 * temperatures. For plants that have a range of temperatures corresponding to their
 * maximum development rate, the thermal_time_trilinear model (which has four cardinal
 * temperatures) may be a more appropriate piecewise linear model. If optimum or maximum
 * temperatures are unavailable, the thermal_time_linear_extended or thermal_time_linear
 * models may be more appropriate piecewise linear models.
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's parameters:
 * - ``'TTc'`` for the thermal time `t_th`
 * - ``'temp'`` for the air temperature `T`
 * - ``'tbase'`` for the base temperature `T_base`
 * - ``'topt'`` for the optimal temperature `T_opt`
 * - ``'tmax'`` for the maximum temperature `T_max`
 *
 * Also note that time derivatives in BioCro are specified on a per hour basis, so a
 * conversion factor of `24 hours per day` is required in the code as compared to the
 * formulas presented above.
 */
class thermal_time_bilinear : public differential_module
{
   public:
    thermal_time_bilinear(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          fractional_doy{get_input(input_quantities, "fractional_doy")},
          sowing_fractional_doy{get_input(input_quantities, "sowing_fractional_doy")},
          temp{get_input(input_quantities, "temp")},
          tbase{get_input(input_quantities, "tbase")},
          topt{get_input(input_quantities, "topt")},
          tmax{get_input(input_quantities, "tmax")},

          // Get pointers to output quantities
          TTc_op{get_op(output_quantities, "TTc")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_bilinear"; }

   private:
    // References to input quantities
    double const& fractional_doy;
    double const& sowing_fractional_doy;
    double const& temp;
    double const& tbase;
    double const& topt;
    double const& tmax;

    // Pointers to output quantities
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

string_vector thermal_time_bilinear::get_inputs()
{
    return {
        "fractional_doy",         // days
        "sowing_fractional_doy",  // days
        "temp",                   // degrees C
        "tbase",                  // degrees C
        "topt",                   // degrees C
        "tmax"                    // degrees C
    };
}

string_vector thermal_time_bilinear::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_bilinear::do_operation() const
{
    // Find the rate of change on a daily basis
    double const rate_per_day =
        fractional_doy < sowing_fractional_doy ? 0.0
        : temp <= tbase                        ? 0.0
        : temp <= topt                         ? temp - tbase
        : temp <= tmax                         ? (tmax - temp) * (topt - tbase) / (tmax - topt)
                                               : 0.0;  // degrees C

    // Convert to an hourly rate
    double const rate_per_hour = rate_per_day / 24.0;  // degrees C * day / hr

    // Update the output quantity list
    update(TTc_op, rate_per_hour);
}

}  // namespace standardBML
#endif
