#ifndef ED_RH_TO_MOLE_FRACTION_H
#define ED_RH_TO_MOLE_FRACTION_H

#include "../modules.h"
#include "AuxBioCro.h"  // for saturation_vapor_pressure

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
class ed_rh_to_mole_fraction : public SteadyModule
{
   public:
    ed_rh_to_mole_fraction(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_rh_to_mole_fraction"),
          // Get pointers to input parameters
          atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          relative_humidity_atmosphere_ip(get_ip(input_parameters, "rh")),
          // Get pointers to output parameters
          mole_fraction_h2o_atmosphere_op(get_op(output_parameters, "mole_fraction_h2o_atmosphere"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* atmospheric_pressure_ip;
    const double* temperature_air_ip;
    const double* relative_humidity_atmosphere_ip;
    // Pointers to output parameters
    double* mole_fraction_h2o_atmosphere_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_rh_to_mole_fraction::get_inputs()
{
    return {
        "atmospheric_pressure",  // Pa
        "temp",                  // deg. C
        "rh"                     // dimensionless from Pa / Pa
    };
}

std::vector<std::string> ed_rh_to_mole_fraction::get_outputs()
{
    return {
        "mole_fraction_h2o_atmosphere"  // dimensionless from mol / mol
    };
}

void ed_rh_to_mole_fraction::do_operation() const
{
    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"atmospheric_pressure cannot be zero", *atmospheric_pressure_ip == 0},  // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(mole_fraction_h2o_atmosphere_op, *relative_humidity_atmosphere_ip * saturation_vapor_pressure(*temperature_air_ip) / *atmospheric_pressure_ip);
}

#endif
