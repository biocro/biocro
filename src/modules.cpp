
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"

string join_string_vector(vector<string> const &state_keys) {
    std::ostringstream message;
    for(auto it = state_keys.begin(); it != state_keys.end() - 1; ++it) {
        message << *it << ", ";
    }
    message << state_keys.back();
    return (message.str());
}

vector<string> IModule::list_required_state() const
{
    return (this->_required_state);
}

vector<string> IModule::list_modified_state() const
{
    return (this->_modified_state);
}

state_map IModule::run(state_map const &state) const
{
    state_map result;
    try {
        result = this->do_operation(state);
    }
    catch (std::out_of_range const &oor) {
        vector<string> missing_state = this->state_requirements_are_met(state);
        if (missing_state.size() > 0) {
            std::ostringstream message;
            message << "The following required state variables are missing: " << join_string_vector(missing_state);
            throw std::out_of_range(message.str());
        } else {
            throw std::out_of_range(oor.what());
        }
    }
    return (result);
}

state_map IModule::run(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    state_map result;
    try {
        result = this->do_operation(state_history, deriv_history, parameters);
    }
    catch (std::out_of_range const &oor) {
        state_map state = combine_state(at(state_history, 0), parameters);
        vector<string> missing_state = this->state_requirements_are_met(state);
        if (missing_state.size() > 0) {
            std::ostringstream message;
            message << "The following required state variables are missing: " << join_string_vector(missing_state);
            throw std::out_of_range(message.str());
        } else {
            throw std::out_of_range(oor.what());
        }
    }
    return (result);
}

state_map IModule::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    state_map current_state = combine_state(at(state_history, state_history.begin()->second.size() - 1), p);
    return (this->do_operation(current_state));
}

vector<string> IModule::state_requirements_are_met(state_map const &s) const
{
    vector<string> missing_state;
    for (auto it = _required_state.begin(); it != _required_state.end(); ++it) {
        if (s.find(*it) == s.end()) {
            missing_state.push_back(*it);
        }
    }
    return (missing_state);
}

state_map c4_canopy::do_operation(state_map const &s) const
{
    struct nitroParms nitroP; 
    nitroP.ileafN = s.at("nileafn");
    nitroP.kln = s.at("nkln");
    nitroP.Vmaxb1 = s.at("nvmaxb1");
    nitroP.Vmaxb0 = s.at("nvmaxb0");
    nitroP.alphab1 = s.at("nalphab1");
    nitroP.alphab0 = s.at("nalphab0");
    nitroP.Rdb1 = s.at("nRdb1");
    nitroP.Rdb0 = s.at("nRdb0");
    nitroP.kpLN = s.at("nkpLN");
    nitroP.lnb0 = s.at("nlnb0");
    nitroP.lnb1 = s.at("nlnb1");

    struct Can_Str result = CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax1"),
            s.at("alpha1"), s.at("kparm"), s.at("beta"), s.at("Rd"), s.at("Catm"),
            s.at("b0"), s.at("b1"), s.at("theta"), s.at("kd"), s.at("chil"),
            s.at("heightf"), s.at("LeafN"), s.at("kpLN"), s.at("lnb0"), s.at("lnb1"),
            (int)s.at("lnfun"), s.at("upperT"), s.at("lowerT"), nitroP, s.at("leafwidth"),
            (int)s.at("et_equation"), s.at("StomataWS"), (int)s.at("water_stress_approach"));

    state_map derivs;
    derivs["Assim"] = result.Assim;
    derivs["Trans"] = result.Trans;
    derivs["GrossAssim"] = result.GrossAssim;

    return (derivs);
}

state_map c3_canopy::do_operation(state_map const &s) const
{

    struct Can_Str result = c3CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax"),
            s.at("jmax"), s.at("Rd"), s.at("Catm"), s.at("O2"), s.at("b0"),
            s.at("b1"), s.at("theta"), s.at("kd"), s.at("heightf"), s.at("LeafN"),
            s.at("kpLN"), s.at("lnb0"), s.at("lnb1"), (int)s.at("lnfun"), s.at("chil"), s.at("StomataWS"),
            (int)s.at("water_stress_approach"), s.at("electrons_per_carboxylation"), s.at("electrons_per_oxygenation"));

    state_map derivs;
    derivs["Assim"] = result.Assim;
    derivs["Trans"] = result.Trans;
    derivs["GrossAssim"] = result.GrossAssim;

    return (derivs);
}

