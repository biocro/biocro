#include "no_leaf_resp_partitioning_growth_module.hpp"
#include "BioCro.h"

state_map no_leaf_resp_partitioning_growth_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    // This is the same as paritioning_growth_module except that leaf respiration is treated differently.
    // CanopyA already includes losses from leaf respiration, so it should only be removed from leaf mass.

    state_map derivs;
    auto t = state_history.begin()->second.size() - 1;
    state_map s = combine_state(at(state_history, t), p);
    //auto &s = state_history;
    //auto &p = parameters;
    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");
    double assimilation = p.at("canopy_assimilation_rate");
    double nonleaf_carbon_flux;

    if (assimilation < 0) {
        nonleaf_carbon_flux = 0;
    } else {
        nonleaf_carbon_flux = assimilation;
    }

    if (kLeaf > 0) {
        if (assimilation < 0) {  // If assimilation is negative then leaves are respiring more than photosynthesizing.
            derivs["newLeafcol"] = assimilation;  // assimilation is negative here, so this removes leaf mass.
        } else {
            derivs["newLeafcol"] = assimilation * kLeaf; // * p.at("LeafWS")[t];
        }
        /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
            Plant. Phys. 46, 233-235. For this the water stress coefficient is different
            for leaf and vmax. */

        derivs["Leaf"] = derivs["newLeafcol"];
    } else {
        derivs["newLeafcol"] = 0;
        derivs["Leaf"] += s.at("Leaf") * kLeaf;
        derivs["Rhizome"] += kRhizome * -derivs.at("Leaf") * 0.9; /* 0.9 is the efficiency of retranslocation */
        derivs["Stem"] += kStem * -derivs.at("Leaf") * 0.9;
        derivs["Root"] += kRoot * -derivs.at("Leaf") * 0.9;
        derivs["Grain"] += kGrain * -derivs.at("Leaf") * 0.9;
    }

    if (kStem >= 0) {
        derivs["newStemcol"] = nonleaf_carbon_flux * kStem;
        derivs["newStemcol"] = resp(derivs["newStemcol"], p.at("mrc1"), p.at("temp"));
        derivs["Stem"] += derivs["newStemcol"];
    } else {
        throw std::range_error("kStem should be positive");
    }

    if (kRoot > 0) {
        derivs["newRootcol"] = nonleaf_carbon_flux * kRoot;
        derivs["newRootcol"] = resp(derivs["newRootcol"], p.at("mrc2"), p.at("temp"));
        derivs["Root"] += derivs["newRootcol"];
    } else {
        derivs["newRootcol"] = 0;
        derivs["Root"] += s.at("Root") * kRoot;
        derivs["Rhizome"] += kRhizome * -derivs.at("Root") * 0.9;
        derivs["Stem"] += kStem * -derivs.at("Root") * 0.9;
        derivs["Leaf"] += kLeaf * -derivs.at("Root") * 0.9;
        derivs["Grain"] += kGrain * -derivs.at("Root") * 0.9;
    }

    if (kRhizome > 0) {
        derivs["newRhizomecol"] = nonleaf_carbon_flux * kRhizome;
        derivs["newRhizomecol"] = resp(derivs["newRhizomecol"], p.at("mrc2"), p.at("temp"));
        derivs["Rhizome"] += derivs["newRhizomecol"];
        ++derivs["rhizome_index"];
    } else {
        derivs["newRhizomecol"] = 0;
        derivs["Rhizome"] += s.at("Rhizome") * kRhizome;
        if ( derivs["Rhizome"] > s.at("Rhizome") ) {  // Check if this would make the rhizome mass negative.
            derivs["Rhizome"] = s.at("Rhizome");
        }

        derivs["Root"] += kRoot * -derivs.at("Rhizome");
        derivs["Stem"] += kStem * -derivs.at("Rhizome");
        derivs["Leaf"] += kLeaf * -derivs.at("Rhizome");
        derivs["Grain"] += kGrain * -derivs.at("Rhizome");
    }

    if ((kGrain >= 1e-10) && (s.at("TTc") >= p.at("tp5"))) {
        derivs["Grain"] += nonleaf_carbon_flux * kGrain;
        /* No respiration for grain at the moment */
        /* No senescence either */
    }
    return derivs;
}
