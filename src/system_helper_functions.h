#ifndef SYSTEM_HELPER_FUNCTIONS_H
#define SYSTEM_HELPER_FUNCTIONS_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>  // For unique_ptr
#include "validate_system.h"
#include "constants.h"

/**
 * Returns pairs of pointers that each point to the same named quantity in each of the two input maps.
 * Here it is essential that map_1 and map_2 are passed by reference; otherwise the pointers will be
 * useless.
 */
template <typename name_list_type, typename value_type_1, typename value_type_2>
std::vector<std::pair<value_type_1*, const value_type_2*>> get_pointer_pairs(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type_1>& map_1,
    const std::unordered_map<std::string, value_type_2>& map_2)
{
    std::vector<std::pair<value_type_1*, const value_type_2*>> pointer_pairs;

    for (const std::string& name : quantity_names) {
        std::pair<value_type_1*, const value_type_2*>
            temporary{&(map_1.at(name)), &(map_2.at(name))};
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * @brief Returns pairs of pointers that each point to the same named quantity in different places.
 * 
 * The resulting pairs are intended to be used for updating a central map of quantities (the `target_map`,
 * which is probably the `quantities` member of a Standalone_SS object) using values from another source
 * (the `source_map`, which probably points to variables outside the Standalone_SS object).
 * 
 * One pair will be created for each named quantity in the `source_map` input.
 * 
 * Here it is essential that the `target_map` is passed by reference; otherwise the pointer pairs will be useless.
 * 
 * @param[in] target_map An unordered map with string keys and elements that are variables of type `target_value_type` (often double).
 * 
 * @param[in] source_map An unordered map with string keys and elements that point to variables of type `source_value_type' (often double).
 * 
 * @returns A vector of pairs of pointers. The first pointer in each pair points to a "target" variable in the
 *          `target_map` input, while the second pointer points to the same named quantity in another location,
 *          as specified by the `source_map` input.
 */
template <typename target_value_type, typename source_value_type>
std::vector<std::pair<target_value_type*, const source_value_type*>> get_input_pointer_pairs(
    std::unordered_map<std::string, target_value_type>& target_map,
    std::unordered_map<std::string, const source_value_type*> const& source_map)
{
    std::vector<std::pair<target_value_type*, const source_value_type*>> pointer_pairs;

    for (auto const& it : source_map) {
        std::pair<target_value_type*, const source_value_type*> temporary(&target_map.at(it.first), it.second);
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * @brief Returns pairs of pointers that each point to the same named quantity in different places.
 * 
 * The resulting pairs are intended to be used for updating quantities (specified by the keys and pointers
 * in the `target_map`) using values from the `source_map` (which is usually the `quantities` member of a
 * Standalone_SS object).
 * 
 * One pair will be created for each named quantity in the `target_map` input.
 * 
 * Here it is essential that the `source_map` is passed by reference; otherwise the pointer pairs will be useless.
 * 
 * @param[in] source_map An unordered map with string keys and elements that are variables of type `source_value_type' (often double).
 * 
 * @param[in] target_map An unordered map with string keys and elements that point to variables of type `target_value_type` (often double).
 * 
 * @returns A vector of pairs of pointers. The first pointer in each pair points to a "source" variable in the
 *          `source_map` input, while the second pointer points to the same named quantity in another location,
 *          as specified by the `target_map` input.
 */
template <typename source_value_type, typename target_value_type>
std::vector<std::pair<const source_value_type*, target_value_type*>> get_output_pointer_pairs(
    std::unordered_map<std::string, source_value_type> const& source_map,
    std::unordered_map<std::string, target_value_type*> const& target_map)
{
    std::vector<std::pair<const source_value_type*, target_value_type*>> pointer_pairs;

    for (auto const& it : target_map) {
        std::pair<const source_value_type*, target_value_type*> temporary(&source_map.at(it.first), it.second);
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * Returns a vector of pointers that point to elements of map whose targets are specified by quantity_names.
 * Here it is essential that the map is passed by reference; otherwise the pointers will be useless
 */
template <typename name_list_type, typename value_type>
std::vector<value_type*> get_pointers(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type>& map)
{
    std::vector<value_type*> pointers;

    for (std::string const& name : quantity_names) {
        pointers.push_back(&map.at(name));
    }

    return pointers;
}

/**
 * Returns a vector of pointers that point to elements of map whose targets are specified by quantity_names.
 * Here it is essential that the map is passed by reference; otherwise the pointers will be useless
 */
template <typename name_list_type, typename value_type>
std::vector<const value_type*> get_const_pointers(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type> const& map)
{
    std::vector<const value_type*> pointers;

    for (std::string const& name : quantity_names) {
        pointers.push_back(&map.at(name));
    }

    return pointers;
}

bool check_adaptive_compatible(const module_vector* ptr_to_module_vector);

#endif
