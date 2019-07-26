#include "gamma_oscillator.hpp"

state_map gamma_oscillator::do_operation(state_map const &s) const
{
    return { {"gamma", 0 } };
}
