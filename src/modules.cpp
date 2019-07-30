#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"
#include "state_map.h"

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
