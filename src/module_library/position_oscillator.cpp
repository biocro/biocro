#include "position_oscillator.hpp"

state_map position_oscillator::do_operation(state_map const &s) const
{
    return { {"position", s.at("velocity") } };
}
