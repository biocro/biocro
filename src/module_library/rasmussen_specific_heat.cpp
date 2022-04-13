#include "rasmussen_specific_heat.h"
#include "../framework/constants.h"  // For conversion_constants

using conversion_constants::celsius_to_kelvin;
using conversion_constants::joules_per_calorie;
using standardBML::rasmussen_specific_heat;

double rasmussen_specific_heat_of_air(
    double air_temperature,   // K
    double mole_fraction_h2o  // dimensionless
)
{
    // Define conversion factors uesd to convert the original units to coherent
    // SI units
    double constexpr g_per_kg = 1e3;                      // g / kg
    double constexpr cf = g_per_kg * joules_per_calorie;  // J * g / (cal / kg)

    // Define coefficients for the heat capacity equation
    double constexpr a0 = +2.51625e-01 * cf;  // J / kg / K
    double constexpr a1 = -9.25250e-05 * cf;  // J / kg / K
    double constexpr a2 = +2.13340e-07 * cf;  // J / kg / K
    double constexpr a3 = -1.00430e-10 * cf;  // J / kg / K
    double constexpr a4 = +1.24770e-01 * cf;  // J / kg / K
    double constexpr a5 = -2.28300e-05 * cf;  // J / kg / K
    double constexpr a6 = +1.26700e-07 * cf;  // J / kg / K
    double constexpr a7 = +1.11600e-02 * cf;  // J / kg / K
    double constexpr a8 = +4.61000e-06 * cf;  // J / kg / K
    double constexpr a9 = +1.74000e-08 * cf;  // J / kg / K

    // Calculate the specific heat capacity at constant pressure

    double const b_0 = a0 +
                       a1 * air_temperature +
                       a2 * pow(air_temperature, 2) +
                       a3 * pow(air_temperature, 3);

    double const b_1 = a4 +
                       a5 * air_temperature +
                       a6 * pow(air_temperature, 2);

    double const b_2 = a7 +
                       a8 * air_temperature +
                       a9 * pow(air_temperature, 2);

    return b_0 +
           b_1 * mole_fraction_h2o +
           b_2 * pow(mole_fraction_h2o, 2);  // J / kg / K
}

string_vector rasmussen_specific_heat::get_inputs()
{
    return {
        "temp",                         // degrees C
        "mole_fraction_h2o_atmosphere"  // dimensionless
    };
}

string_vector rasmussen_specific_heat::get_outputs()
{
    return {
        "specific_heat_of_air"  // J / kg / K
    };
}

void rasmussen_specific_heat::do_operation() const
{
    // Convert temperature to Kelvin. (Note: this step will be unnecessary once
    // inputs have been standardized to coherent SI units and air temperature is
    // always given in Kelvin)
    double const air_temperature_kelvin = temp + celsius_to_kelvin;  // K

    // Update the output quantity list
    update(specific_heat_of_air_op,
           rasmussen_specific_heat_of_air(
               air_temperature_kelvin,
               mole_fraction_h2o_atmosphere));  // J / kg / K
}
