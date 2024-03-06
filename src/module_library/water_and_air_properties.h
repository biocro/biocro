#ifndef WATER_AND_AIR_PROPERTIES_H
#define WATER_AND_AIR_PROPERTIES_H

#include <cmath>                     // for pow, exp
#include "../framework/constants.h"  // for ideal_gas_constant,
                                     // molar_mass_of_dry_air

/**
 * @brief Determine saturation water vapor pressure (Pa) from air temperature
 * (degrees C) using the Arden Buck equation.
 *
 * Equations of this form were used by Arden Buck to model saturation water
 * vapor pressure in 1981: [A. L. Buck, J. Appl. Meteor. 20, 1527–1532 (1981)]
 * (https://doi.org/10.1175/1520-0450(1981)020%3C1527:NEFCVP%3E2.0.CO;2)
 *
 * In 1996, some of the fitting coefficients were updated. Here we use these
 * updated values, found in [a Buck hygrometer manual]
 * (https://www.hygrometers.com/wp-content/uploads/CR-1A-users-manual-2009-12.pdf),
 * and also displayed on the [Wikipedia page for the Arden Buck equation]
 * (https://en.wikipedia.org/wiki/Arden_Buck_equation).
 *
 * We use the values for vapor over water (rather than ice) and disregard the
 * "enhancement factor" since we are only concerned with the pressure at
 * saturation.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Saturation water vapor pressure in Pa
 */
inline double saturation_vapor_pressure(
    double air_temperature  // degrees C
)
{
    double a = (18.678 - air_temperature / 234.5) * air_temperature;
    double b = 257.14 + air_temperature;
    return 611.21 * exp(a / b);  // Pa
}

/**
 *  @brief Determines the density of dry air from the air temperature.
 *
 *  This function appears to be derived from fitting a linear equation to the
 *  values of `rho` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `rho` (kg / m^3) |
 *  | :-----------: | :--------------: |
 *  |  0            | 1.29             |
 *  |  5            | 1.27             |
 *  | 10            | 1.25             |
 *  | 15            | 1.23             |
 *  | 20            | 1.20             |
 *  | 25            | 1.18             |
 *  | 30            | 1.16             |
 *  | 35            | 1.15             |
 *  | 40            | 1.13             |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Density of dry air in kg / m^3
 */
inline double TempToDdryA(
    double air_temperature  // degrees C
)
{
    return 1.295163636 + -0.004258182 * air_temperature;  // kg / m^3
}

/**
 *  @brief Calculate the density of dry air from temperature and pressure using
 *  the ideal gas law.
 *
 *  See https://en.wikipedia.org/wiki/Density_of_air for details.
 *
 *  @param [in] air_temperature Air temperature in kelvin
 *
 *  @param [in] air_pressure Air pressure in Pa
 *
 *  @return Density of dry air in kg / m^3
 */
inline double dry_air_density(
    const double air_temperature,  // kelvin
    const double air_pressure      // Pa
)
{
    constexpr double r_specific = physical_constants::ideal_gas_constant /
                                  physical_constants::molar_mass_of_dry_air;  // J / kg / K

    return air_pressure / r_specific / air_temperature;  // kg / m^3
}

/**
 *  @brief Determine the latent heat of vaporization for water from its
 *  temperature.
 *
 *  This function appears to be derived from fitting a linear equation to the
 *  values of `lambda` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `lambda` (J / kg) |
 *  | :-----------: | :---------------: |
 *  |  0            | 2.50              |
 *  |  5            | 2.49              |
 *  | 10            | 2.48              |
 *  | 15            | 2.47              |
 *  | 20            | 2.45              |
 *  | 25            | 2.44              |
 *  | 30            | 2.43              |
 *  | 35            | 2.42              |
 *  | 40            | 2.41              |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] temperature Water temperature in degrees C
 *
 *  @return Latent heat of vaporization for water in J / kg
 */
inline double TempToLHV(
    double temperature  // degrees C
)
{
    return 2501000 + -2372.727 * temperature;  // J / kg.
}

/**
 *  @brief Determine the latent heat of vaporization of water from its
 *  temperature.
 *
 *  Here we use Equation 8 from
 *  [Henderson-Sellers, B. "A new formula for latent heat of vaporization of water as a function of
 *  temperature" Quarterly Journal of the Royal Meteorological Society 110, 1186–1190 (1984)]
 *  (https://doi.org/10.1002/qj.49711046626)
 *
 *  @param [in] temperature Water temperature in kelvin
 *
 *  @return Latent heat of vaporization for water in J / kg
 */
inline double water_latent_heat_of_vaporization_henderson(
    double temperature)  // kelvin
{
    return 1.91846e6 * pow(temperature / (temperature - 33.91), 2);  // J / kg
}

/**
 *  @brief Determine the derivative of saturation water vapor pressure with
 *  respect to temperature at a particular value of air temperature.
 *
 *  This function appears to be derived from fitting a quadratic function to the
 *  values of `s` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `s` (10^(-3) kg / m^3 / K) |
 *  | :-----------: | :------------------------: |
 *  |  0            | 0.33                       |
 *  |  5            | 0.45                       |
 *  | 10            | 0.60                       |
 *  | 15            | 0.78                       |
 *  | 20            | 1.01                       |
 *  | 25            | 1.30                       |
 *  | 30            | 1.65                       |
 *  | 35            | 2.07                       |
 *  | 40            | 2.57                       |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Derivative of saturation water vapor pressure with respect to
 *  temperature in kg / m^3 / K (equivalent to Pa / K)
 */
inline double TempToSFS(
    double air_temperature  // degrees C
)
{
    return (0.338376068 + 0.011435897 * air_temperature + 0.001111111 * pow(air_temperature, 2)) * 1e-3;  //  kg / m^3 / K
}

#endif
