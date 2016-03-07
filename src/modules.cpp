
#include <iostream>
#include <memory>
#include <stdexcept>
#include <R.h>
#include "modules.h"

vector<string> IModule::list_required_state() const
{
    return(this->_required_state);
}

vector<string> IModule::list_modified_state() const
{
    return(this->_modified_state);
}

state_map IModule::run(state_map const &state) const
{
    state_map result;
    try {
        result = this->do_operation(state);
    }
    catch (std::out_of_range const &oor) {
        vector<string> missing_state = this->state_requirements_are_met(state);

        if (!missing_state.empty()) {
            Rprintf("The following state variables are required but are missing: ");
            for(vector<string>::iterator it = missing_state.begin(); it != missing_state.end() - 1; ++it) {
                Rprintf("%s, ", it->c_str());
            }
            Rprintf("%s.\n", missing_state.back().c_str());
            error("This function cannot continue unless all state variables are set.");
        } else {
            error("An out of range exception was thrown, but all required state variables are present.\nYou probabaly mistyped a variable in the C++ code\nor there is a required variable that has not been added to the module declaration.\n");
        }

    }
    return(result);
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

        if (!missing_state.empty()) {
            Rprintf("The following state variables are required but are missing: ");
            for(vector<string>::iterator it = missing_state.begin(); it != missing_state.end() - 1; ++it) {
                Rprintf("%s, ", it->c_str());
            }
            Rprintf("%s.\n", missing_state.back().c_str());
            error("This function cannot continue unless all state variables are set.");
        } else {
            error("An out of range exception was thrown, but all required state variables are present.\nYou probabaly mistyped a variable in the C++ code\nor there is a required variable that has not been added to the module declaration.\n");
        }

    }
    return(result);
}

state_map IModule::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &p) const
{
    state_map current_state = combine_state(at(state_history, state_history.begin()->second.size()), p);
    return(this->run(current_state));
}

vector<string> IModule::state_requirements_are_met(state_map const &s) const
{
    vector<string> missing_state;
    for (auto it = _required_state.begin(); it != _required_state.end(); ++it) {
        if (s.find(*it) == s.end()) {
            missing_state.push_back(*it);
        }
    }
    return(missing_state);
}

state_map c4_canopy::do_operation(state_map const &s) const
{
    struct Can_Str result;
    struct nitroParms nitroP; 
    state_map fluxes;

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

    result = CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax1"),
            s.at("alpha1"), s.at("kparm"), s.at("beta"), s.at("Rd"), s.at("Catm"),
            s.at("b0"), s.at("b1"), s.at("theta"), s.at("kd"), s.at("chil"),
            s.at("heightf"), s.at("LeafN"), s.at("kpLN"), s.at("lnb0"), s.at("lnb1"),
            (int)s.at("lnfun"), s.at("upperT"), s.at("lowerT"), nitroP, s.at("leafwidth"),
            (int)s.at("et_equation"), s.at("StomataWS"), (int)s.at("ws"));

    fluxes["Assim"] = result.Assim;
    fluxes["Trans"] = result.Trans;
    fluxes["GrossAssim"] = result.GrossAssim;

    return(fluxes);
}

state_map c4_canopy::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    struct Can_Str result;
    struct nitroParms nitroP; 
    state_map fluxes;

    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);

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

    result = CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax1"),
            s.at("alpha1"), s.at("kparm"), s.at("beta"), s.at("Rd"), s.at("Catm"),
            s.at("b0"), s.at("b1"), s.at("theta"), s.at("kd"), s.at("chil"),
            s.at("heightf"), s.at("LeafN"), s.at("kpLN"), s.at("lnb0"), s.at("lnb1"),
            (int)s.at("lnfun"), s.at("upperT"), s.at("lowerT"), nitroP, s.at("leafwidth"),
            (int)s.at("et_equation"), s.at("StomataWS"), (int)s.at("ws"));

    fluxes["Assim"] = result.Assim;
    fluxes["Trans"] = result.Trans;
    fluxes["GrossAssim"] = result.GrossAssim;

    return(fluxes);
}

state_map c3_canopy::do_operation(state_map const &s) const
{
    struct Can_Str result;
    state_map fluxes;

    result = c3CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solarr"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax"),
            s.at("jmax"), s.at("rd"), s.at("catm"), s.at("o2"), s.at("b0"),
            s.at("b1"), s.at("theta"), s.at("kd"), s.at("heightf"), s.at("leafn"),
            s.at("kpln"), s.at("lnb0"), s.at("lnb1"), (int)s.at("lnfun"), s.at("stomataws"),
            (int)s.at("ws"));

    fluxes["Assim"] = result.Assim;
    fluxes["Trans"] = result.Trans;
    fluxes["GrossAssim"] = result.GrossAssim;

    return(fluxes);
}

state_map one_layer_soil_profile::do_operation(state_map const &s) const
{
    double soilEvap, TotEvap;
    struct ws_str WaterS = {0, 0, 0, 0, 0, 0};
    state_map derivs;

    soilEvap = SoilEvapo(s.at("lai"), 0.68, s.at("temp"), s.at("solar"), s.at("waterCont"),
                s.at("FieldC"), s.at("WiltP"), s.at("windspeed"), s.at("rh"), s.at("rsec"));
    TotEvap = soilEvap + s.at("CanopyT");

    WaterS = watstr(s.at("precip"), TotEvap, s.at("waterCont"), s.at("soilDepth"), s.at("FieldC"),
            s.at("WiltP"), s.at("phi1"), s.at("phi2"), s.at("soilType"), s.at("wsFun"));

    //derivs["waterCont"] = s.at("waterCont") - WaterS.awc;
    //derivs["StomataWS"] = s.at("StomataWS") - WaterS.rcoefPhoto;
    //derivs["LeafWS"] = s.at("LeafWS") - WaterS.rcoefSpleaf;
    derivs["soilEvap"] = soilEvap;
    derivs["waterCont"] =  WaterS.awc;
    derivs["StomataWS"] =  WaterS.rcoefPhoto;
    derivs["LeafWS"] =  WaterS.rcoefSpleaf;
    return(derivs);
}

