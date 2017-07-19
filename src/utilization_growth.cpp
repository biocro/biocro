#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"

state_map utilization_growth_module::do_operation(state_vector_map const &s, state_vector_map const &deriv_history, state_map const &p) const
{
// NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
// record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
// Since it's not recorded, that part will never senesce.
// Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
// at the end of the season for all of the tissue to senesce.
// This doesn't seem like a good approach.

    state_map derivs;
    auto t = s.begin()->second.size() - 1;

    double carbon_input = p.at("CanopyA"); //Pg in paper
    if (carbon_input < 0) carbon_input = 0;

    double Leaf = s.at("Leaf")[t];
    double Stem = s.at("Stem")[t];
    double Root = s.at("Root")[t];
    double Grain = s.at("Grain")[t];
    double Rhizome = s.at("Rhizome")[t];

    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");

    double beta = Leaf+Grain+Stem+Root+Rhizome;

    double KmLeaf = p.at("KmLeaf");
    double KmStem = p.at("KmStem");
    double KmRoot = p.at("KmRoot");
    double KmRhizome = p.at("KmRhizome");
    double KmGrain = p.at("KmGrain");

    double resistance_leaf_to_stem = p.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = p.at("resistance_stem_to_grain");
    double resistance_stem_to_root = p.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = p.at("resistance_stem_to_rhizome");

    double substrate_pool_leaf = s.at("substrate_pool_leaf")[t];
    double substrate_pool_grain = s.at("substrate_pool_grain")[t];
    double substrate_pool_stem = s.at("substrate_pool_stem")[t];
    double substrate_pool_root = s.at("substrate_pool_root")[t];
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome")[t];


    double ratio_leaf = 1;
    double ratio_stem = 1;

    // TODO: Change this so that S / T is 0 instead of transport = 0;
    double transport_leaf_to_stem = (Leaf <= 0 || Stem <= 0) ? 0 : beta*(substrate_pool_leaf / Leaf - substrate_pool_stem / Stem)/resistance_leaf_to_stem;
    double transport_stem_to_grain = (Stem <= 0 || Grain <= 0) ? 0 : beta*(substrate_pool_stem / Stem - substrate_pool_grain / Grain)/resistance_stem_to_grain;
    double transport_stem_to_root = (Stem <= 0 || Root <= 0) ? 0 : beta*(substrate_pool_stem / Stem - substrate_pool_root / Root)/resistance_stem_to_root;
    double transport_stem_to_rhizome = (Stem <= 0 || Rhizome <= 0) ? 0 : beta*(substrate_pool_stem / Stem - substrate_pool_rhizome / Rhizome)/resistance_stem_to_rhizome;
    double utilization_leaf = substrate_pool_leaf*kLeaf/(KmLeaf+Leaf);
    double utilization_grain = substrate_pool_grain*kGrain/(KmGrain+Grain);
    double utilization_stem = substrate_pool_stem*kStem/(KmStem+Stem);
    double utilization_root = substrate_pool_root*kRoot/(KmRoot+Root);
    double utilization_rhizome = substrate_pool_rhizome * kRhizome / (KmRhizome + Rhizome);


    if (transport_leaf_to_stem + utilization_leaf > substrate_pool_leaf + carbon_input) {
        ratio_leaf = (substrate_pool_leaf + carbon_input) / (transport_leaf_to_stem + utilization_leaf);
    }
    derivs["newLeafcol"] = derivs["Leaf"] = utilization_leaf * ratio_leaf ;
    derivs["substrate_pool_leaf"] = carbon_input + (-transport_leaf_to_stem - utilization_leaf) * ratio_leaf;






    if (transport_stem_to_grain + transport_stem_to_root + transport_stem_to_rhizome + utilization_stem > substrate_pool_stem + transport_leaf_to_stem) {
        ratio_stem = (substrate_pool_stem + transport_leaf_to_stem) / (transport_stem_to_grain + transport_stem_to_root + transport_stem_to_rhizome + utilization_stem);
    }
    derivs["newStemcol"] = derivs["Stem"] = utilization_stem * ratio_stem;
    derivs["substrate_pool_stem"] = transport_leaf_to_stem + (-transport_stem_to_grain - transport_stem_to_root - transport_stem_to_rhizome - utilization_stem) * ratio_stem;
    derivs["ratio_stem"] = ratio_stem;
    derivs["transport_leaf_to_stem"] = transport_leaf_to_stem;
    derivs["transport_stem_to_grain"] = transport_stem_to_grain;
    derivs["transport_stem_to_root"] = transport_stem_to_root;
    derivs["transport_stem_to_rhizome"] = transport_stem_to_rhizome;
    derivs["utilization_stem"] = utilization_stem;






    if (utilization_grain > substrate_pool_grain + transport_stem_to_grain) {
        utilization_grain = substrate_pool_grain + transport_stem_to_grain;
    }
    derivs["Grain"] = utilization_grain;
    derivs["substrate_pool_grain"] = transport_stem_to_grain - utilization_grain;

    if (utilization_root > substrate_pool_root + transport_stem_to_root) {
        utilization_root = substrate_pool_root + transport_stem_to_root;
    }
    derivs["newRootcol"] = derivs["Root"] = utilization_root;
    derivs["substrate_pool_root"] = transport_stem_to_root - utilization_root;

    if (utilization_rhizome > substrate_pool_rhizome + transport_stem_to_rhizome) {
        utilization_rhizome = substrate_pool_rhizome + transport_stem_to_rhizome;
    }
    derivs["newRhizomecol"] = derivs["Rhizome"] = utilization_rhizome;
    derivs["substrate_pool_rhizome"] = transport_stem_to_rhizome - utilization_rhizome;

    return (derivs);
}


