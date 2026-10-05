#ifndef THERMAL_TIME_BILINEAR_H
#define THERMAL_TIME_BILINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../module_macros.h"

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
#define THERMAL_TIME_BILINEAR_INPUTS(X)                                                \
    X(fractional_doy, "fractional_doy")                /* days */                      \
    X(sowing_fractional_doy, "sowing_fractional_doy")  /* days */                      \
    X(temp, "temp")                                    /* degrees C */                 \
    X(tbase, "tbase")                                  /* degrees C */                 \
    X(topt, "topt")                                    /* degrees C */                 \
    X(tmax, "tmax")                                    /* degrees C */

#define THERMAL_TIME_BILINEAR_OUTPUTS(X)                                               \
    X(TTc_op, "TTc")                                   /* degrees C * day / hr */

DEFINE_MODULE_CLASS(
    thermal_time_bilinear,
    differential_module,
    THERMAL_TIME_BILINEAR_INPUTS,
    THERMAL_TIME_BILINEAR_OUTPUTS
)

#undef THERMAL_TIME_BILINEAR_INPUTS
#undef THERMAL_TIME_BILINEAR_OUTPUTS

inline void thermal_time_bilinear::do_operation() const
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
   //
#endif
