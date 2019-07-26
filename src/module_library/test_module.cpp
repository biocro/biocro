#include "test_module.hpp"

state_map test_module::do_operation (state_map const& s) const
{
    // As an alternative to using [], you can initialize the state_map
    // directly, which can look nicer if the calculations are simple.
    state_map result {
        {"first",  19.5},  // units
    };

    return result;
}
