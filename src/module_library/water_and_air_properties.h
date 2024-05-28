#ifndef WATER_AND_AIR_PROPERTIES_H
#define WATER_AND_AIR_PROPERTIES_H

#include <cmath>                     // for pow, exp
#include "../framework/constants.h"  // for ideal_gas_constant,
                                     // molar_mass_of_dry_air, celsius_to_kelvin

/**
 *  @brief Determine the volume of one mole of an ideal gas using the ideal gas
 *  law.
 *
 *  The ideal gas law is: `PV = nRT`. Solving for `V/n`, we have `V/n = RT / P`.
 *
 *  The ideal gas constant `R` has units `J / K / mol`, so the molar volume has
 *  units `J / mol / Pa`. However, note that `1 Pa = 1 N / m^2` and
 *  `1 J = 1 N * m`, so
 *  `1 J / mol / Pa = 1 N * m / mol / (N / m^2) = 1 m^3 / mol`.
 *
 *  @param [in] temperature Gas temperature in degrees C
 *
 *  @param [in] pressure Gas pressure in Pa
 *
 *  @return Molar volume in m^3 / mol
 */
inline double molar_volume(
    double temperature,  // degrees C
    double pressure      // Pa
)
{
    double const TK =
        temperature + conversion_constants::celsius_to_kelvin;  // K

    return physical_constants::ideal_gas_constant * TK / pressure;  // m^3 / mol
}

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
 *  Assuming the air behaves like an ideal gas, its molar volume `MV` (in
 *  m^3 / mol) can be found using the molar_volume() function. Then its density
 *  can be found using the molar mass `MM` of air (in kg / mol) as `MM / MV`.
 *
 *  See https://en.wikipedia.org/wiki/Density_of_air for details.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @param [in] air_pressure Air pressure in Pa
 *
 *  @return Density of dry air in kg / m^3
 */
inline double dry_air_density(
    const double air_temperature,  // degrees C
    const double air_pressure      // Pa
)
{
    double const MV = molar_volume(air_temperature, air_pressure);  // m^3 / mol

    return physical_constants::molar_mass_of_dry_air / MV;  // kg / m^3
}

/**
 *  @brief Determine the latent heat of vaporization for water from its
 *  temperature.
 *
 *  This function appears to be derived from fitting a linear equation to the
 *  values of `lambda` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `lambda` (MJ / kg) |
 *  | :-----------: | :-------------- -: |
 *  |  0            | 2.50               |
 *  |  5            | 2.49               |
 *  | 10            | 2.48               |
 *  | 15            | 2.47               |
 *  | 20            | 2.45               |
 *  | 25            | 2.44               |
 *  | 30            | 2.43               |
 *  | 35            | 2.42               |
 *  | 40            | 2.41               |
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
    return 2501000 + -2372.727 * temperature;  // J / kg
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
 *  @param [in] temperature Water temperature in degrees C
 *
 *  @return Latent heat of vaporization for water in J / kg
 */
inline double water_latent_heat_of_vaporization_henderson(
    double temperature  // degrees C
)
{
    double const TK =
        temperature + conversion_constants::celsius_to_kelvin;  // K

    return 1.91846e6 * pow(TK / (TK - 33.91), 2);  // J / kg
}

/**
 *  @brief Determine the derivative of saturation water vapor density with
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

/**
 *  @brief Determine the specific heat capacity of dry air at constant pressure
 *  (c_p) at a particular value of air pressure.
 *
 *  The Thermophysical Properties of Matter Database provides several functions
 *  for calculating the specific heat of air, which is designated as material
 *  M91 in the database (https://cindasdata.com/products/tpmd). Here we use
 *  Equation 3, which is described as follows:
 *
 *  > A number of experimental and theoretical values
 *  > are also available for the specific heat in the real gas
 *  > state. Among them, the derived values of Hilsenrath
 *  > et al. (575), which are also compiled in various
 *  > reports (132, 260, 313, 454, 481), are considered to
 *  > be the most reliable in the temperature range from
 *  > 260 K up to 900 K. Therefore, their values are fitted
 *  > to the following equations in the present analysis of
 *  > the real gas specific heat.
 *  >
 *  > For temperatures between 260 K and 610 K:
 *  >
 *  > C(p) (cal g[-1]K[-1]) = 0.249679 - 7.55179 x 10[-5]T +
 *  > 1.69194 x 10[-7]T[2]- 6.46128 x 10[-11]T[3] (T in K). (3)
 *
 *  Note that 260 - 610 K corresponds to approximately -13 to 337 degrees C, so
 *  this equation should be accurate for any temperatures relevant to crop
 *  growth.
 *
 *  Here we use this equation, but convert the result from cal / g / K to
 *  J / kg / K using 1 cal / g = 4184 cal / kg
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Specific heat capacity of dry air in J / kg / K.
 */
inline double TempToCp(
    double air_temperature  // degrees C
)
{
    double const TK =
        air_temperature + conversion_constants::celsius_to_kelvin;  // K

    double const cp = 0.249679 - 7.55179e-5 * TK + 1.69194e-7 * pow(TK, 2) -
                      6.46128e-11 * pow(TK, 3);  // cal / g / K

    return cp * 4184;  // J / kg / K
}
#endif
