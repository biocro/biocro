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
const double eps_deriv = 1e-5;  // a relative step size for numerically calculating derivatives
const double eps_zero = 1e-13;  // a small number for checking whether a double is zero
}  // namespace calculation_constants

namespace physical_constants
{
/**
 * These physical constants are truly constant everywhere
 * in the universe in all possible conditions, as far as
 * anybody knows.
 */
const double stefan_boltzmann = 5.670374419e-8;      // W / m^2 / K^4
const double celsius_to_kelvin = 273.15;             // deg. C or deg. K
const double ideal_gas_constant = 8.31446261815324;  // J / K / mol
const double molar_mass_of_water = 18.01528e-3;      // kg / mol

/**
 * These constants are very constant, but only on Earth.
 */
const double pressure_at_sea_level = 101325;  // Pa

/**
 * These water properties vary with temperature
 * and pressure, but the changes are not significant
 * across the range of conditions that a plant
 * might encounter.
 */
const double specific_heat_of_water = 1010;  // J / kg / K
}  // namespace physical_constants

#endif
