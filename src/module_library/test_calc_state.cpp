#include "test_calc_state.hpp"

state_map test_calc_state::do_operation(state_map const &s) const
{
    state_map new_state;
    new_state["useless_parameter"] = s.at("LeafArea") * s.at("parameter");

    return new_state;
}
