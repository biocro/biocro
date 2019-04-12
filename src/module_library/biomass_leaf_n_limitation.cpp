#include "biomass_leaf_n_limitation.hpp"

state_map biomass_leaf_n_limitation::do_operation(state_map const &s) const
{
    state_map calculated_state;
    calculated_state["LeafN"] = biomass_leaf_nitrogen_limitation(s);
    return calculated_state;
}

