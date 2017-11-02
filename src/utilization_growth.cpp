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
    size_t max_loops = 10;
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);

    double total_time = p.at("timestep"); // hours
    size_t sub_time_steps = total_time * 60;  // At the start, integrate over each minute.

    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t counter = 0;

    while (true) {

        double Leaf = s.at("Leaf");
        double Stem = s.at("Stem");
        double Root = s.at("Root");
        double Grain = s.at("Grain");
        double Rhizome = s.at("Rhizome");

        double substrate_pool_leaf = s.at("substrate_pool_leaf");
        double substrate_pool_grain = s.at("substrate_pool_grain");
        double substrate_pool_stem = s.at("substrate_pool_stem");
        double substrate_pool_root = s.at("substrate_pool_root");
        double substrate_pool_rhizome = s.at("substrate_pool_rhizome");

        //Rprintf("Loop %d\n", counter++);
        //Rprintf("Before mass fraction\n");

        double d_substrate_leaf = 0, d_substrate_stem = 0, d_substrate_grain = 0, d_substrate_root = 0, d_substrate_rhizome = 0;
        double d_leaf = 0, d_stem = 0, d_grain = 0, d_root = 0, d_rhizome = 0;

        size_t i;
        bool failed = false;
        for (i = 0; i < sub_time_steps; ++i) {
            state_map sub_derivs;
            double d_time = total_time / sub_time_steps;

            sub_derivs = this->run(s);

            double current_d_substrate_leaf = sub_derivs["substrate_pool_leaf"] * d_time;
            double current_d_substrate_stem = sub_derivs["substate_pool_stem"] * d_time;
            double current_d_substrate_grain = sub_derivs["substate_pool_grain"] * d_time;
            double current_d_substrate_root = sub_derivs["substrate_pool_root"] * d_time;
            double current_d_substrate_rhizome = sub_derivs["substrate_pool_rhizome"] * d_time;

            double current_d_leaf = sub_derivs["utilization_leaf"] * d_time;
            double current_d_stem = sub_derivs["utilization_stem"] * d_time;
            double current_d_grain = sub_derivs["utilization_grain"] * d_time;
            double current_d_root = sub_derivs["utilization_root"] * d_time;
            double current_d_rhizome = sub_derivs["utilization_rhizome"] * d_time;

            d_substrate_leaf += current_d_substrate_leaf;
            d_substrate_stem += current_d_substrate_stem;
            d_substrate_grain += current_d_substrate_grain;
            d_substrate_root += current_d_substrate_root;
            d_substrate_rhizome += current_d_substrate_rhizome;

            d_leaf += current_d_leaf;
            d_stem += current_d_stem;
            d_grain += current_d_grain;
            d_root += current_d_root;
            d_rhizome += current_d_rhizome;

            substrate_pool_leaf +=  current_d_substrate_leaf;
            substrate_pool_stem +=  current_d_substrate_stem;
            substrate_pool_grain +=  current_d_substrate_grain;
            substrate_pool_root +=  current_d_substrate_root;
            substrate_pool_rhizome +=  current_d_substrate_rhizome;

            Leaf += current_d_leaf;
            Stem += current_d_stem;
            Grain += current_d_grain;
            Root += current_d_root;
            Rhizome += current_d_rhizome;

            // The following conditions are not possible and will not be corrected with futher iteration.
            if ((substrate_pool_leaf < 0) |
                (substrate_pool_stem < 0) |
                (substrate_pool_grain < 0) |
                (substrate_pool_root < 0) |
                (substrate_pool_rhizome < 0) |
                (current_d_stem < 0) |
                (current_d_grain < 0) |
                (current_d_stem < 0) |
                (current_d_rhizome < 0) |
                (current_d_root < 0))
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
            break;
        }
    }

    return (derivs);
}

