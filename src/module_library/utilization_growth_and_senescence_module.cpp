#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "utilization_growth_and_senescence_module.hpp"
#include "utilization_growth_module.hpp"
#include "utilization_senescence.hpp"

state_map utilization_growth_and_senescence_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t max_loops = 4;
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);

    std::unique_ptr<IModule> growth_module = std::unique_ptr<IModule>(new utilization_growth_module);
    std::unique_ptr<IModule> senescence_module = std::unique_ptr<IModule>(new utilization_senescence);

    double total_time = s.at("timestep"); // hours
    size_t sub_time_steps = total_time * 60;  // At the start, integrate over one minute intervals.

    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t counter = 0;
    while (true) {
        state_map s_copy = state_map(s);
        state_map total_derivs;

        bool failed = false;
        for (size_t i = 0; i < sub_time_steps; ++i) {
            double d_time = total_time / sub_time_steps;

            state_map sub_derivs = growth_module->run(s_copy);
            sub_derivs += senescence_module->run(s_copy);

            update_state(s_copy, sub_derivs * d_time);
            total_derivs += sub_derivs * d_time;

            // The following conditions are not possible and will not be corrected with further iteration.
            if ((s_copy.at("substrate_pool_leaf") < 0) |
                    (s_copy.at("substrate_pool_stem") < 0) |
                    (s_copy.at("substrate_pool_root") < 0) |
                    (s_copy.at("substrate_pool_rhizome") < 0) |
                    (s_copy.at("Leaf") < 0) |
                    (s_copy.at("Stem") < 0) |
                    (s_copy.at("Root") < 0) |
                    (s_copy.at("Rhizome") < 0) |
                    (s_copy.at("Grain") < 0) |
                    (sub_derivs.at("utilization_stem") < 0) |
                    (sub_derivs.at("utilization_grain") < 0) |
                    (sub_derivs.at("utilization_rhizome") < 0) |
                    (sub_derivs.at("utilization_root") < 0) |
                    (sub_derivs.at("senescence_leaf") < 0) |
                    (sub_derivs.at("senescence_stem") < 0) |
                    (sub_derivs.at("senescence_root") < 0) |
                    (sub_derivs.at("senescence_rhizome") < 0))
            {
                if (counter < max_loops) {  // Abort if the maximum number of loops hasn't been reached. It the maximun number of loops is reached, continue with the caveat that something is wrong.
                    failed = true;
                    break;
                }
            }

        }
        // If iteration failed, increase the number of steps. After some limit, abort the integration early.
        if (failed & (counter < max_loops))
        {
            sub_time_steps = sub_time_steps * 2;
            ++counter;
            continue;
        } else {
            derivs = total_derivs /= total_time;
            break;
        }
    }
    return (derivs);
}

state_map utilization_growth_and_senescence_module::do_operation(state_map const &s) const
{
    std::unique_ptr<IModule> growth_module = std::unique_ptr<IModule>(new utilization_growth_module);
    std::unique_ptr<IModule> senescence_module = std::unique_ptr<IModule>(new utilization_senescence);

    state_map derivs = growth_module->run(s);
    derivs += senescence_module->run(s);

    return derivs;
}
