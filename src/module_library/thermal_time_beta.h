#ifndef THERMAL_TIME_BETA_H
#define THERMAL_TIME_BETA_H

#include <cmath>  // for pow
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class thermal_time_beta
 *
 * @brief Calculates the rate of thermal time accumulation using a beta distribution
 * function.
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
 * This particular module is based off [Yin et al. "A nonlinear model for crop development as
 * a function of temperature." Agricultural and Forest Meteorology 77, 1–16 (1995)]
 * (https://dx.doi.org/10.1016/0168-1923(95)02236-Q). In that paper, `DR` is determined from
 * the air temperature `T` according to:
 *
 * | DR                                           | T range          |
 * | :------------------------------------------: | :--------------: |
 * | `0`                                          | `T <= T_b`       |
 * | `exp(mu) * (T - T_b)^alpha * (T_c - T)^beta` | `T_b < T <= T_c` |
 * | `0`                                          | `T_c < T`        |
 *
 * Here `mu` sets the overall scaling while `alpha` and `beta` determine the shape
 * of the curve. Yin et al. also show that the maximum development rate occurs at
 * an optimal temperature `T_o` defined by
 *
 *     T_o = (alpha * T_c + beta * T_b) / (alpha + beta).
 *
 * The maximum development rate `R_o` that occurs at this temperature is given by
 *
 *     R_o = exp(mu) *
 *           alpha^alpha *
 *           beta^beta *
 *           [(T_c - T_b) / (alpha + beta)]^(alpha + beta).
 *
 * This model can be characterized as a nonlinear model having three cardinal temperatures.
 * Note that although only two temperatures are directly specified (`T_b` and `T_c`), an
 * optimal temperature nevertheless exists and is determined by the values of `alpha` and
 * `beta`. In this sense it is most directly comparable to the thermal_time_bilinear model.
 * However, there is a range of temperatures where the development rate is close to its
 * maximum value and there are actually four parameters that define the shape of the curve,
 * so this model also has some similarity to the thermal_time_trilinear model.
 *
 * ### Suggestion for choosing a value for the scaling factor
 *
 * Taking inspiration from the thermal_time_bilinear and thermal_time_trilinear models,
 * we can require that the maximum development rate in units of `degrees C` should be given
 * by the difference between the optimal temperature and the temperature below which
 * development cannot occur. Using Yin et al.'s notation, we have:
 *
 *     R_o = (T_o - T_b) / 24.0,
 *
 * where this rate is given in units of `degrees C * day / hr`. In this case,
 * we can use the formula for `R_o` shown above to solve for `exp(mu)`:
 *
 *     exp(mu) = (T_o - T_b) /
 *               alpha^alpha /
 *               beta^beta /
 *               [(T_c - T_b) / (alpha + beta)]^(alpha + beta)) /
 *               24.0,
 *
 * where `exp(mu)` has units of `degrees C * day / hr` and `T_o` is defined as above.
 *
 * ### BioCro module implementation
 *
 * In their paper, Yin et al. report the development rate in units of `day^-1`. However,
 * due to the scaling factor in the formula for development rate, the choice of units is
 * rather arbitrary. For simplicity and for consistency with other thermal time modules
 * available in BioCro, we replace `exp(mu)` with a scaling factor ``'ttc_scale'`` having
 * units of `degrees C * day / hr` and treat the other two factors in the formula for `DR`
 * as being dimensionless. Thus, the overall development rate has the correct units of
 * `degrees C * day / hr`.
 *
 * Additionally, we use the following names for the remaining parameters in this model:
 * - ``'TTc'`` for the thermal time `t_th`
 * - ``'temp'`` for the air temperature `T`
 * - ``'tbase'`` for the lower temperature `T_b`
 * - ``'tmax'`` for the upper temperature `T_c`
 * - ``'talpha'`` for the shape factor `alpha`
 * - ``'tbeta'`` for the shape factor `beta`
 */
class thermal_time_beta : public differential_module
{
   public:
    thermal_time_beta(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          time{get_input(input_quantities, "time")},
          sowing_time{get_input(input_quantities, "sowing_time")},
          temp{get_input(input_quantities, "temp")},
          tbase{get_input(input_quantities, "tbase")},
          tmax{get_input(input_quantities, "tmax")},
          talpha{get_input(input_quantities, "talpha")},
          tbeta{get_input(input_quantities, "tbeta")},
          ttc_scale{get_input(input_quantities, "ttc_scale")},

          // Get pointers to output quantities
          TTc_op{get_op(output_quantities, "TTc")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_beta"; }

   private:
    // References to input quantities
    double const& time;
    double const& sowing_time;
    double const& temp;
    double const& tbase;
    double const& tmax;
    double const& talpha;
    double const& tbeta;
    double const& ttc_scale;

    // Pointers to output quantities
    double* TTc_op;

    // Main operation
    void do_operation() const;
};

string_vector thermal_time_beta::get_inputs()
{
    return {
        "time",         // days
        "sowing_time",  // days
        "temp",         // degrees C
        "tbase",        // degrees C
        "tmax",         // degrees C
        "talpha",       // dimensionless
        "tbeta",        // dimensionless
        "ttc_scale"     // degrees C * day / hr
    };
}

string_vector thermal_time_beta::get_outputs()
{
    return {
        "TTc"  // degrees C * day / hr
    };
}

void thermal_time_beta::do_operation() const
{
    // Find the rate of change
    double const rate = time < sowing_time ? 0.0
                        : temp <= tbase    ? 0.0
                        : temp <= tmax     ? ttc_scale * pow(temp - tbase, talpha) * pow(tmax - temp, tbeta)
                                           : 0.0;  // degrees C * day / hr

    // Update the output quantity list
    update(TTc_op, rate);
}

}  // namespace standardBML
#endif