state_map one_layer_soil_profile::do_operation(state_map const &s) const
{
    double soilEvap = SoilEvapo(s.at("lai"), 0.68, s.at("temp"), s.at("solar"), s.at("waterCont"),
                s.at("FieldC"), s.at("WiltP"), s.at("windspeed"), s.at("rh"), s.at("rsec"));
    double TotEvap = soilEvap + s.at("CanopyT");

    struct ws_str WaterS = watstr(s.at("precip"), TotEvap, s.at("waterCont"), s.at("soilDepth"), s.at("FieldC"),
            s.at("WiltP"), s.at("phi1"), s.at("phi2"), s.at("soilType"), s.at("wsFun"));

    state_map derivs;
    derivs["soilEvap"] = soilEvap;
    derivs["waterCont"] = WaterS.awc - s.at("waterCont");
    derivs["StomataWS"] = WaterS.rcoefPhoto - s.at("StomataWS");
    derivs["LeafWS"] =  WaterS.rcoefSpleaf - s.at("LeafWS");
    return (derivs);
}

state_map two_layer_soil_profile::do_operation(state_map const &s) const
{
    double cws[] = {s.at("cws1"), s.at("cws2")};
    double soilDepths[] = {s.at("soilDepth1"), s.at("soilDepth2"), s.at("soilDepth3")};
    struct soilText_str soTexS = soilTchoose(s.at("soilType"));

    struct soilML_str soilMLS = soilML(s.at("precip"), s.at("CanopyT"), cws, s.at("soilDepth3"), soilDepths,
            s.at("FieldC"), s.at("WiltP"), s.at("phi1"), s.at("phi2"), soTexS, s.at("wsFun"),
            2 /* Always uses 2 layers. */, s.at("Root"), s.at("lai"), 0.68, s.at("temp"),
           s.at("solar"), s.at("windspeed"), s.at("rh"), s.at("hydrDist"), s.at("rfl"),
           s.at("rsec"), s.at("rsdf"));

    state_map derivs;
    derivs["StomataWS"] = soilMLS.rcoefPhoto - s.at("StomataWS");
    derivs["LeafWS"] =  soilMLS.rcoefSpleaf - s.at("LeafWS");
    derivs["soilEvap"] = soilMLS.SoilEvapo;

    derivs["cws1"] = soilMLS.cws[0] - s.at("cws1");
    derivs["cws2"] = soilMLS.cws[1] - s.at("cws2");
    double cws_sum = soilMLS.cws[0] + soilMLS.cws[1];
    derivs["waterCont"] =  cws_sum / 2 - s.at("waterCont");  // Divide by 2, the number of layers.
    
    return (derivs);
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

    return (derivs);
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

    return (derivs);
}

