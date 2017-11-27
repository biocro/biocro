#include "state_map.h"

using std::vector;
using std::string;

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

state_map combine_state(state_map state_a, state_map const &state_b)
{
    state_a.insert(state_b.begin(), state_b.end());
    return state_a;
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

state_map& operator+=(state_map &lhs, state_map const &rhs)
{
    for(auto it = rhs.begin(); it != rhs.end(); ++it) {
        lhs[it->first] += it->second;
    }
    return lhs;
}

state_map& operator+=(state_map &x, double const a)
{
    for(auto it = x.begin(); it != x.end(); ++it) {
        it->second += a;
    }
    return x;
}

state_map& operator*=(state_map &x, double const a)
{
    for(auto it = x.begin(); it != x.end(); ++it) {
        it->second *= a;
    }
    return x;
}

state_map operator/(state_map lhs, state_map const& rhs)
{
    for(auto it = rhs.begin(); it != rhs.end(); ++it) {
        lhs[it->first] /= it->second;
    }
    return lhs;
}

