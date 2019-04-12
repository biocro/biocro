#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"
#include "state_map.h"
#include "ball_berry.h"
#include "AuxBioCro.h"

using std::string;
using std::vector;

string join_string_vector(vector<string> const &state_keys) {
    std::ostringstream message;
    for(auto it = state_keys.begin(); it != state_keys.end() - 1; ++it) {
        message << *it << ", ";
    }
    message << state_keys.back();
    return message.str();
}

vector<string> IModule::list_required_state() const
{
    return this->_required_state;
}

vector<string> IModule::list_modified_state() const
{
    return this->_modified_state;
}

string IModule::list_module_name() const
{
    return this->_module_name;
}

state_map IModule::run(state_map const &state) const
{
    try {
        state_map derivs = this->do_operation(state);
        return derivs;
    }
    catch (std::out_of_range const &oor) {
        vector<string> missing_state = this->state_requirements_are_met(state);
        if (missing_state.size() > 0) {
            std::ostringstream message;
            message << "The following state variables, required by " << _module_name << ", are missing: " << join_string_vector(missing_state);
            throw std::out_of_range(message.str());
        } else {
            throw std::out_of_range(std::string("Out of range exception while running module \"") + _module_name + "\": " + oor.what());
        }
    }
}

state_map IModule::run(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    try {
        state_map derivs = this->do_operation(state_history, deriv_history, parameters);
        return derivs;
    }
    catch (std::out_of_range const &oor) {
        state_map state = combine_state(at(state_history, 0), parameters);
        vector<string> missing_state = this->state_requirements_are_met(state);
        if (missing_state.size() > 0) {
            std::ostringstream message;
            message << "The following state variables, required by " << _module_name << ", are missing: " << join_string_vector(missing_state);
            throw std::out_of_range(message.str());
        } else {
            throw std::out_of_range(std::string("Out of range exception while running module \"") + _module_name + "\": " + oor.what());
        }
    }
}

state_map IModule::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    state_map current_state = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);
    return this->do_operation(current_state);
}

vector<string> IModule::state_requirements_are_met(state_map const &s) const
{
    vector<string> missing_state;
    for (auto it = _required_state.begin(); it != _required_state.end(); ++it) {
        if (s.find(*it) == s.end()) {
            missing_state.push_back(*it);
        }
    }
    return missing_state;
}

/*
state_map leaf_boundary_layer_conductance_nikolov::do_operation(state_map const &s) const
{
    return { {"leaf_boundary_layer_conductance", leaf_boundary_layer_conductance } };
}
*/

state_map penman_monteith_leaf_temperature::do_operation(state_map const &s) const
{
    //check_state s(ss);
    //output_map(s);
    // From Thornley and Johnson 1990. pg 418. equation 14.11e.
    double const slope_water_vapor = s.at("slope_water_vapor");
    double const psychr_parameter = s.at("psychrometric_parameter");
    double const LHV = s.at("latent_heat_vaporization_of_water");
    double const ga = s.at("leaf_boundary_layer_conductance");  // m / s

    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double const gc = s.at("leaf_stomatal_conductance") * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double const PhiN = s.at("leaf_net_irradiance");  // W / m^2. Leaf area basis.

    double const delta_t = (PhiN * (1 / ga + 1 / gc) - LHV * s.at("vapor_density_deficit"))
        /
        (LHV * (slope_water_vapor + psychr_parameter * (1 + ga / gc)));  // K. It is also degrees C because it's a change in temperature.

    state_map new_state { { "leaf_temperature", s.at("temp") + delta_t } };
    return new_state;
}

state_map penman_monteith_transpiration::do_operation(state_map const &s) const
{
    //check_state s(ss);
    // From Thornley and Johnson 1990. pg 408. equation 14.4k.
    double const slope_water_vapor = s.at("slope_water_vapor");  // kg / m^3 / K
    double const psychr_parameter = s.at("psychrometric_parameter");  // kg / m^3 / K
    double const LHV = s.at("latent_heat_vaporization_of_water");  // J / kg
    double const ga = s.at("leaf_boundary_layer_conductance");  // m / s

    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double const gc = s.at("leaf_stomatal_conductance") * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double const PhiN = s.at("leaf_net_irradiance");  // W / m^2. Leaf area basis.

    //Rprintf("SlopeFS %f, PhiN %f, LHV %f, PsycParam %f, ga %f, vapor_density_deficit %f, conductance_in... %f\n", slope_water_vapor, PhiN, LHV, psychr_parameter, ga, s.at("vapor_density_deficit"), gc);
    double const evapotranspiration = (slope_water_vapor * PhiN + LHV * psychr_parameter * ga * s.at("vapor_density_deficit"))
        /
        (LHV * (slope_water_vapor + psychr_parameter * (1 + ga / gc)));

    //Rprintf("trans rate %f\n", evapotranspiration * 1e3 * 1e3 / 18);
    state_map new_state { { "leaf_transpiration_rate", evapotranspiration } };  // kg / m^2 / s. Leaf area basis.
    return new_state;
}

state_map priestley_transpiration::do_operation(state_map const &s) const
{
    double const slope_water_vapor = s.at("slope_water_vapor");
    double const psychr_parameter = s.at("psychrometric_parameter");
    double const LHV = s.at("latent_heat_vaporization_of_water");
    double const evapotranspiration = 1.26 * slope_water_vapor * s.at("PhiN") / (LHV * (slope_water_vapor + psychr_parameter));

    state_map new_state { { "transpiration_rate", evapotranspiration } };  // kg / m^2 / s. Leaf area basis.
    return new_state;
}

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

double biomass_leaf_nitrogen_limitation(state_map const &s)
{
    double leaf_n = s.at("LeafN_0") * pow(s.at("Leaf") + s.at("Stem"), -s.at("kln"));
    return (leaf_n > s.at("LeafN_0") ? s.at("LeafN_0") : leaf_n);
}

double thermal_leaf_nitrogen_limitation(state_map const &s)
{
    return (s.at("LeafN_0") * exp(-s.at("kln") * s.at("TTc")));
}

bool any_key_is_duplicated(vector<vector<string>> const &keys) {
    vector<string> all_keys;
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
            all_keys.push_back(*it2);
        }
    }

    auto last = all_keys.end();
    for (auto it = all_keys.begin(); it != last; ++it) {
        if (std::find(it + 1, last, *it) != last) {
            return true;
        }
    }
    return false;
}


