#ifndef VALIDATE_SYSTEM_H
#define VALIDATE_SYSTEM_H

#include <string>
#include "state_map.h"
#include "modules.h"

bool validate_system_inputs(
    std::string& message,
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names);

std::string analyze_system_inputs(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names);

std::vector<std::string> define_quantity_names(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names);

state_map define_quantity_map(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names);

std::vector<std::string> find_multiple_quantity_definitions(std::vector<std::string> quantity_names);

std::vector<std::string> find_undefined_module_inputs(
    std::vector<std::string> quantity_names,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names);

std::vector<std::string> find_undefined_module_outputs(
    state_map initial_state,
    std::vector<std::string> deriv_module_names);

std::vector<std::string> find_misordered_modules(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names);

std::set<std::string> find_all_module_inputs(
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names);

std::vector<std::string> find_unused_parameters(
    std::set<std::string> all_module_inputs,
    state_map invariant_params);

std::string create_message_from_duplicated_quantities(std::vector<std::string> duplicated_quantities);

std::string create_message_from_undefined_module_inputs(std::vector<std::string> undefined_module_inputs);

std::string create_message_from_undefined_module_outputs(std::vector<std::string> undefined_module_outputs);

std::string create_message_from_misordered_modules(std::vector<std::string> misordered_modules);

std::string create_message_from_unused_invariant_params(std::vector<std::string> unused_invariant_params);

void insert_quantity_name_if_new(
    std::string quantity_name,
    std::vector<std::string>& target_vector,
    std::vector<std::string>& duplicate_vector);

void insert_module_param_if_undefined(
    std::string param_name,
    std::string module_name,
    std::vector<std::string> defined_quantity_names,
    std::vector<std::string>& undefined_module_inputs);

void insert_quantity_if_undefined(
    std::string quantity_name,
    std::vector<std::string> defined_quantity_names,
    std::vector<std::string>& undefined_quantity_names);

/**
 * Inserts all the keys from map into name_vector
 */
template <typename map_type>
void insert_key_names(std::vector<std::string>& name_vector, map_type map)
{
    std::vector<std::string> map_key_names = keys(map);
    name_vector.insert(name_vector.begin(), map_key_names.begin(), map_key_names.end());
}

/**
 * If quantity_name is not included in defined_quantity_names,
 * it is inserted into undefined_quantity_names
 */
template <typename list_type>
void insert_quantity_if_undefined(
    std::string quantity_name,
    list_type defined_quantity_names,
    std::vector<std::string>& undefined_quantity_names)
{
    if (std::find(defined_quantity_names.begin(), defined_quantity_names.end(), quantity_name) == defined_quantity_names.end()) {
        undefined_quantity_names.push_back(quantity_name);
    }
}

/**
 * Checks whether all of the elements of list find_these are in the list in_this
 */
template <typename list_type>
bool all_are_in_list(list_type find_these, list_type in_this)
{
    bool found;
    for (auto const& find_this : find_these) {
        found = false;
        for (auto const& this_it : in_this) {
            if (find_this == this_it) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
    }
    return found;
}

#endif