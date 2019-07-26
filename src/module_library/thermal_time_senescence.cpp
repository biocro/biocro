#include "thermal_time_senescence.hpp"

state_map thermal_time_senescence::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);
    //output_map(s);
    double TTc = s.at("TTc");
    if (TTc >= parameters.at("seneLeaf")) {
        double change = deriv_history.at("newLeafcol")[s["leaf_senescence_index"]];
        derivs["Leaf"] -= change; /* This means that the new value of leaf is
                                               the previous value plus the newLeaf
                                               (Senescence might start when there is
                                               still leaf being produced) minus the leaf
                                               produced at the corresponding k. */
        double Remob = change * 0.6;
        derivs["LeafLitter"] += change * 0.4; /* Collecting the leaf litter */
        derivs["Rhizome"] += s.at("kRhizome") * Remob;
        derivs["Stem"] += s.at("kStem") * Remob;
        derivs["Root"] += s.at("kRoot") * Remob;
        derivs["Grain"] += s.at("kGrain") * Remob;
        ++derivs["leaf_senescence_index"];
    }

    if (TTc >= parameters.at("seneStem")) {
        double change = deriv_history.at("newStemcol")[s["stem_senescence_index"]];
        derivs["Stem"] -= change;
        derivs["StemLitter"] += change;
        ++derivs["stem_senescence_index"];
    }

    if (TTc >= parameters.at("seneRoot")) {
        double change = deriv_history.at("newRootcol")[s["root_senescence_index"]];
        derivs["Root"] -= change;
        derivs["RootLitter"] += change;
        ++derivs["root_senescence_index"];
    }

    if (TTc >= parameters.at("seneRhizome")) {
        double change = deriv_history.at("newRhizomecol")[s["rhizome_senescence_index"]];
        derivs["Rhizome"] -= change;
        derivs["RhizomeLitter"] += change;
        ++derivs["rhizome_senescence_index"];
    }

    return derivs;
}
