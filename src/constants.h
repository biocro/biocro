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
const double eps_deriv = 1e-11;
} // namespace calculation_constants

namespace physical_constants
{
const double stefan_boltzmann = 5.670374419e-8;      // W / m^2 / K^4
const double celsius_to_kelvin = 273.15;             // deg. C or deg. K
const double ideal_gas_constant = 8.31446261815324;  // J / K / mol
}  // namespace physical_constants

#endif
