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

#endif
