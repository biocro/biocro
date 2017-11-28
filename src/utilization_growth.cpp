#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"

//static int counter = 0;

state_map utilization_growth_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    size_t max_loops = 3;
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);

    double total_time = p.at("timestep"); // hours
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

            state_map sub_derivs = this->run(s) * d_time;

            s_copy += sub_derivs;
            total_derivs += sub_derivs;

            // The following conditions are not possible and will not be corrected with futher iteration.
            if ((s_copy.at("substrate_pool_leaf") < 0) |
                (s_copy.at("substrate_pool_stem") < 0) |
                (s_copy.at("substrate_pool_root") < 0) |
                (s_copy.at("substrate_pool_rhizome") < 0) |
                (s_copy.at("substrate_pool_grain") < 0) |
                (sub_derivs.at("utilization_stem") < 0) |
                (sub_derivs.at("utilization_root") < 0) |
                (sub_derivs.at("utilization_rhizome") < 0) |
                (sub_derivs.at("utilization_grain") < 0))
            {
                if (counter < max_loops) {  // Abort if the maximum number of loops hasn't been reached. Otherwise, continue knowing it won't provide the right solution.
                    failed = true;
                    break;
                }
            }

        }
        // If iteration failed, increase the number of steps. After that limit, abort the integration early.
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

