#ifndef RH_TO_MOLE_FRACTION_H
#define RH_TO_MOLE_FRACTION_H

#include <cmath>                       // for std::abs
#include "water_and_air_properties.h"  // for saturation_vapor_pressure
#include "../framework/constants.h"    // for calculation_constants::eps_zero
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class rh_to_mole_fraction
 *
 * @brief Converts atmospheric relative humidity into a water vapor
 * concentration expressed as a dimensionless mole fraction.
 *
 * Relative humidity (`RH`) is defined as the ratio of the partial pressure of
 * water vapor in air (`WVP`) to the saturation water vapor pressure of the air
 * (`SWVP`): `RH = WVP / SWVP`.
 *
 * Molar water vapor concentration (`XW`) is defined as the number of moles of
 * water vapor divided by the total number of moles of gas molecules in a
 * mixture. For an ideal gas, it can be shown that this ratio is equivalent to
 * the ratio of `WVP` to the total air pressure (`AP`): `XW = WVP / AP`.
 *
 * Combining these two equations, it is possible to calculate `XW` from `RH`,
 * `SWVP`, and `AP`: `XW = RH * SWVP / AP`.
 *
 * For more info, see [Campbell, G. S. & Norman, J. M. "Chapter 3: Water Vapor
 * and Other Gases" in "An Introduction to Environmental Biophysics" (1998)]
 * (https://dx.doi.org/10.1007/978-1-4612-1626-1_2).
 *
 * In BioCro and in the code below, these quantities are represented as follows:
 * - ``'rh'`` for the local atmospheric relative humidity `RH`
 * - ``'atmospheric_pressure'`` for the local atmospheric pressure `AP`
 * - ``'saturation_water_vapor_pressure_atmosphere'`` for the saturation water
 *   vapor pressure in the local atmosphere `SWVP`
 *
 */
class rh_to_mole_fraction : public direct_module
{
   public:
    rh_to_mole_fraction(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          rh{get_input(input_quantities, "rh")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          saturation_water_vapor_pressure_atmosphere{get_input(input_quantities, "saturation_water_vapor_pressure_atmosphere")},

          // Get pointers to output quantities
          mole_fraction_h2o_atmosphere_op{get_op(output_quantities, "mole_fraction_h2o_atmosphere")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "rh_to_mole_fraction"; }

   private:
    // References to input quantities
    double const& rh;
    double const& atmospheric_pressure;
    double const& saturation_water_vapor_pressure_atmosphere;

    // Pointers to output quantities
    double* mole_fraction_h2o_atmosphere_op;

    // Main operation
    void do_operation() const;
};

string_vector rh_to_mole_fraction::get_inputs()
{
    return {
        "rh",                                          // dimensionless
        "atmospheric_pressure",                        // Pa
        "saturation_water_vapor_pressure_atmosphere",  // Pa
    };
}

string_vector rh_to_mole_fraction::get_outputs()
{
    return {
        "mole_fraction_h2o_atmosphere"  // dimensionless
    };
}

void rh_to_mole_fraction::do_operation() const
{
    // Check for possible error conditions:
    // atmospheric_pressure == 0 causes a division by zero
    std::map<std::string, bool> errors_to_check = {
        {"atmospheric_pressure cannot be zero",
         std::abs(atmospheric_pressure) < calculation_constants::eps_zero}};

    check_error_conditions(errors_to_check, get_name());

    update(mole_fraction_h2o_atmosphere_op,
           rh * saturation_water_vapor_pressure_atmosphere /
               atmospheric_pressure);  // dimensionless
}

}  // namespace standardBML
#endif
