#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "utilization_senescence.hpp"

state_map utilization_senescence::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    size_t max_loops = 3;
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);

    double total_time = s.at("timestep"); // hours
    size_t sub_time_steps = total_time * 60;  // At the start, integrate over each minute.

    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t counter = 0;
    while (true) {
        state_map s_copy = state_map(s);
        state_map total_derivs;

        size_t i;
        bool failed = false;
        for (i = 0; i < sub_time_steps; ++i) {
            double d_time = total_time / sub_time_steps;

            state_map sub_derivs = this->run(s_copy) * d_time;

            update_state(s_copy, sub_derivs);
            total_derivs += sub_derivs;

            // The following conditions are not possible and will not be corrected with futher iteration.
            if ((s_copy.at("substrate_pool_leaf") < 0) |
                (s_copy.at("substrate_pool_stem") < 0) |
                (s_copy.at("substrate_pool_root") < 0) |
                (s_copy.at("substrate_pool_rhizome") < 0) |
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
            derivs = total_derivs;
            break;
        }
    }
    return (derivs);
}

state_map utilization_senescence::do_operation(state_map const &s) const
{
    double TTc = s.at("TTc");

    double remobilization_fraction = s.at("remobilization_fraction");

    double kLeaf = s.at("rate_constant_leaf_senescence");
    double kStem = s.at("rate_constant_stem_senescence");
    double kRoot = s.at("rate_constant_root_senescence");
    double kRhizome = s.at("rate_constant_rhizome_senescence");

    double KmLeaf = s.at("KmLeaf_senescence");
    double KmStem = s.at("KmStem_senescence");
    double KmRoot = s.at("KmRoot_senescence");
    double KmRhizome = s.at("KmRhizome_senescence");

    double seneLeaf = s.at("seneLeaf");
    double seneStem = s.at("seneStem");
    double seneRoot = s.at("seneRoot");
    double seneRhizome = s.at("seneRhizome");

    double grain_TTc = s.at("grain_TTc");

    double Leaf = s.at("Leaf");
    double Stem = s.at("Stem");
    double Grain = s.at("Grain");
    double Root = s.at("Root");
    double Rhizome = s.at("Rhizome");

    double substrate_pool_leaf = s.at("substrate_pool_leaf");
    double substrate_pool_stem = s.at("substrate_pool_stem");
    double substrate_pool_root = s.at("substrate_pool_root");
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome");

    double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0;
    double senescence_leaf = 0, senescence_stem = 0, senescence_root = 0, senescence_rhizome = 0; 

    double start_grain = 0, start_sub = 0;

    if ((Leaf != 0) & (TTc >= seneLeaf)) {
        mass_fraction_leaf = substrate_pool_leaf / Leaf;
        senescence_leaf = mass_fraction_leaf * kLeaf / (KmLeaf + mass_fraction_leaf);
    }
    if ((Stem != 0) & (TTc >= seneStem)) {
        mass_fraction_stem = substrate_pool_stem / Stem;
        senescence_stem = mass_fraction_stem * kStem / (KmStem + mass_fraction_stem);
    }
    if ((Root != 0) & (TTc >= seneRoot)) {
        mass_fraction_root = substrate_pool_root / Root;
        senescence_root = mass_fraction_root * kRoot / (KmRoot + mass_fraction_root);
    }
    if ((Rhizome != 0) & (TTc >= seneRhizome)) {
        mass_fraction_rhizome = substrate_pool_rhizome;
        senescence_rhizome = mass_fraction_rhizome * kRhizome / (KmRhizome + mass_fraction_rhizome);
    }
    if ((Grain <= 0) & (TTc >= grain_TTc) & (substrate_pool_stem > 0)) {
        // Base grain growth on the amount of stem mass. In the derivatives, remove this from soluble carbon in the stem.
        start_grain = substrate_pool_stem * 100;
        start_sub = substrate_pool_stem * start_grain / Stem;
    }

    state_map derivs {
        {"Leaf", -senescence_leaf},
        {"substrate_pool_leaf", senescence_leaf * remobilization_fraction},
        {"LeafLitter", senescence_leaf * (1 - remobilization_fraction)},

        {"Stem", -senescence_stem},
        {"substrate_pool_stem", senescence_stem * remobilization_fraction - start_grain - start_sub},
        {"StemLitter", senescence_stem * (1 - remobilization_fraction)},

        {"Grain", start_grain},
        {"substrate_pool_grain", start_sub},

        {"Root", -senescence_root},
        {"substrate_pool_root", senescence_root * remobilization_fraction},
        {"RootLitter", senescence_root * (1 - remobilization_fraction)},

        {"Rhizome", -senescence_rhizome},
        {"substrate_pool_rhizome", senescence_rhizome * remobilization_fraction},
        {"RhizomeLitter", senescence_rhizome * (1 - remobilization_fraction)},

        {"senescence_leaf", senescence_leaf},
        {"senescence_stem", senescence_stem},
        {"senescence_root", senescence_root},
        {"senescence_rhizome", senescence_rhizome}
    };

    return derivs;
}