state_map utilization_growth_module::do_operation(state_map const &s) const
{
    double grain_TTc = s.at("grain_TTc");
    double TTc = s.at("TTc");
    double kGrain_scale = std::max(std::min((TTc - grain_TTc)/16, 0.0), 1.0);  // Create a linear scaling coefficient in the interval [0, 1] that delays grain growth until grain_TTC + 1, and ramps it up over 16 growing degree days.  This prevents a jump when grains appear.

    double kLeaf = s.at("rate_constant_leaf");
    double kStem = s.at("rate_constant_stem");
    double kRoot = s.at("rate_constant_root");
    double kRhizome = s.at("rate_constant_rhizome");
    double kGrain = s.at("rate_constant_grain") * kGrain_scale;

    double KmLeaf = s.at("KmLeaf");
    double KmStem = s.at("KmStem");
    double KmRoot = s.at("KmRoot");
    double KmRhizome = s.at("KmRhizome");
    double KmGrain = s.at("KmGrain");

    double resistance_leaf_to_stem = s.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = s.at("resistance_stem_to_grain");
    double resistance_stem_to_root = s.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = s.at("resistance_stem_to_rhizome");

    double carbon_input = s.at("canopy_assimilation_rate"); //Pg in paper

    double Leaf = s.at("Leaf");
    double Stem = s.at("Stem");
    double Root = s.at("Root");
    double Grain = s.at("Grain");
    double Rhizome = s.at("Rhizome");

    double beta = Leaf + Grain + Stem + Root + Rhizome;

    double substrate_pool_leaf = s.at("substrate_pool_leaf");
    double substrate_pool_stem = s.at("substrate_pool_stem");
    double substrate_pool_root = s.at("substrate_pool_root");
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome");
    double substrate_pool_grain = s.at("substrate_pool_grain");

    double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0, mass_fraction_grain = 0;
    double transport_leaf_to_stem = 0, transport_stem_to_grain = 0, transport_stem_to_root = 0, transport_stem_to_rhizome = 0;
    double utilization_leaf = 0, utilization_grain = 0, utilization_stem = 0, utilization_root = 0, utilization_rhizome = 0; 
    double d_substrate_leaf = 0, d_substrate_stem = 0, d_substrate_grain = 0, d_substrate_root = 0, d_substrate_rhizome = 0;
    double d_leaf = 0, d_stem = 0, d_grain = 0, d_root = 0, d_rhizome = 0;

    if ((Leaf > 0)) {
        mass_fraction_leaf = substrate_pool_leaf / Leaf;
        utilization_leaf = std::max(mass_fraction_leaf * kLeaf / (KmLeaf + mass_fraction_leaf), 0.0);
    }
    if ((Stem > 0)) {
        mass_fraction_stem = substrate_pool_stem / Stem;
        utilization_stem = std::max(mass_fraction_stem * kStem / (KmStem + mass_fraction_stem), 0.0);
    }
    if ((Root > 0)) {
        mass_fraction_root = substrate_pool_root / Root;
        utilization_root = std::max(mass_fraction_root * kRoot / (KmRoot + mass_fraction_root), 0.0);
        //if ((Root > 2.2) | (Grain > 0)) utilization_root = 0;
    }
    if ((Rhizome > 0)) {
        mass_fraction_rhizome = substrate_pool_rhizome;
        utilization_rhizome = std::max(mass_fraction_rhizome * kRhizome / (KmRhizome + mass_fraction_rhizome), 0.0);
    }
    if ((Grain > 0)) {
        mass_fraction_grain = substrate_pool_grain / Grain;
        utilization_grain = std::max(mass_fraction_grain * kGrain / (KmGrain + mass_fraction_grain), 0.0);
    }

    //if ((Leaf != 0) & (Stem != 0)) transport_leaf_to_stem = std::max(beta * (mass_fraction_leaf - mass_fraction_stem) / resistance_leaf_to_stem, 0.0);
    //if ((Stem != 0) & (Grain != 0)) transport_stem_to_grain = std::max(beta * (mass_fraction_stem - mass_fraction_grain) / resistance_stem_to_grain, 0.0);
    //if ((Stem != 0) & (Root != 0)) transport_stem_to_root = std::max(beta * (mass_fraction_stem - mass_fraction_root) / resistance_stem_to_root, 0.0);
    //if ((Stem != 0) & (Rhizome != 0)) transport_stem_to_rhizome = std::max(beta * (mass_fraction_stem - mass_fraction_rhizome) / resistance_stem_to_rhizome, 0.0);

    if ((Leaf != 0) & (Stem != 0))    transport_leaf_to_stem    = beta * (mass_fraction_leaf - mass_fraction_stem)    / resistance_leaf_to_stem;
    if ((Stem != 0) & (Grain != 0))   transport_stem_to_grain   = beta * (mass_fraction_stem - mass_fraction_grain)   / resistance_stem_to_grain;
    if ((Stem != 0) & (Root != 0))    transport_stem_to_root    = beta * (mass_fraction_stem - mass_fraction_root)    / resistance_stem_to_root;
    if ((Stem != 0) & (Rhizome != 0)) transport_stem_to_rhizome = beta * (mass_fraction_stem - mass_fraction_rhizome) / resistance_stem_to_rhizome;

    if (carbon_input < -substrate_pool_leaf) {  // This is true when respiration uses more carbon than there is in the substrate pool. The carbon must come from somewhere, so even though utilization for growth is thought of as irreversible, remove previously fixed carbon and don't grow or transport carbon.
        transport_leaf_to_stem = 0;
        double respiratory_deficit = substrate_pool_leaf + carbon_input;  // carbon_input is negative in this case, so the deficit is the amount in excess of the amount currently in the substrate pool.
        utilization_leaf = respiratory_deficit;  // Account for the deficit by taking mass from leaves.
    }

    d_substrate_leaf = (carbon_input - transport_leaf_to_stem - utilization_leaf);
    d_substrate_stem = (transport_leaf_to_stem -transport_stem_to_grain - transport_stem_to_root - transport_stem_to_rhizome - utilization_stem);
    d_substrate_grain = (transport_stem_to_grain - utilization_grain);
    d_substrate_root = (transport_stem_to_root - utilization_root);
    d_substrate_rhizome = (transport_stem_to_rhizome - utilization_rhizome);

    d_leaf = utilization_leaf;
    d_stem = utilization_stem;
    d_grain = utilization_grain;
    d_root = utilization_root;
    d_rhizome = utilization_rhizome;

    state_map derivs {
        { "newLeafcol", d_leaf },
        { "Leaf", d_leaf },
        { "substrate_pool_leaf", d_substrate_leaf },

        { "newStemcol", d_stem },
        { "Stem", d_stem },
        { "substrate_pool_stem", d_substrate_stem },

        { "Grain", d_grain },
        { "substrate_pool_grain", d_substrate_grain },

        { "newRootcol", d_root },
        { "Root", d_root },
        { "substrate_pool_root", d_substrate_root },

        { "newRhizomecol", d_rhizome },
        { "Rhizome", d_rhizome },
        { "substrate_pool_rhizome", d_substrate_rhizome },

        { "utilization_leaf", d_leaf },
        { "utilization_stem", d_stem },
        { "utilization_root", d_root },
        { "utilization_rhizome", d_rhizome },
        { "utilization_grain", d_grain },

        { "transport_leaf_to_stem", transport_leaf_to_stem },
        { "transport_stem_to_grain", transport_stem_to_grain },
        { "transport_stem_to_root", transport_stem_to_root },
        { "transport_stem_to_rhizome", transport_stem_to_rhizome },

        { "mass_fraction_leaf", mass_fraction_leaf },
        { "mass_fraction_stem", mass_fraction_stem },
        { "mass_fraction_root", mass_fraction_root },
        { "mass_fraction_rhizome", mass_fraction_rhizome },
        { "mass_fraction_grain", mass_fraction_grain }
    };
    /*
    state_map derivs;
    derivs["newLeafcol"] = derivs["Leaf"] = d_leaf;
    derivs["substrate_pool_leaf"] = d_substrate_leaf;

    derivs["newStemcol"] = derivs["Stem"] = d_stem;
    derivs["substrate_pool_stem"] = d_substrate_stem;

    derivs["Grain"] = d_grain;
    derivs["substrate_pool_grain"] = d_substrate_grain;

    derivs["newRootcol"] = derivs["Root"] = d_root;
    derivs["substrate_pool_root"] = d_substrate_root;

    derivs["newRhizomecol"] = derivs["Rhizome"] = d_rhizome;
    derivs["substrate_pool_rhizome"] = d_substrate_rhizome;

    derivs["utilization_leaf"] = d_leaf;
    derivs["utilization_stem"] = d_stem;
    derivs["utilization_grain"] = d_grain;
    derivs["utilization_root"] = d_root;
    derivs["utilization_rhizome"] = d_rhizome;
    */

    return (derivs);
}


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

            s_copy += sub_derivs;
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

    double start_grain = 0;

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
    if ((Grain <= 0) & (TTc >= grain_TTc)) {
        // Base grain growth on the amount of stem mass. In the derivatives, remove this from soluble carbon in the stem.
        start_grain = substrate_pool_stem * 0.1;
    }

    state_map derivs {
        {"Leaf", -senescence_leaf},
        {"substrate_pool_leaf", senescence_leaf * remobilization_fraction},
        {"LeafLitter", senescence_leaf * (1 - remobilization_fraction)},

        {"Stem", -senescence_stem},
        {"substrate_pool_stem", senescence_stem * remobilization_fraction - start_grain},
        {"StemLitter", senescence_stem * (1 - remobilization_fraction)},

        {"Grain", start_grain},

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

    /*
    state_map derivs;
//    derivs.reserve(17);
    derivs["Leaf"] = -senescence_leaf;
    derivs["substrate_pool_leaf"] = senescence_leaf * remobilization_fraction;
    derivs["LeafLitter"] = senescence_leaf * (1 - remobilization_fraction);

    derivs["Stem"] = -senescence_stem;
    derivs["substrate_pool_stem"] = senescence_stem * remobilization_fraction - start_grain;
    derivs["StemLitter"] = senescence_stem * (1 - remobilization_fraction);

    derivs["Grain"] = start_grain;

    derivs["Root"] = -senescence_root;
    derivs["substrate_pool_root"] = senescence_root * remobilization_fraction;
    derivs["RootLitter"] = senescence_root * (1 - remobilization_fraction);

    derivs["Rhizome"] = -senescence_rhizome;
    derivs["substrate_pool_rhizome"] = senescence_rhizome * remobilization_fraction;
    derivs["RhizomeLitter"] = senescence_rhizome * (1 - remobilization_fraction);

    derivs["senescence_leaf"] = senescence_leaf;
    derivs["senescence_stem"] = senescence_stem;
    derivs["senescence_root"] = senescence_root;
    derivs["senescence_rhizome"] = senescence_rhizome;
    */

    return (derivs);
}
