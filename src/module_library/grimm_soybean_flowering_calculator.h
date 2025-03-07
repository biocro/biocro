#ifndef GRIMM_SOYBEAN_FLOWERING_CALCULATOR_H
#define GRIMM_SOYBEAN_FLOWERING_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class grimm_soybean_flowering_calculator
 *
 * @brief Model for soybean development and flowering based on Grimm et al.
 * (1993)
 *
 * This module is intended to be used to model soybean development and flowering
 * following the model introduced by Grimm et al. (1993) and further
 * investigated by Piper et al. (1996).
 *
 * Once the day of year exceeds the sowing date, the plant is assumed to be in a
 * photoperiod-insensitive juvenile stage where physiological days accumulate at
 * a rate solely determined by temperature. Here the temperature dependence is
 * given by a piecewise-linear function defined by four critical temperature
 * values.
 *
 * Once the plant's physiological age exceeds a threshold value, it enters a
 * photoperiod-sensitive stage where physiological days accumulate at a rate
 * determined by both temperature and night length. Here the overall rate is the
 * product of two factors:
 *
 * > `R = F(T) * F(N)`
 *
 * where `F(T)` is a linear-plateau function of temperature with two critical
 * temperature values and `F(N)` is a linear-plateau function of night length
 * with two critical night length values.
 *
 * When the physiological age exceeds a third threshold, the plant begins to
 * flower (R1). Here this is represented by the quantity `grimm_flowering,`
 * whose value becomes nonzero when the plant begins to flower.
 *
 * The Grimm model cites the method from Goudriaan (1978) for calculating day
 * length. Unfortunately the Goudriaan paper includes two day lengths: the
 * astronomical and effective day lengths. The Grimm model does not explicitly
 * mention the effective day length, which seems to be used in Goudriaan for
 * calculating average carbon assimilation by a plant canopy. Here we assume the
 * Grimm model requires the astronomical day length.
 *
 * References:
 * - Grimm, S. S., Jones, J. W., Boote, K. J. & Hesketh, J. D. Parameter
 *   Estimation for Predicting Flowering Date of Soybean Cultivars. Crop Science
 *   33, 137–144 (1993)
 *
 * - Piper, E. L., Boote, K. J., Jones, J. W. & Grimm, S. S. Comparison of Two
 *   Phenology Models for Predicting Flowering and Maturity Date of Soybean.
 *   Crop Science 36, 1606–1614 (1996)
 *
 * - Goudriaan, J. & Van Laar, H. Calculation of daily totals of the gross CO2
 *   assimilation of leaf canopies. Netherlands Journal of Agricultural Science
 *   26, 373–382 (1978)
 */
class grimm_soybean_flowering_calculator : public direct_module
{
   public:
    grimm_soybean_flowering_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          sowing_fractional_doy{get_input(input_quantities, "sowing_fractional_doy")},
          grimm_physiological_age{get_input(input_quantities, "grimm_physiological_age")},
          grimm_juvenile_T0{get_input(input_quantities, "grimm_juvenile_T0")},
          grimm_juvenile_T1{get_input(input_quantities, "grimm_juvenile_T1")},
          grimm_juvenile_T2{get_input(input_quantities, "grimm_juvenile_T2")},
          grimm_juvenile_T3{get_input(input_quantities, "grimm_juvenile_T3")},
          grimm_juvenile_pd_threshold{get_input(input_quantities, "grimm_juvenile_pd_threshold")},
          grimm_T_min{get_input(input_quantities, "grimm_T_min")},
          grimm_T_opt{get_input(input_quantities, "grimm_T_opt")},
          grimm_N_min{get_input(input_quantities, "grimm_N_min")},
          grimm_N_opt{get_input(input_quantities, "grimm_N_opt")},
          grimm_flowering_threshold{get_input(input_quantities, "grimm_flowering_threshold")},
          fractional_doy{get_input(input_quantities, "fractional_doy")},
          temp{get_input(input_quantities, "temp")},
          day_length{get_input(input_quantities, "day_length")},

          // Get pointers to output quantities
          grimm_rate_op{get_op(output_quantities, "grimm_rate")},
          grimm_rate_temperature_op{get_op(output_quantities, "grimm_rate_temperature")},
          grimm_rate_photoperiod_op{get_op(output_quantities, "grimm_rate_photoperiod")},
          grimm_flowering_op{get_op(output_quantities, "grimm_flowering")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "grimm_soybean_flowering_calculator"; }

   private:
    // References to input quantities
    double const& sowing_fractional_doy;
    double const& grimm_physiological_age;
    double const& grimm_juvenile_T0;
    double const& grimm_juvenile_T1;
    double const& grimm_juvenile_T2;
    double const& grimm_juvenile_T3;
    double const& grimm_juvenile_pd_threshold;
    double const& grimm_T_min;
    double const& grimm_T_opt;
    double const& grimm_N_min;
    double const& grimm_N_opt;
    double const& grimm_flowering_threshold;
    double const& fractional_doy;
    double const& temp;
    double const& day_length;

