#ifndef ED_BALL_BERRY_H
#define ED_BALL_BERRY_H

#include "../modules.h"
#include "../state_map.h"
#include <cmath>           // for fabs
#include <algorithm>       // for std::max
#include "../constants.h"  // for eps_zero
#include "AuxBioCro.h"     // for saturation_vapor_pressure

/**
 * @class ed_ball_berry
 *
 * @brief Uses the Ball-Berry equation to calculate stomatal conductance to water vapor.
 * Currently only intended for use by Ed.
 *
 * The Ball-Berry model was originally written as:
 *
 * g = slope * A_n * rh_s / C_s + intercept
 *
 * where g is the stomatal conductance to water vapor, A_n is net leaf assimilation,
 * rh_s is relative humidity at the leaf surface, C_s is CO2 mole fraction at the
 * leaf surface, and the slope & intercept are determined by fits to experimental
 * data.
 *
 * However, in BioCro we express water vapor concentration as a mole fraction rather
 * than relative humidity. The two quantities are related by:
 *
 * rh = water_vapor_pressure / saturation_water_vapor_pressure
 *    = mole_fraction_h2o * atmospheric_pressure / saturation_water_vapor_pressure
 *
 * So in this case the Ball-Berry formula becomes:
 *
 * g = slope * A_n * mole_fraction_h2o * atmospheric_pressure / (C_s * saturation_water_vapor_pressure) + intercept
 *
 * The saturation water vapor pressure depends on temperature. Here the leaf temperature is more
 * appropriate than atmospheric air temperature.
 *
 * It is also important to note that when A_n < 0, the Ball-Berry model just predicts its minimum
 * conductance value: the intercept.
 *
 * In BioCro we also may want to include the effects of water stress as a modification to the
 * stomatal conductance. To do this, we treat the Ball-Berry conductance as a maximum value
 * corresponding to ideal water availability. We use a conductance_adjustment_factor to linearly
 * switch between a minimum conductance value and the maximum one from the Ball-Berry model. I.e.,
 * when conductance_adjustment_factor is zero, conductance is the minimum value. When it is one,
 * conductance is the maximum (Ball-Berry) value.
 */
class ed_ball_berry : public SteadyModule
{
   public:
    ed_ball_berry(
        const state_map* input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_ball_berry"),
          // Get pointers to input quantities
          ball_berry_slope_ip(get_ip(input_quantities, "ball_berry_slope")),
          ball_berry_intercept_ip(get_ip(input_quantities, "ball_berry_intercept")),
          assimilation_net_ip(get_ip(input_quantities, "assimilation_net")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          temperature_leaf_ip(get_ip(input_quantities, "temperature_leaf")),
          mole_fraction_h2o_leaf_surface_ip(get_ip(input_quantities, "mole_fraction_h2o_leaf_surface")),
          mole_fraction_co2_leaf_surface_ip(get_ip(input_quantities, "mole_fraction_co2_leaf_surface")),
          conductance_adjustment_factor_WS_ip(get_ip(input_quantities, "conductance_adjustment_factor_WS")),
          conductance_stomatal_h2o_min_ip(get_ip(input_quantities, "conductance_stomatal_h2o_min")),
          // Get pointers to output quantities
          conductance_stomatal_h2o_op(get_op(output_quantities, "conductance_stomatal_h2o"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* ball_berry_slope_ip;
    const double* ball_berry_intercept_ip;
    const double* assimilation_net_ip;
    const double* atmospheric_pressure_ip;
    const double* temperature_leaf_ip;
    const double* mole_fraction_h2o_leaf_surface_ip;
    const double* mole_fraction_co2_leaf_surface_ip;
    const double* conductance_adjustment_factor_WS_ip;
    const double* conductance_stomatal_h2o_min_ip;
    // Pointers to output quantities
    double* conductance_stomatal_h2o_op;
    // Main operation
    void do_operation() const override;
};

string_vector ed_ball_berry::get_inputs()
{
    return {
        "ball_berry_slope",                 // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
        "ball_berry_intercept",             // mol / m^2 / s
        "assimilation_net",                 // mol / m^2 / s
        "atmospheric_pressure",             // Pa
        "temperature_leaf",                 // deg. C
        "mole_fraction_h2o_leaf_surface",   // dimensionless from Pa / Pa
        "mole_fraction_co2_leaf_surface",   // dimensionless from mol / mol
        "conductance_adjustment_factor_WS", // dimensionless
        "conductance_stomatal_h2o_min"      // mol / m^2 / s
    };
}

string_vector ed_ball_berry::get_outputs()
{
    return {
        "conductance_stomatal_h2o"  // mol / m^2 / s
    };
}

void ed_ball_berry::do_operation() const
{
    // Calculate values for the output quantities
    const double ball_berry_index = std::max(0.0, *assimilation_net_ip) * *mole_fraction_h2o_leaf_surface_ip * *atmospheric_pressure_ip /
                                    (*mole_fraction_co2_leaf_surface_ip * saturation_vapor_pressure(*temperature_leaf_ip));

    const double ball_berry_conductance = *ball_berry_slope_ip * ball_berry_index + *ball_berry_intercept_ip;

    const double adjusted_conductance = *conductance_stomatal_h2o_min_ip +
                                        *conductance_adjustment_factor_WS_ip * (ball_berry_conductance - *conductance_stomatal_h2o_min_ip);

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"mole_fraction_co2_leaf_surface cannot be zero",       fabs(*mole_fraction_co2_leaf_surface_ip) < calculation_constants::eps_zero},               // divide by zero
        {"saturation_vapor_pressure in leaf cannot be zero",    fabs(saturation_vapor_pressure(*temperature_leaf_ip)) < calculation_constants::eps_zero}   // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    // Update the output quantity list
    update(conductance_stomatal_h2o_op, adjusted_conductance);
}

#endif
