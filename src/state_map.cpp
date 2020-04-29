#include "state_map.h"

using std::vector;
using std::string;

// Forward declare; defined below.
vector<double>::size_type  valid(const state_vector_map& vector_map);

/**
 * @brief Given a sequence of states (presented as a state_vector_map)
 *        and an index n, return the n'th state in the sequence
 *        (counting from 0) in the form of a state_map.
 *
 * @param vector_map A valid sequence of states represented as a
 *                   state_vector_map.
 *
 * @param n An index into the sequence of state given by vector_map.
 *
 * @returns The n'th state (counting from 0) of the sequence of states
 *          represented by vector_map.
 *
 * @throws invalid_state_vector_map if vector_map is invalid.
 *
 * @throws bad_state_vector_map_index if n is not a valid index for
 *                                    the vector values of the map.
 *
 */
state_map at(const state_vector_map& vector_map,
             std::vector<double>::size_type n)
{
    const auto length = valid(vector_map);

    state_map result;
    result.reserve(vector_map.size());

    if (! (n >= 0 && n < length)) {
        throw bad_state_vector_map_index(std::string{"Index "} +
                                         std::to_string(n) +
                                         " is out of range for vector_map.");
    }

    for (auto pair : vector_map) {
        result.emplace(pair.first, pair.second[n]);
    }
    return result;
}

/**
 * Check that vector_map is a valid state_vector_map, and if it is,
 * return the length of the vectors.
 *
 * In order for vector_map to be valid, it must have at least one key
 * (i.e., not be empty), and the values corresponding to the keys must
 * be of uniform length.
 *
 * @param vector_map The state_vector_map whose size is being
 *                   assessed.
 *
 * @returns The length of the vectors comprising the values of the
 *          map.
 *
 * @throws invalid_state_vector_map if vector_map is invalid.
 *
 * @note A state_vector_map is used to represent sequences of
 *       states. In theory, we might wish to consider and represent a
 *       sequence of states having no attributes, but a
 *       state_vector_map can't do this, for there is no way to
 *       represent the length of such a sequence if there isn't at
 *       least one attribute.  (Had we used std::vector<state_map> to
 *       represent sequences of states instead, this problem wouldn't
 *       arise.)
 */
vector<double>::size_type valid(const state_vector_map& vector_map) {
    if (vector_map.size() == 0) {
        throw invalid_state_vector_map(std::string("state_vector_map has no keys"));
    }
    auto it = vector_map.begin();

    auto size = it->second.size();

    for (++it; it != vector_map.end(); ++it) {
        if (it->second.size() != size) {
            throw invalid_state_vector_map(std::string("state_vector_map has vectors of unequal lengths"));
        }
    }

    return size;
}
