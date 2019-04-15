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