    // Pointers to output quantities
    double* grimm_rate_op;
    double* grimm_rate_temperature_op;
    double* grimm_rate_photoperiod_op;
    double* grimm_flowering_op;

    // Main operation
    void do_operation() const;
};

string_vector grimm_soybean_flowering_calculator::get_inputs()
{
    return {
        "sowing_fractional_doy",        // days
        "grimm_physiological_age",      // physiological days
        "grimm_juvenile_T0",            // degrees C
        "grimm_juvenile_T1",            // degrees C
        "grimm_juvenile_T2",            // degrees C
        "grimm_juvenile_T3",            // degrees C
        "grimm_juvenile_pd_threshold",  // physiological days
        "grimm_T_min",                  // degrees C
        "grimm_T_opt",                  // degrees C
        "grimm_N_min",                  // hours
        "grimm_N_opt",                  // hours
        "grimm_flowering_threshold",    // physiological days
        "fractional_doy",               // days
        "temp",                         // degrees C
        "day_length"                    // hours
    };
}

string_vector grimm_soybean_flowering_calculator::get_outputs()
{
    return {
        "grimm_rate",              // physiological days / hr
        "grimm_rate_temperature",  // physiological days / day
        "grimm_rate_photoperiod",  // physiological days / day
        "grimm_flowering"          // dimensionless
    };
}

void grimm_soybean_flowering_calculator::do_operation() const
{
    // Calculate the night length
    const double night_length = 24.0 - day_length;  // hours

    // Calculate the normalized growth rate based on the day of year, night
    // length, temperature, and growth stage
    double rate_normalized = 0.0;              // dimensionless
    double temperature_rate_normalized = 0.0;  // dimensionless
    double photoperiod_rate_normalized = 0.0;  // dimensionless
    if (fractional_doy < sowing_fractional_doy) {
        // The plant has not been sowed yet, so it can't grow
        rate_normalized = 0.0;
    } else if (grimm_physiological_age < grimm_juvenile_pd_threshold) {
        // The plant is still in the juvenile (photoperiod-insensitive) phase
        if (temp < grimm_juvenile_T0) {
            // The temperature is too low for the plant to grow
            rate_normalized = 0.0;
        } else if (temp < grimm_juvenile_T1) {
            // There is a linear increase from 0 to 1 as temp changes from T0 to
            // T1
            rate_normalized = (temp - grimm_juvenile_T0) /
                              (grimm_juvenile_T1 - grimm_juvenile_T0);
        } else if (temp < grimm_juvenile_T2) {
            // We are in the flat region at maximum growth
            rate_normalized = 1.0;
        } else if (temp < grimm_juvenile_T3) {
            // There is a linear decrease from 1 to 0 as temp changes from T2 to
            // T3
            rate_normalized = 1.0 - (temp - grimm_juvenile_T2) /
                                        (grimm_juvenile_T3 - grimm_juvenile_T2);
        } else {
            // The temperature is too high for the plant to grow
            rate_normalized = 0.0;
        }
    } else {
        // The plant has entered the photoperiod-sensitive phase
        if (temp < grimm_T_min) {
            // The temperature is too low for the plant to develop
            temperature_rate_normalized = 0.0;
        } else if (temp < grimm_T_opt) {
            // There is a linear increase from 0 to 1 as temp changes from T_min
            // to T_opt
            temperature_rate_normalized = (temp - grimm_T_min) /
                                          (grimm_T_opt - grimm_T_min);
        } else {
            // The development rate is maxed out at 1
            temperature_rate_normalized = 1.0;
        }
        if (night_length < grimm_N_min) {
            // The night is too short for the plant to develop
            photoperiod_rate_normalized = 0.0;
        } else if (night_length < grimm_N_opt) {
            // There is a linear increase from 0 to 1 as night_length changes
            // from N_min to N_opt
            photoperiod_rate_normalized = (night_length - grimm_N_min) /
                                          (grimm_N_opt - grimm_N_min);
        } else {
            // The development rate is maxed out at 1
            photoperiod_rate_normalized = 1.0;
        }
        rate_normalized = temperature_rate_normalized * photoperiod_rate_normalized;
    }

    // Decide if the plant is flowering
    const bool is_flowering =
        (grimm_physiological_age > grimm_juvenile_pd_threshold + grimm_flowering_threshold);

    // Scale the rates to the maximum value, which is 1 physiological day per
    // day, equivalent to 1 / 24 physiological days per hour
    const double rate_per_hour = rate_normalized / 24.0;
    const double temperature_rate_per_hour = temperature_rate_normalized / 24.0;
    const double photoperiod_rate_per_hour = photoperiod_rate_normalized / 24.0;

    // Update the output quantity list
    update(grimm_rate_op, rate_per_hour);
    update(grimm_rate_temperature_op, temperature_rate_per_hour);
    update(grimm_rate_photoperiod_op, photoperiod_rate_per_hour);
    update(grimm_flowering_op, is_flowering);
}

}  // namespace standardBML
#endif
