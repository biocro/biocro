#ifndef VALIDATE_SYSTEM_H
#define VALIDATE_SYSTEM_H

#include <string>
#include <functional>
#include "state_map.h"
#include "modules.h"

using string_vector = std::vector<std::string>;
using string_set = std::set<std::string>;

bool validate_system_inputs(
    std::string& message,
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    string_vector ss_module_names,
    string_vector deriv_module_names);

std::string analyze_system_inputs(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    string_vector ss_module_names,
    string_vector deriv_module_names);

string_vector define_quantity_names(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

state_map define_quantity_map(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_vector find_multiple_quantity_definitions(string_vector quantity_names);

string_vector find_undefined_module_inputs(
    string_vector quantity_names,
    std::vector<string_vector> module_name_vectors);

string_vector find_undefined_module_outputs(
    string_vector quantity_names,
    std::vector<string_vector> module_name_vectors);

string_vector find_misordered_modules(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_set find_unique_module_inputs(std::vector<string_vector> module_name_vectors);

string_set find_unique_module_outputs(std::vector<string_vector> module_name_vectors);

string_vector find_unused_input_parameters(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_vector find_static_output_parameters(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

void add_indented_line(std::string& message, std::string text_to_add, int num_spaces);

void insert_quantity_name_if_new(
    std::string quantity_name,
    string_vector& target_vector,
    string_vector& duplicate_vector);

void insert_module_param_if_undefined(
    std::string param_name,
    std::string module_name,
    string_vector defined_quantity_names,
    string_vector& undefined_module_names);

/**
 * @brief Function for testing a system input criterion
 * 
 * This function first calls criterion_test, which should return some kind
 * of string container, e.g. std::vector<std::string> or std::set<std::string>.
 * 
 * The output of criterion_test is then passed to form_message, which should
 * create a user feedback message based on the result of the criterion test.
 * 
 * The output of form_message is added to the message input (passed by reference).
 * 
 * Finally, the number of elements in the output of criterion_test is returned
 * 
 */
template <typename string_list_type>
size_t process_criterion(
    std::string& message,
    std::function<string_list_type()> criterion_test,
    std::function<std::string(string_list_type)> form_message)
{
    string_list_type test_results = criterion_test();
    std::string new_message = form_message(test_results);
    message += new_message;
    return test_results.size();
}

/**
 * Forms a user feedback message from a list of quantities, modules, etc
 */
template <typename string_list_type>
std::string create_message(std::string message_if_empty, std::string message_at_beginning, std::string message_at_end, string_list_type string_list)
{
    std::string message;

    if (string_list.size() == 0) {
        message = std::string("\n") + message_if_empty + std::string("\n");
    } else {
        message = std::string("\n") + message_at_beginning + std::string("\n");
        for (std::string text_item : string_list) {
            add_indented_line(message, text_item, 1);
        }
        if (message_at_end.size() > 0) {
            message += message_at_end + std::string("\n");
        }
    }

    return message;
}

/**
 * Inserts all the keys from map into name_vector
 */
template <typename map_type>
void insert_key_names(string_vector& name_vector, const map_type map)
{
    string_vector map_key_names = keys(map);
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
    string_vector& undefined_quantity_names)
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