state_map one_layer_soil_profile::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const {
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);
    return one_layer_soil_profile::do_operation(s);
}

state_map thermal_time_senescence_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
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

    /*
    if (i % 24 * s.at("centTimestep") == 0) {
        double coefficient = ((0.1 / 30) * s.at("centTimestep"));
        derivs["LeafLitter"] -= s.at("LeafLitter") * coefficient;
        derivs["StemLitter"] -= s.at("StemLitter") * coefficient;
        derivs["RootLitter"] -= s.at("RootLitter") * coefficient;
        derivs["RhizomeLitter"] -= s.at("RhizomeLitter") * coefficient;
    }
    */
    return(derivs);
}

state_map partitioning_growth_module::do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const
{
    state_map derivs;
    state_map s = combine_state(at(state_history, state_history.begin()->second.size() - 1), parameters);
    double kLeaf = s.at("kLeaf");
    double kStem = s.at("kStem");
    double kRoot = s.at("kRoot");
    double kRhizome = s.at("kRhizome");
    double kGrain = s.at("kGrain");
    double CanopyA = s.at("CanopyA");

    if (kLeaf > 0) {
        derivs["newLeafcol"] = CanopyA * kLeaf * s.at("LeafWS");
        /*  The major effect of water stress is on leaf expansion rate. See Boyer (1970)
            Plant. Phys. 46, 233-235. For this the water stress coefficient is different
            for leaf and vmax. */

        /* Tissue respiration. See Amthor (1984) PCE 7, 561-*/ 
        derivs["newLeafcol"] = resp(derivs["newLeafcol"], s.at("mrc1"), s.at("temp"));

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
        derivs["newStemcol"] = resp(derivs["newStemcol"], s.at("mrc1"), s.at("temp"));
        derivs["Stem"] += derivs["newStemcol"];
    } else {
        error("kStem should be positive");
    }

    if (kRoot > 0) {
        derivs["newRootcol"] = CanopyA * kRoot;
        derivs["newRootcol"] = resp(derivs["newRootcol"], s.at("mrc2"), s.at("temp"));
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
        derivs["newRhizomecol"] = resp(derivs["newRhizomecol"], s.at("mrc2"), s.at("temp"));
        derivs["Rhizome"] += derivs["newRhizomecol"];
        /* Here i will not work because the rhizome goes from being a source
           to a sink. I need its own index. Let's call it rhizome's i or ri.*/
        ++derivs["rhizome_index"];
    } else {
        derivs["newRhizomecol"] = 0;
        derivs["Rhizome"] += s.at("Rhizome") * kRhizome;
        if ( derivs["Rhizome"] > s.at("Rhizome") ) {  // Check if this would make the rhizome mass negative.
            derivs["Rhizome"] = s.at("Rhizome");
            warning("Rhizome flux would make rhizome mass negative.");
        }

        derivs["Root"] += kRoot * -derivs.at("Rhizome");
        derivs["Stem"] += kStem * -derivs.at("Rhizome");
        derivs["Leaf"] += kLeaf * -derivs.at("Rhizome");
        derivs["Grain"] += kGrain * -derivs.at("Rhizome");
    }

    if ((kGrain >= 1e-10) && (s["TTc"] >= s.at("tp5"))) {
        derivs["Grain"] += CanopyA * kGrain;
        /* No respiration for grain at the moment */
        /* No senescence either */
    }
    return(derivs);
}

struct c3_str c3_leaf::assimilation(state_map s)
{
    struct c3_str result = {0, 0, 0, 0};
    result = c3photoC(s.at("Qp"), s.at("Tleaf"), s.at("RH"), s.at("Vcmax0"), s.at("Jmax"), s.at("Rd0"), s.at("bb0"), s.at("bb1"), s.at("Ca"), s.at("O2"), s.at("thet"), s.at("StomWS"), s.at("ws"));
    return(result);
}

std::unique_ptr<IModule> make_module(string const &module_name)
{
    if (module_name.compare("c4_canopy") == 0) {
        return std::unique_ptr<IModule>(new c4_canopy);
    }
    else if (module_name.compare("one_layer_soil_profile") == 0) {
        return std::unique_ptr<IModule>(new one_layer_soil_profile);
    }
    else {
        return std::unique_ptr<IModule>(new c3_canopy);
    }
}


state_vector_map allocate_state(state_map const &m, int n)
{
    state_vector_map result;
    for (auto it = m.begin(); it != m.end(); ++it) {
        vector<double> temp;
        temp.reserve(n);
        //temp.push_back(it->second);
        //result.insert(std::pair<string, vector<double>>(it->first, temp));
    }
    return(result);
}

state_map combine_state(state_map const &state_a, state_map const &state_b)
{
    state_map result = state_a;
    result.insert(state_b.begin(), state_b.end());
    return result;
}

state_map at(state_vector_map const vector_map, vector<double>::size_type n)
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
    double leaf_n = 0;
    leaf_n = s.at("LeafN_0") * pow(s.at("Leaf") + s.at("Stem"), -s.at("kln"));
    return(leaf_n > s.at("LeafN_0") ? s.at("LeafN_0") : leaf_n);
}

state_map& operator+=(state_map &lhs, state_map const &rhs)
{
    for(auto it = rhs.begin(); it != rhs.end(); ++it) {
        lhs[it->first] += it->second;
    }
    return lhs;
}
