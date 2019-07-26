#include "ball_berry_module.hpp"
#include "ball_berry.hpp"

state_map ball_berry_module::do_operation(state_map const &s) const
{
    double const stomatal_conductance = ball_berry(s.at("net_assimilation_rate"), s.at("atmospheric_co2_concentration"), s.at("rh"), s.at("b0"), s.at("b1"));
    return { {"leaf_stomatal_conductance", stomatal_conductance } };  // mmol / m^2 / s
}
