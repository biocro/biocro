#ifndef STATE_MAP_H
#define STATE_MAP_H

#include <unordered_map>
#include <vector>
#include <string>

/**
 * `string_vector` serves as an alias for a type widely used to
 * hold lists of module names or lists of module parameters.
 * Formally, it is simply a `std::vector` of `std::string`s.
 */
using string_vector = std::vector<std::string>;

/**
 * A `state_vector_map` represents a chronologically-ordered sequence
 * of states of a system over some period of time.  The keys of the
 * map are attributes of the system; each value is a vector
 * representing the sequence of values of one of those attributes as
 * it changes over time.  To be a valid representation, each of the
 * mapped-to vectors should be of the same length.
 */
using state_vector_map = std::unordered_map<std::string, std::vector<double>>;

/**
 * Exception thrown when the `at` function tries access the state in a
 * state_vector_map using an out-of-range index.
 */
struct bad_state_vector_map_index : std::out_of_range {
    bad_state_vector_map_index(std::string msg) :out_of_range{msg} { }
};

/**
 * A `state_map` represents the _state_ of a system.  It maps names of
 * system attributes to the values they have at some particular time.
 */
using state_map = std::unordered_map<std::string, double>;

/**
 * Given a map, return a list of its keys.
 *
 * @param map A map having keys of type `std::string`.
 *
 * @returns A vector containing all of the keys of the map.
 *
 * @note The minimal requirement on the "map" argument is that it be a
 * "range" or "sequence" of objects that have a "first" member
 * function yielding a string, what we are considering to be the "key"
 * of the object.
 */
template <typename map_with_string_key_type>
string_vector keys(map_with_string_key_type const& map)
{
    string_vector result;
    for (auto const& it : map) {
        result.push_back(it.first);
    }
    return result;
}

state_map at(const state_vector_map& vector_map,
             std::vector<double>::size_type n);

/**
 * Given a list of names, create a `state_map` object whose keys are
 * precisely those names and whose values are all zero.
 */
template <typename name_list_type>
state_map state_map_from_names(name_list_type names)
{
    state_map m;
    for (std::string const& n : names) {
        m[n] = 0;
    }
    return m;
}

state_map abs(state_map x);

#endif

