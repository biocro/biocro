#include "velocity_oscillator.hpp"

state_map velocity_oscillator::do_operation(state_map const &s) const
{
    return { {"velocity", -s.at("position") } };
}
