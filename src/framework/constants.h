#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <boost/math/constants/constants.hpp>

/**
 * @brief Mathematical constants
 */
namespace math_constants
{
using boost::math::double_constants::e;
using boost::math::double_constants::pi;
}  // namespace math_constants

/**
 * @brief Convenience constants used in BioCro calculations
 */
namespace calculation_constants
{
/// A relative step size for numerically calculating derivatives.
constexpr double eps_deriv = 1e-5;
/// A small number for checking whether a double is zero.
constexpr double eps_zero = 1e-13;
}  // namespace calculation_constants

/**
 *  @brief These conversion constants are collected here for convenience and
 *  clarity
 */
namespace conversion_constants
{
constexpr double celsius_to_kelvin = 273.15;  // deg. C or deg. K
constexpr double joules_per_calorie = 4.184;  // J / cal
}  // namespace conversion_constants

/**
 * @brief Various physical constants are collected here.
 *
 * These physical constants may be subdivided according to how
 * "constant" they are:
 *
 * - `stefan_boltzmann` and `ideal_gas_constant`: These are truly
 * constant, and as of the 2019 redefinition of SI base units, they
 * have exact numerical values when expressed in SI units.
 *
 * - `molar_mass_of_water`: This depends on the relative frequency of
 * the water's isotopologues, but for practical purposes, and because
 * this composition doesn't usually vary greatly, we treat it as
 * constant with the approximate value given.
 *
 * - `atmospheric_pressure_at_sea_level`: More precisely, this is
 * **standard sea-level pressure**, which has the value given here _by
 * definition_ and represents the near-average atmospheric pressure at
 * sea level on the Earth.  The actual atmospheric pressure at any
 * sea-level location changes continuously over time and may vary
 * considerably from this.
 */
namespace physical_constants
{
constexpr double stefan_boltzmann = 5.670374419e-8;      // W / m^2 / K^4
constexpr double ideal_gas_constant = 8.31446261815324;  // J / K / mol
constexpr double molar_mass_of_water = 18.01528e-3;      // kg / mol
constexpr double molar_mass_of_glucose = 180.156e-3;     // kg / mol
constexpr double molar_mass_of_dry_air = 28.9647e-3;     // kg / mol (https://www.engineeringtoolbox.com/molecular-mass-air-d_679.html)

constexpr double atmospheric_pressure_at_sea_level = 101325;  // Pa
}  // namespace physical_constants

#endif
