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
    //state_map s = combine_state(at(state_history, t), parameters);
    //auto &s = state_history;
    //auto &p = parameters;

    double CanopyA = p.at("CanopyA"); //Pg in paper

    double Leaf = s.at("Leaf");
    double Stem = s.at("Stem");
    double Root = s.at("Root");
    double Grain = s.at("Grain");
    double Rhizome = s.at("Rhizome");

    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");

    double beta = Leaf+Grain+Stem+Root+Rhizome;
    double conversion_efficiency = p.at("conversion_efficiency");

    double KmLeaf = p.at("KLeaf");
    double KmStem = p.at("KStem");
    double KmRoot = p.at("KRoot");
    double KmRhizome = p.at("KRhizome");
    double KmGrain = p.at("KGrain");

    double resistance_leaf_to_stem = p.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = p.at("resistance_stem_to_grain");
    double resistance_stem_to_root = p.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = p.at("resistance_stem_to_rhizome");

    double substrate_pool_leaf = s.at("substrate_pool_leaf");
    double substrate_pool_grain = s.at("substrate_pool_grain");
    double substrate_pool_stem = s.at("substrate_pool_stem");
    double substrate_pool_root = s.at("substrate_pool_root");
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome");


    derivs["newLeafcol"] = derivs["Leaf"] = conversion_efficiency*substrate_pool_leaf*kLeaf/(KmLeaf+Leaf);
    derivs["substrate_pool_leaf"] = CanopyA - beta*(Leaf-Stem)/resistance_leaf_to_stem - substrate_pool_leaf*kLeaf/(KmLeaf+Leaf);

    derivs["newStemcol"] = derivs["Stem"] = conversion_efficiency*substrate_pool_stem*kStem/(KmStem+Stem);
    derivs[substrate_pool_stem] = beta*(Leaf-Stem)/resistance_leaf_to_stem - beta*(Stem-Grain)/resistance_stem_to_grain - beta*(Stem-Root)/resistance_stem_to_root - beta*(Stem-Rhizome)/resistance_stem_to_rhizome - substrate_pool_stem*kStem/(KmStem+Stem);

    derivs["Grain"] = conversion_efficiency*substrate_pool_grain*kGrain/(KmGrain+Grain);
    derivs[substrate_pool_grain] = beta*(Stem-Grain)/resistance_stem_to_grain - substrate_pool_grain*kGrain/(KmGrain+Grain);

    derivs["newRootcol"] = derivs["Root"] = conversion_efficiency*substrate_pool_root*kRoot/(KmRoot+Root);
    derivs[substrate_pool_root] = beta*(Stem-Root)/resistance_stem_to_root - substrate_pool_root*kRoot/(KmRoot+Root);

    derivs["newRhizomecol"] = derivs["Rhizome"] = conversion_efficiency*substrate_pool_rhizome*kRoot/(KmRoot+Rhizome);
    derivs[substrate_pool_rhizome] = beta*(Stem-Rhizome)/resistance_stem_to_rhizome - substrate_pool_rhizome*kRhizome/(KmRhizome+Rhizome);

    return (derivs);
}
