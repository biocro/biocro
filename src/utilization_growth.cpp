#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"
#include <R.h>

//static int counter = 0;

state_map utilization_growth_module::do_operation(state_vector_map const &s, state_vector_map const &deriv_history, state_map const &p) const
{
// NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
// record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
// Since it's not recorded, that part will never senesce.
// Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
// at the end of the season for all of the tissue to senesce.
// This doesn't seem like a good approach.

    size_t max_loops = 3;
    state_map derivs;
    auto t = s.begin()->second.size() - 1;

    double kLeaf = p.at("rate_constant_leaf");
    double kStem = p.at("rate_constant_stem");
    double kRoot = p.at("rate_constant_root");
    double kRhizome = p.at("rate_constant_rhizome");
    double kGrain = p.at("rate_constant_grain");

    double KmLeaf = p.at("KmLeaf");
    double KmStem = p.at("KmStem");
    double KmRoot = p.at("KmRoot");
    double KmRhizome = p.at("KmRhizome");
    double KmGrain = p.at("KmGrain");

    double resistance_leaf_to_stem = p.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = p.at("resistance_stem_to_grain");
    double resistance_stem_to_root = p.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = p.at("resistance_stem_to_rhizome");

    double total_time = p.at("timestep"); // hours
    size_t sub_time_steps = total_time * 60;  // At the start, integrate over each minute.
    double carbon_input = p.at("CanopyA"); //Pg in paper

    //double TTc = p.at("TTc");

    // BioCro uses a fixed time-step integrator, which works very poorly with this growth model. The while loop here is a crappy integrator that checks whether the values are feasible. If they are not feasible, it breaks the time period into a smaller period and integrates that. It repeats until the integration produces valid results.
    size_t counter = 0;

    while (true) {

        double Leaf = s.at("Leaf")[t];
        double Stem = s.at("Stem")[t];
        double Root = s.at("Root")[t];
        double Grain = s.at("Grain")[t];
        double Rhizome = s.at("Rhizome")[t];

        double beta = Leaf + Grain + Stem + Root + Rhizome;

        double substrate_pool_leaf = s.at("substrate_pool_leaf")[t];
        double substrate_pool_grain = s.at("substrate_pool_grain")[t];
        double substrate_pool_stem = s.at("substrate_pool_stem")[t];
        double substrate_pool_root = s.at("substrate_pool_root")[t];
        double substrate_pool_rhizome = s.at("substrate_pool_rhizome")[t];

        //Rprintf("Loop %d\n", counter++);
        //Rprintf("Before mass fraction\n");

        double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0, mass_fraction_grain = 0;
        double transport_leaf_to_stem = 0, transport_stem_to_grain = 0, transport_stem_to_root = 0, transport_stem_to_rhizome = 0;
        double utilization_leaf = 0, utilization_grain = 0, utilization_stem = 0, utilization_root = 0, utilization_rhizome = 0; 
        double d_substrate_leaf = 0, d_substrate_stem = 0, d_substrate_grain = 0, d_substrate_root = 0, d_substrate_rhizome = 0;
        double d_leaf = 0, d_stem = 0, d_grain = 0, d_root = 0, d_rhizome = 0;

        size_t i;
        bool failed = false;
        for (i = 0; i < sub_time_steps; ++i) {
            double d_time = total_time / sub_time_steps;

            if (Leaf != 0) {
                mass_fraction_leaf = substrate_pool_leaf / Leaf;
                utilization_leaf = mass_fraction_leaf * kLeaf / (KmLeaf + mass_fraction_leaf);
            }
            if (Stem != 0) {
                mass_fraction_stem = substrate_pool_stem / Stem;
                utilization_stem = mass_fraction_stem * kStem / (KmStem + mass_fraction_stem);
            }
            if (Root != 0) {
                mass_fraction_root = substrate_pool_root / Root;
                utilization_root = mass_fraction_root * kRoot / (KmRoot + mass_fraction_root);
                //if ((Root > 2.2) | (Grain > 0)) utilization_root = 0;
            }
            if (Rhizome != 0) {
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

            if ((Leaf != 0) & (Stem != 0)) transport_leaf_to_stem = beta * (mass_fraction_leaf - mass_fraction_stem) / resistance_leaf_to_stem;
            if ((Stem != 0) & (Grain != 0)) transport_stem_to_grain = beta * (mass_fraction_stem - mass_fraction_grain) / resistance_stem_to_grain;
            if ((Stem != 0) & (Root != 0)) transport_stem_to_root = beta * (mass_fraction_stem - mass_fraction_root) / resistance_stem_to_root;
            if ((Stem != 0) & (Rhizome != 0)) transport_stem_to_rhizome = beta * (mass_fraction_stem - mass_fraction_rhizome) / resistance_stem_to_rhizome;

            if (carbon_input < -substrate_pool_leaf) {  // Respiration uses more carbon than there is in the substrate pool. The carbon must come from somewhere, so even though utilization for growth is thought of as irreversible, remove previously fixed carbon and don't grow or transport carbon.
                transport_leaf_to_stem = 0;
                double respiratory_deficit = substrate_pool_leaf + carbon_input;  // carbon_input is negative in this case, so the deficit is the amount in excess of the amount currently in the substrate pool.
                utilization_leaf = respiratory_deficit;  // Account for the deficit by taking mass from leaves.
            }

            double current_d_substrate_leaf = (carbon_input - transport_leaf_to_stem - utilization_leaf) * d_time;
            double current_d_substrate_stem = (transport_leaf_to_stem -transport_stem_to_grain - transport_stem_to_root - transport_stem_to_rhizome - utilization_stem) * d_time;
            double current_d_substrate_grain = (transport_stem_to_grain - utilization_grain) * d_time;
            double current_d_substrate_root = (transport_stem_to_root - utilization_root) * d_time;
            double current_d_substrate_rhizome = (transport_stem_to_rhizome - utilization_rhizome) * d_time;

            double current_d_leaf = utilization_leaf * d_time;
            double current_d_stem = utilization_stem * d_time;
            double current_d_grain = utilization_grain * d_time;
            double current_d_root = utilization_root * d_time;
            double current_d_rhizome = utilization_rhizome * d_time;

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
                (utilization_stem < 0) |
                (utilization_grain < 0) |
                (utilization_stem < 0) |
                (utilization_rhizome < 0))
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
            //if (TTc < 1) Rprintf("For loop %f, %d.\n", TTc, i);
            //if (counter >= max_loops) Rprintf("Broken integrator, counter %zu, sub_time_steps %zu.\n", counter, sub_time_steps);
            //Rprintf("Loops %zu, counter: %zu\n", i, sub_time_steps, counter);
            //if (d_grain < 0) Rprintf("i: %zu, d_grain: %f, Grain: %f, substrate_pool_grain: %f.\n", i, d_grain, s.at("Grain")[t], s.at("substrate_pool_grain")[t]); 

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


