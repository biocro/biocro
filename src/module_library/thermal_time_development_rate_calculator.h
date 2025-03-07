#ifndef THERMAL_TIME_DEVELOPMENT_RATE_CALCULATOR_H
#define THERMAL_TIME_DEVELOPMENT_RATE_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class thermal_time_development_rate_calculator
 *
 * @brief Calculates hourly plant development rate based on thermal time ranges
 * of emergence, vegetative, and reproductive growth stages.
 *
 * Based on growth rate functions from Osborne et al. and uses the following
 * `DVI` ranges:
 *
 * | Development index (DVI)         | Growth stages               |
 * | :-----------------------------: | :-------------------------: |
 * | \f$ -1 \le \text{DVI} < 0 \f$   | sowing to emergence         |
 * | \f$  0 \le \text{DVI} < 1 \f$   | vegetative growth stages    |
 * | \f$  1 \le \text{DVI} < 2 \f$   | reproductive growth stages  |
 *
 *
 * The `development_rate`, \f$ r \f$ is defined as:
 *
 * \f[ r = \frac{t-t_\text{base}}{TT_\text{stage}}, \f]
 *
 * where \f$ t \f$ is the temperature (`temp`), \f$ t_\text{base} \f$ is the
 * base temperature (`tbase`), and \f$ TT_\text{stage} \f$ is the accumulated
 * thermal time for each of the above three growth stages (`TTemr`, `TTveg`,
 * and `TTrep`).
 *
 * This module is intended to be used along with the `development_index` module,
 * and it is particularly useful for modeling soybean growth, since it can
 * easily be swapped with the `soybean_development_rate_calculator` module to
 * compare thermal and photothermal development. However, this module specifies
 * a linear thermal time equation identical to the one implemented by the
 * `thermal_time_linear` module, and cannot represent other thermal time
 * equations, such as the one implemented in the `thermal_time_bilinear` module.
 * For more flexibility regarding thermal time calculations, an alternative
 * approach is to use the `development_index_from_thermal_time` module instead.
 * See that module's documentation for more information.
 *
 * ### References:
 *
 *  [Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment
 *  Simulator.” Geoscientific Model Development 8(4): 1139–55.]
 *  (https://doi.org/10.5194/gmd-8-1139-2015)
 */
class thermal_time_development_rate_calculator : public direct_module
{
   public:
    thermal_time_development_rate_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          fractional_doy{get_input(input_quantities, "fractional_doy")},
          sowing_fractional_doy{get_input(input_quantities, "sowing_fractional_doy")},
          DVI{get_input(input_quantities, "DVI")},
          temp{get_input(input_quantities, "temp")},
          tbase{get_input(input_quantities, "tbase")},
          TTemr{get_input(input_quantities, "TTemr")},
          TTveg{get_input(input_quantities, "TTveg")},
          TTrep{get_input(input_quantities, "TTrep")},

          // Get pointers to output quantities
          development_rate_per_hour_op{get_op(output_quantities, "development_rate_per_hour")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "thermal_time_development_rate_calculator"; }

   private:
    // Pointers to input quantities
    double const& fractional_doy;
    double const& sowing_fractional_doy;
    double const& DVI;
    double const& temp;
    double const& tbase;
    double const& TTemr;
    double const& TTveg;
    double const& TTrep;

    // Pointers to output quantities
    double* development_rate_per_hour_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector thermal_time_development_rate_calculator::get_inputs()
{
    return {
        "fractional_doy",         // days
        "sowing_fractional_doy",  // days
        "DVI",                    // dimensionless, development index
        "temp",                   // degrees C
        "tbase",                  // degrees C, base temperature
        "TTemr",                  // degrees C * days, thermal time from sowing to emergence
        "TTveg",                  // degrees C * days, thermal time of vegetative states
        "TTrep"                   // degrees C * days, thermal time of reproductive states
    };
}

string_vector thermal_time_development_rate_calculator::get_outputs()
{
    return {
        "development_rate_per_hour"  // hour^-1
    };
}

void thermal_time_development_rate_calculator::do_operation() const
{
    // Calculate the development_rate
    double development_rate;                      // day^-1
    double temp_diff = temp - tbase;              // degrees C
    temp_diff = (temp_diff > 0) ? temp_diff : 0;  // if temp < tbase, temp_diff = 0

    if (fractional_doy < sowing_fractional_doy) {
        // The seeds haven't been sown yet, so no development should occur
        development_rate = 0;  // day^-1
    } else if (DVI < -1) {
        // error, DVI out of bounds, this should never occur unless initial DVI
        // state is less than -1.
        development_rate = 0;
    } else if (DVI < 0) {
        // 1. Sowing to emergence
        development_rate = temp_diff / TTemr;  // day^-1

    } else if (DVI < 1) {
        // 2. Vegetative stages
        development_rate = temp_diff / TTveg;  // day^-1

    } else {
        // 3. Reproductive Stages
        development_rate = temp_diff / TTrep;  // day^-1
    }

    double development_rate_per_hour = development_rate / 24.0;  // hour^-1

    // Update the output quantity list
    update(development_rate_per_hour_op, development_rate_per_hour);
}

}  // namespace standardBML
#endif
