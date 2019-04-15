#include "thermal_time_and_frost_senescence.h"

state_map thermal_time_and_frost_senescence::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);
    //output_map(s);
    double TTc = s.at("TTc");
    if (TTc >= parameters.at("seneLeaf")) {
        bool A = s.at("lat") >= 0.0;
        bool B = s.at("doy") >= 180.0;

        if ((A && B) || ((!A) && (!B))) {
            double frost_leaf_death_rate = 0;
            double leafdeathrate = s.at("leafdeathrate");
            if (s.at("temp") > parameters.at("Tfrostlow")) {
                frost_leaf_death_rate = 100 * (s.at("temp") - parameters.at("Tfrosthigh")) / (parameters.at("Tfrostlow") - parameters.at("Tfrosthigh"));
                frost_leaf_death_rate = (frost_leaf_death_rate > 100.0) ? 100.0 : frost_leaf_death_rate;
            } else {
                frost_leaf_death_rate = 0.0;
            }
            double current_leaf_death_rate = (leafdeathrate > frost_leaf_death_rate) ? leafdeathrate : frost_leaf_death_rate;
            derivs["leafdeathrate"] = current_leaf_death_rate - leafdeathrate;

            double change = s.at("Leaf") * current_leaf_death_rate * (0.01 / 24);
            double Remob = change * 0.6;
            derivs["LeafLitter"] += (change - Remob); /* Collecting the leaf litter */
            derivs["Rhizome"] += s.at("kRhizome") * Remob;
            derivs["Stem"] += s.at("kStem") * Remob;
            derivs["Root"] += s.at("kRoot") * Remob;
            derivs["Grain"] += s.at("kGrain") * Remob;
            derivs["Leaf"] += -change + s.at("kLeaf") * Remob;
            ++derivs["leaf_senescence_index"];
        }
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
