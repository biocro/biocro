#ifndef ED_RH_TO_MOLE_FRACTION_H
#define ED_RH_TO_MOLE_FRACTION_H

#include "../module.h"
#include "../state_map.h"
#include <cmath>           // for fabs
#include "../constants.h"  // for eps_zero
#include "AuxBioCro.h"     // for saturation_vapor_pressure

/**
 * @class ed_rh_to_mole_fraction
 *
 * @brief Just converts relative humidity values from weather data into atmospheric
 * mole fractions of water vapor. Currently only intended for use by Ed.
 *
 * Here we just use the definitions of mole fraction and relative humidity:
 *
 * mole_fraction = partial_pressure / atmospheric_pressure
 *               = saturation_water_vapor_pressure * relative_humidity / atmospheric_pressure
 */
class ed_rh_to_mole_fraction : public direct_module
{
   public:
    ed_rh_to_mole_fraction(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          temperature_air_ip(get_ip(input_quantities, "temp")),
          relative_humidity_atmosphere_ip(get_ip(input_quantities, "rh")),

          // Get pointers to output quantities
          mole_fraction_h2o_atmosphere_op(get_op(output_quantities, "mole_fraction_h2o_atmosphere"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_rh_to_mole_fraction"; }

   private:
    // Pointers to input quantities
    const double* atmospheric_pressure_ip;
    const double* temperature_air_ip;
    const double* relative_humidity_atmosphere_ip;

    // Pointers to output quantities
    double* mole_fraction_h2o_atmosphere_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_rh_to_mole_fraction::get_inputs()
{
    return {
        "atmospheric_pressure",  // Pa
        "temp",                  // deg. C
        "rh"                     // dimensionless from Pa / Pa
    };
}

string_vector ed_rh_to_mole_fraction::get_outputs()
{
    return {
        "mole_fraction_h2o_atmosphere"  // dimensionless from mol / mol
    };
}

void ed_rh_to_mole_fraction::do_operation() const
{
    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"atmospheric_pressure cannot be zero", fabs(*atmospheric_pressure_ip) < calculation_constants::eps_zero},  // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(mole_fraction_h2o_atmosphere_op, *relative_humidity_atmosphere_ip * saturation_vapor_pressure(*temperature_air_ip) / *atmospheric_pressure_ip);
}

#endif
