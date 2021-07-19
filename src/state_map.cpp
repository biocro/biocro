#include "state_map.h"

using std::vector;
using std::string;

/**
 * @brief Given a sequence of states (presented as a state_vector_map)
 *        and an index n, return the n'th state in the sequence
 *        (counting from 0) in the form of a state_map.
 *
 * @param vector_map A sequence of states represented as a
 *                   state_vector_map.
 *
 * @param n An index into the sequence of states given by vector_map.
 *
 * @returns The n'th state (counting from 0) of the sequence of states
 *          represented by vector_map.
 *
 * @throws bad_state_vector_map_index if n is not a valid index for
 *                                    some or all the vector values of
 *                                    the map.
 *
 */
state_map at(const state_vector_map& vector_map,
             std::vector<double>::size_type n)
{
    state_map result;

    try {
        for (auto pair : vector_map) {
            result.emplace(pair.first, pair.second.at(n));
        }
    }
    catch (std::out_of_range const&) {
        throw bad_state_vector_map_index(std::string{"Index "} +
                                         std::to_string(n) +
                                         " is out of range for vector_map.");
    }
    return result;
}
