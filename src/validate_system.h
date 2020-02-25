#ifndef VALIDATE_SYSTEM_H
#define VALIDATE_SYSTEM_H

#include <string>
#include "state_map.h"

bool validate_system_inputs(
    std::string& message,
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names);

std::vector<std::string> define_quantities(
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

std::string create_message_from_duplicated_quantities(std::vector<std::string> duplicated_quantities);

std::string create_message_from_undefined_module_inputs(std::vector<std::string> undefined_module_inputs);

std::string create_message_from_undefined_module_outputs(std::vector<std::string> undefined_module_outputs);

/**
 * Inserts all the keys from map into name_vector
 */
template <typename map_type>
void insert_key_names(std::vector<std::string>& name_vector, map_type map)
{
    std::vector<std::string> map_key_names = keys(map);
    name_vector.insert(name_vector.begin(), map_key_names.begin(), map_key_names.end());
}

void insert_quantity_name_if_new(
    std::string quantity_name,
    std::vector<std::string>& target_vector,
    std::vector<std::string>& duplicate_vector);

void insert_module_input_if_undefined(
    std::string input_name,
    std::string module_name,
    std::vector<std::string> defined_quantity_names,
    std::vector<std::string>& undefined_module_inputs);

#endif