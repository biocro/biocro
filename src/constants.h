#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <boost/math/constants/constants.hpp>

namespace math_constants
{
using boost::math::double_constants::e;
using boost::math::double_constants::pi;
}  // namespace math_constants

namespace calculation_constants
{
constexpr double eps_deriv = 1e-5;  // a relative step size for numerically calculating derivatives
constexpr double eps_zero = 1e-13;  // a small number for checking whether a double is zero
}  // namespace calculation_constants

namespace conversion_constants
{
/**
 *  These conversion constants are collected here for convenience and clarity
 */
constexpr double celsius_to_kelvin = 273.15;  // deg. C or deg. K
constexpr double joules_per_calorie = 4.184;  // J / cal
}  // namespace conversion_constants

namespace physical_constants
{
/**
 * These physical constants are truly constant everywhere
 * in the universe in all possible conditions, as far as
 * anybody knows.
 */
constexpr double stefan_boltzmann = 5.670374419e-8;      // W / m^2 / K^4
constexpr double ideal_gas_constant = 8.31446261815324;  // J / K / mol
constexpr double molar_mass_of_water = 18.01528e-3;      // kg / mol
constexpr double molar_mass_of_glucose = 180.156e-3;     // kg / mol

/**
 * These constants are very constant, but only on Earth.
 */
constexpr double atmospheric_pressure_at_sea_level = 101325;  // Pa
}  // namespace physical_constants

#endif
