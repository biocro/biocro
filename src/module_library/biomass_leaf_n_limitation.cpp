#include "biomass_leaf_n_limitation.hpp"

state_map biomass_leaf_n_limitation::do_operation(state_map const &s) const
{
    state_map calculated_state;
    calculated_state["LeafN"] = biomass_leaf_nitrogen_limitation(s);
    return calculated_state;
}

double biomass_leaf_nitrogen_limitation(state_map const &s)
{
    double leaf_n = s.at("LeafN_0") * pow(s.at("Leaf") + s.at("Stem"), -s.at("kln"));
    return (leaf_n > s.at("LeafN_0") ? s.at("LeafN_0") : leaf_n);
}
