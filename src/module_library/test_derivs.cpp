#include "test_derivs.hpp"

state_map test_derivs::do_operation(state_map const &s) const
{
    state_map derivs;
    derivs["CarbonAvailableForGrowth"] = s.at("LeafArea") * s.at("PAR");

    return derivs;
}
