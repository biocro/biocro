
#include <iostream>
#include <stdexcept>
#include <R.h>
#include "modules.h"

vector<string> IModule::list_required_state() {
    return(this->_required_state);
}

vector<string> IModule::list_modified_state() {
    return(this->_modified_state);
}

map<string, double> IModule::run(map<string, double> const &state) {
    map<string, double> result;
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

vector<string> IModule::state_requirements_are_met(map<string, double> const &s) {
    vector<string> missing_state;
    for (auto it = _required_state.begin(); it != _required_state.end(); ++it) {
        if (s.find(*it) == s.end()) {
            missing_state.push_back(*it);
        }
    }
    return(missing_state);
}

map<string, double> c4_canopy::do_operation(map<string, double> const &s) {
    struct Can_Str result;
    struct nitroParms nitroP; 
    map<string, double> fluxes;

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

map<string, double> c3_canopy::do_operation(map<string, double> const &s) {
    struct Can_Str result;
    map<string, double> fluxes;

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

struct c3_str c3_leaf::assimilation(map<string, double> s) {
    struct c3_str result = {0, 0, 0, 0};
    result = c3photoC(s.at("Qp"), s.at("Tleaf"), s.at("RH"), s.at("Vcmax0"), s.at("Jmax"), s.at("Rd0"), s.at("bb0"), s.at("bb1"), s.at("Ca"), s.at("O2"), s.at("thet"), s.at("StomWS"), s.at("ws"));
    return(result);
}

map<string, vector<double>> allocate_state(map<string, double> m, int n) {
    map<string, vector<double>> result;
    for (map<string, double>::iterator it = m.begin(); it != m.end(); ++it) {
        vector<double> temp;
        temp.reserve(n);
        temp.push_back(it->second);
        result.insert(std::pair<string, vector<double>>(it->first, temp));
    }
    return(result);
}

state_map combine_state(state_map const &state, state_map const &invariant_parameters, state_vector_map const &varying_parameters, int timestep) {
    state_map all_state = state;
    all_state.insert(invariant_parameters.begin(), invariant_parameters.end());
    for (auto it = varying_parameters.begin(); it != varying_parameters.end(); ++it) {
        all_state.insert(std::pair<string, double>(it->first, it->second[timestep]));
    }
    return all_state;
}

state_map replace_state(state_map const &state, state_map const &newstate) {
    state_map result = state;
    for (auto it = result.begin(); it != result.end(); ++it) {
        it->second = newstate.at(it->first);
    }
    return result;
}

double biomass_leaf_nitrogen_limitation(state_map const &s)
{
    double leaf_n = 0;
    leaf_n = s.at("LeafN_0") * pow(s.at("Leaf") + s.at("Stem"), -s.at("kln"));
    return(leaf_n > s.at("LeafN_0") ? s.at("LeafN_0") : leaf_n);
}

