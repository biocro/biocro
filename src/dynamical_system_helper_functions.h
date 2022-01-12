#ifndef DYNAMICAL_SYSTEM_HELPER_FUNCTIONS_H
#define DYNAMICAL_SYSTEM_HELPER_FUNCTIONS_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>     // For unique_ptr
#include "module.h"  // For module_vector
#include "validate_dynamical_system.h"
#include "constants.h"

/**
 * @brief Returns pairs of pointers that each point to the same named
 * quantity in each of the two input maps.
 *
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
 * @brief Returns a vector of pointers that point to elements of a map
 * whose targets are specified by quantity_names.
 *
 * Here it is essential that the map is passed by reference; otherwise
 * the pointers will be useless
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
 * @brief Returns a vector of pointers that point to elements of a map
 * whose targets are specified by quantity_names.
 *
 * Here it is essential that the map is passed by reference; otherwise
 * the pointers will be useless
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

bool check_euler_requirement(module_vector const& modules_to_check);

#endif