state_map utilization_growth_module::do_operation(state_map const &s) const
{
    double kLeaf = s.at("rate_constant_leaf");
    double kStem = s.at("rate_constant_stem");
    double kRoot = s.at("rate_constant_root") * s.at("rate_constant_root_scale");
    double kRhizome = s.at("rate_constant_rhizome");
    double kGrain = s.at("rate_constant_grain");

    double KmLeaf = s.at("KmLeaf");
    double KmStem = s.at("KmStem");
    double KmRoot = s.at("KmRoot");
    double KmRhizome = s.at("KmRhizome");
    double KmGrain = s.at("KmGrain");

    double resistance_leaf_to_stem = s.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = s.at("resistance_stem_to_grain");
    double resistance_stem_to_root = s.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = s.at("resistance_stem_to_rhizome");

    double carbon_input = s.at("CanopyA"); //Pg in paper

    double Leaf = s.at("Leaf");
    double Stem = s.at("Stem");
    double Root = s.at("Root");
    double Grain = s.at("Grain");
    double Rhizome = s.at("Rhizome");

    double beta = Leaf + Grain + Stem + Root + Rhizome;

    double substrate_pool_leaf = s.at("substrate_pool_leaf");
    double substrate_pool_grain = s.at("substrate_pool_grain");
    double substrate_pool_stem = s.at("substrate_pool_stem");
    double substrate_pool_root = s.at("substrate_pool_root");
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome");

    double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0, mass_fraction_grain = 0;
    double transport_leaf_to_stem = 0, transport_stem_to_grain = 0, transport_stem_to_root = 0, transport_stem_to_rhizome = 0;
    double utilization_leaf = 0, utilization_grain = 0, utilization_stem = 0, utilization_root = 0, utilization_rhizome = 0; 
    double d_substrate_leaf = 0, d_substrate_stem = 0, d_substrate_grain = 0, d_substrate_root = 0, d_substrate_rhizome = 0;
    double d_leaf = 0, d_stem = 0, d_grain = 0, d_root = 0, d_rhizome = 0;


    if ((Leaf != 0)) {
        mass_fraction_leaf = substrate_pool_leaf / Leaf;
        utilization_leaf = mass_fraction_leaf * kLeaf / (KmLeaf + mass_fraction_leaf);
    }
    if ((Stem != 0)) {
        mass_fraction_stem = substrate_pool_stem / Stem;
        utilization_stem = mass_fraction_stem * kStem / (KmStem + mass_fraction_stem);
    }
    if ((Root != 0)) {
        mass_fraction_root = substrate_pool_root / Root;
        utilization_root = mass_fraction_root * kRoot / (KmRoot + mass_fraction_root);
        //if ((Root > 2.2) | (Grain > 0)) utilization_root = 0;
    }
    if ((Rhizome != 0)) {
        mass_fraction_rhizome = substrate_pool_rhizome;
        utilization_rhizome = mass_fraction_rhizome * kRhizome / (KmRhizome + mass_fraction_rhizome);
    }
    if (Grain != 0) {
        mass_fraction_grain = substrate_pool_grain / Grain;
        utilization_grain = mass_fraction_grain * kGrain / (KmGrain + mass_fraction_grain);
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

    return (derivs);
}


state_map utilization_senescence::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);
    double TTc = s.at("TTc");

    size_t max_loops = 3;

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

    double total_time = s.at("timestep"); // hours
    size_t sub_time_steps = total_time * 60;  // At the start, integrate over each minute.

    //double TTc = s.at("TTc");

    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t counter = 0;

    while (true) {

        double Leaf = s.at("Leaf");
        double Stem = s.at("Stem");
        double Grain = s.at("Grain");
        double Root = s.at("Root");
        double Rhizome = s.at("Rhizome");

        double substrate_pool_leaf = s.at("substrate_pool_leaf");
        double substrate_pool_stem = s.at("substrate_pool_stem");
        double substrate_pool_root = s.at("substrate_pool_root");
        double substrate_pool_rhizome = s.at("substrate_pool_rhizome");

        //Rprintf("Loop %d\n", counter++);
        //Rprintf("Before mass fraction\n");

        double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0;
        double senescence_leaf = 0, senescence_stem = 0, senescence_root = 0, senescence_rhizome = 0; 
        double d_substrate_leaf = 0, d_substrate_stem = 0, d_substrate_root = 0, d_substrate_rhizome = 0;
        double d_leaf = 0, d_stem = 0, d_grain = 0, d_root = 0, d_rhizome = 0;

        size_t i;
        bool failed = false;
        for (i = 0; i < sub_time_steps; ++i) {
            double d_time = total_time / sub_time_steps;
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
                start_grain = 0.01;
            }

            double current_d_substrate_leaf = senescence_leaf * remobilization_fraction * d_time;
            double current_d_substrate_stem = (senescence_stem * remobilization_fraction - start_grain) * d_time;
            double current_d_substrate_root = senescence_root * remobilization_fraction * d_time;
            double current_d_substrate_rhizome = senescence_rhizome * remobilization_fraction * d_time;

            double current_d_leaf = -senescence_leaf * d_time;
            double current_d_stem = -senescence_stem * d_time;
            double current_d_grain = start_grain;
            double current_d_root = -senescence_root * d_time;
            double current_d_rhizome = -senescence_rhizome * d_time;


            d_substrate_leaf += current_d_substrate_leaf;
            d_substrate_stem += current_d_substrate_stem;
            d_substrate_root += current_d_substrate_root;
            d_substrate_rhizome += current_d_substrate_rhizome;

            d_leaf += current_d_leaf;
            d_stem += current_d_stem;
            d_grain += current_d_grain;
            d_root += current_d_root;
            d_rhizome += current_d_rhizome;

            substrate_pool_leaf +=  current_d_substrate_leaf;
            substrate_pool_stem +=  current_d_substrate_stem;
            substrate_pool_root +=  current_d_substrate_root;
            substrate_pool_rhizome +=  current_d_substrate_rhizome;

            Leaf += current_d_leaf;
            Stem += current_d_stem;
            Grain += current_d_grain;
            Root += current_d_root;
            Rhizome += current_d_rhizome;

            // The following conditions are not possible and will not be corrected with futher iteration.
            if ((substrate_pool_leaf < 0) |
                (substrate_pool_stem < 0) |
                (substrate_pool_root < 0) |
                (substrate_pool_rhizome < 0) |
                (senescence_stem < 0) |
                (senescence_stem < 0) |
                (senescence_rhizome < 0))
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
            //if (TTc < 1) Rprintf("For loop %f, %d.\n", TTc, i);
            //if (counter >= max_loops) Rprintf("Broken integrator, counter %zu, sub_time_steps %zu.\n", counter, sub_time_steps);
            //Rprintf("Loops %zu, counter: %zu\n", i, sub_time_steps, counter);

            derivs["Leaf"] = d_leaf;
            derivs["substrate_pool_leaf"] = -d_leaf * remobilization_fraction;
            derivs["LeafLitter"] = -d_leaf * (1 - remobilization_fraction);

            derivs["Stem"] = d_stem;
            derivs["substrate_pool_stem"] = -d_stem * remobilization_fraction;
            derivs["StemLitter"] = -d_stem * (1 - remobilization_fraction);

            derivs["Grain"] = d_grain;

            derivs["Root"] = d_root;
            derivs["substrate_pool_root"] = -d_root * remobilization_fraction;
            derivs["RootLitter"] = -d_root * (1 - remobilization_fraction);

            derivs["Rhizome"] = d_rhizome;
            derivs["substrate_pool_rhizome"] = -d_rhizome * remobilization_fraction;
            derivs["RhizomeLitter"] = -d_rhizome * (1 - remobilization_fraction);

            derivs["senescence_leaf"] = d_leaf;
            derivs["senescence_stem"] = d_stem;
            derivs["senescence_root"] = d_root;
            derivs["senescence_rhizome"] = d_rhizome;
            break;
        }
    }

    return (derivs);
}

