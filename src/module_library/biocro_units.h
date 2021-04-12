#ifndef BIOCRO_UNITS_H
#define BIOCRO_UNITS_H

#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/codata/physico-chemical_constants.hpp>
#include <boost/units/pow.hpp>
#include <boost/units/cmath.hpp>

namespace biocro_units {

using namespace boost::units;
using namespace boost::units::si;
using namespace boost::units::si::constants::codata;

typedef divide_typeof_helper<energy, amount>::type energy_over_amount;
typedef divide_typeof_helper<amount, boost::units::si::time>::type amount_over_time;  // 'time' must be fully qualified because it is already defined elsewhere.
typedef divide_typeof_helper<amount_over_time, area>::type flux;
typedef divide_typeof_helper<amount, amount>::type mole_fraction;

}

#endif

