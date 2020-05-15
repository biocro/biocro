#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <boost/math/constants/constants.hpp>

namespace math_constants {
    using boost::math::double_constants::pi;
    using boost::math::double_constants::e;
}

namespace calculation_constants {
    const double eps_deriv = 1e-11;
}

namespace physical_constants {
    const double stefan_boltzmann = 5.67e-8;    // W / m^2 / K^4
    const double celsius_to_kelvin = 273.15;    // deg. C or deg. K
}

#endif
