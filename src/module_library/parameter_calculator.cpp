#include "parameter_calculator.hpp"

state_map parameter_calculator::do_operation(state_map const &s) const
{
    double const Sp = s.at("iSp") - s.at("TTc") * s.at("Sp_thermal_time_decay");
    state_map new_state {
        { "Sp", Sp },
        { "lai",  s.at("Leaf") * Sp },
        { "vmax",  (s.at("LeafN_0") - s.at("LeafN")) * s.at("vmax_n_intercept") + s.at("vmax1") },
        { "alpha",  (s.at("LeafN_0") - s.at("LeafN")) * s.at("alphab1") + s.at("alpha1") }
    };
    return new_state;
}