state_map partitioning_growth_module::do_operation(state_vector_map const &s, state_vector_map const &deriv_history, state_map const &p) const
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
    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");
    double CanopyA = p.at("CanopyA");

    if (kLeaf > 0) {
        derivs["newLeafcol"] = CanopyA * kLeaf * s.at("LeafWS")[t];
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
        derivs["Leaf"] += s.at("Leaf")[t] * kLeaf;
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
        derivs["Root"] += s.at("Root")[t] * kRoot;
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
        derivs["Rhizome"] += s.at("Rhizome")[t] * kRhizome;
        if ( derivs["Rhizome"] > s.at("Rhizome")[t] ) {  // Check if this would make the rhizome mass negative.
            derivs["Rhizome"] = s.at("Rhizome")[t];
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
    return (derivs);
}

state_map no_leaf_resp_partitioning_growth_module::do_operation(state_vector_map const &s, state_vector_map const &deriv_history, state_map const &p) const
{
    // This is the same as paritioning_growth_module except that leaf respiration is treated differently.
    // CanopyA already includes losses from leaf respiration, so it should only be removed from leaf mass.

    state_map derivs;
    auto t = s.begin()->second.size() - 1;
    //state_map s = combine_state(at(state_history, t), parameters);
    //auto &s = state_history;
    //auto &p = parameters;
    double kLeaf = p.at("kLeaf");
    double kStem = p.at("kStem");
    double kRoot = p.at("kRoot");
    double kRhizome = p.at("kRhizome");
    double kGrain = p.at("kGrain");
    double CanopyA = p.at("CanopyA");
    double nonleaf_carbon_flux;

    if (CanopyA < 0) {
        nonleaf_carbon_flux = 0;
    } else {
        nonleaf_carbon_flux = CanopyA;
    }

    if (kLeaf > 0) {
        if (CanopyA < 0) {  // If CanopyA is negative then leaves are respiring more than photosynthesizing.
            derivs["newLeafcol"] = CanopyA;  // CanopyA is negative here, so this removes leaf mass.
        } else {
            derivs["newLeafcol"] = CanopyA * kLeaf; // * s.at("LeafWS")[t];
        }
        /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
            Plant. Phys. 46, 233-235. For this the water stress coefficient is different
            for leaf and vmax. */

        derivs["Leaf"] = derivs["newLeafcol"]; 
    } else {
        derivs["newLeafcol"] = 0;
        derivs["Leaf"] += s.at("Leaf")[t] * kLeaf;
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
        derivs["Root"] += s.at("Root")[t] * kRoot;
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
        derivs["Rhizome"] += s.at("Rhizome")[t] * kRhizome;
        if ( derivs["Rhizome"] > s.at("Rhizome")[t] ) {  // Check if this would make the rhizome mass negative.
            derivs["Rhizome"] = s.at("Rhizome")[t];
        }

        derivs["Root"] += kRoot * -derivs.at("Rhizome");
        derivs["Stem"] += kStem * -derivs.at("Rhizome");
        derivs["Leaf"] += kLeaf * -derivs.at("Rhizome");
        derivs["Grain"] += kGrain * -derivs.at("Rhizome");
    }

    if ((kGrain >= 1e-10) && (s.at("TTc")[t] >= p.at("tp5"))) {
        derivs["Grain"] += nonleaf_carbon_flux * kGrain;
        /* No respiration for grain at the moment */
        /* No senescence either */
    }
    return (derivs);
}

std::unique_ptr<IModule> make_module(string const &module_name)
{
    if (module_name.compare("c4_canopy") == 0) {
        return std::unique_ptr<IModule>(new c4_canopy);
    }
    else if (module_name.compare("c3_canopy") == 0) {
        return std::unique_ptr<IModule>(new c3_canopy);
    }
    else if (module_name.compare("one_layer_soil_profile") == 0) {
        return std::unique_ptr<IModule>(new one_layer_soil_profile);
    }
    else if (module_name.compare("two_layer_soil_profile") == 0) {
        return std::unique_ptr<IModule>(new two_layer_soil_profile);
    }
    else if (module_name.compare("partitioning_growth") == 0) {
        return std::unique_ptr<IModule>(new partitioning_growth_module);
    }
    else if (module_name.compare("utilization_growth") == 0) {
        return std::unique_ptr<IModule>(new utilization_growth_module);
    }
    else if (module_name.compare("no_leaf_resp_partitioning_growth") == 0) {
        return std::unique_ptr<IModule>(new no_leaf_resp_partitioning_growth_module);
    }
    else if (module_name.compare("thermal_time_and_frost_senescence") == 0) {
        return std::unique_ptr<IModule>(new thermal_time_and_frost_senescence);
    }
    else if (module_name.compare("thermal_time_senescence") == 0) {
        return std::unique_ptr<IModule>(new thermal_time_senescence);
    }
    else if (module_name.compare("utilization_senescence") == 0) {
        return std::unique_ptr<IModule>(new utilization_senescence);
    }
    else if (module_name.compare("utilization_growth_and_senescence") == 0) {
        return std::unique_ptr<IModule>(new utilization_growth_and_senescence_module);
    }
    else if (module_name.compare("utilization_growth_and_flowering") == 0) {
        return std::unique_ptr<IModule>(new utilization_growth_flowering_module);
    }
    else if (module_name.compare("empty_senescence") == 0) {
        return std::unique_ptr<IModule>(new empty_senescence);
    }
    else {
        throw std::out_of_range(module_name);
    }
}


state_vector_map allocate_state(state_map const &m, size_t n)
{
    state_vector_map result;
    for (auto it = m.begin(); it != m.end(); ++it) {
        vector<double> temp;
        temp.reserve(n);
        result.insert(std::pair<string, vector<double>>(it->first, temp));
    }
    return (result);
}

state_map combine_state(state_map const &state_a, state_map const &state_b)
{
    state_map result = state_a;
    result.insert(state_b.begin(), state_b.end());
    return result;
}

state_map at(state_vector_map const &vector_map, vector<double>::size_type const n)
{
    state_map result;
    result.reserve(vector_map.size());
    for (auto it = vector_map.begin(); it != vector_map.end(); ++it) {
        result.insert(std::pair<string, double>(it->first, it->second.at(n)));
    }
    return result;
}

state_map replace_state(state_map const &state, state_map const &newstate)
{
    state_map result = state;
    for (auto it = result.begin(); it != result.end(); ++it) {
        it->second = newstate.at(it->first);
    }
    return result;
}

state_map update_state(state_map const &state, state_map const &change_in_state)
{
    state_map result = state;
    for (auto it = result.begin(); it != result.end(); ++it) {
        auto it_change = change_in_state.find(it->first);
        if ( it_change != change_in_state.end()) {
            it->second += it_change->second;
        }
    }
    return result;
}

void append_state_to_vector(state_map const &state, state_vector_map &state_vector)
{
    for (auto it = state.begin(); it != state.end(); ++it) {
        state_vector[it->first].push_back(it->second);
    }
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


state_map& operator+=(state_map &lhs, state_map const &rhs)
{
    for(auto it = rhs.begin(); it != rhs.end(); ++it) {
        lhs[it->first] += it->second;
    }
    return lhs;
}

vector<string> keys(state_map const &state) {
    vector<string> result;
    for (auto it = state.begin(); it != state.end(); ++it) {
        result.push_back(it->first);
    }
    return (result);
}

vector<string> keys(state_vector_map const &state) {
    vector<string> result;
    for (auto it = state.begin(); it != state.end(); ++it) {
        result.push_back(it->first);
    }
    return (result);
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

