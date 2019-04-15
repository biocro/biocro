#include "partitioning_growth_module.hpp"

state_map partitioning_growth_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
// NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
// record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
// Since it's not recorded, that part will never senesce.
// Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
// at the end of the season for all of the tissue to senesce.
// This doesn't seem like a good approach.

    state_map derivs;
    auto t = state_history.begin()->second.size() - 1;
    state_map s = combine_state(at(state_history, t), p);

    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");
    double CanopyA = p.at("canopy_assimilation_rate");

    if (kLeaf > 0) {
        derivs["newLeafcol"] = CanopyA * kLeaf * s.at("LeafWS");
        /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
            Plant. Phys. 46, 233-235. For this the water stress coefficient is different
            for leaf and vmax. */

        /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/
        derivs["newLeafcol"] = resp(derivs["newLeafcol"], p.at("mrc1"), p.at("temp"));

        derivs["Leaf"] = derivs["newLeafcol"]; /* It makes sense to use i because when kLeaf
                                                     is negative no new leaf is being accumulated
                                                     and thus would not be subjected to senescence. */
    } else {
        derivs["newLeafcol"] = 0;
        derivs["Leaf"] += s.at("Leaf") * kLeaf;
        derivs["Rhizome"] += kRhizome * -derivs.at("Leaf") * 0.9; /* 0.9 is the efficiency of retranslocation */
        derivs["Stem"] += kStem * -derivs.at("Leaf") * 0.9;
        derivs["Root"] += kRoot * -derivs.at("Leaf") * 0.9;
        derivs["Grain"] += kGrain * -derivs.at("Leaf") * 0.9;
    }

    if (kStem >= 0) {
        derivs["newStemcol"] = CanopyA * kStem;
        derivs["newStemcol"] = resp(derivs["newStemcol"], p.at("mrc1"), p.at("temp"));
        derivs["Stem"] += derivs["newStemcol"];
    } else {
        throw std::range_error("kStem should be positive");
    }

    if (kRoot > 0) {
        derivs["newRootcol"] = CanopyA * kRoot;
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
        derivs["newRhizomecol"] = CanopyA * kRhizome;
        derivs["newRhizomecol"] = resp(derivs["newRhizomecol"], p.at("mrc2"), p.at("temp"));
        derivs["Rhizome"] += derivs["newRhizomecol"];
        /* Here i will not work because the rhizome goes from being a source
           to a sink. I need its own index. Let's call it rhizome's i or ri.*/
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

    if ((kGrain > 0)) {
        double change = CanopyA * kGrain;
        if (change > 0) {
            derivs["Grain"] += change;
        } else {
            derivs["Grain"] += 0;
        }
        /* No respiration for grain at the moment */
        /* No senescence either */
    }
    return derivs;
}
