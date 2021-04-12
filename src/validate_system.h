#ifndef VALIDATE_SYSTEM_H
#define VALIDATE_SYSTEM_H

#include <set>
#include <string>
#include <set>
#include <functional>  // For std::function
#include <memory>      // For std::unique_ptr and std::shared_ptr
#include <algorithm>   // For std::find
#include "state_map.h"
#include "module_library/module_wrapper_factory.h"
#include "modules.h"

using string_set = std::set<std::string>;
using module_vector = std::vector<std::unique_ptr<Module>>;

const std::string success_mark { "[pass] " };
const std::string failure_mark { "[fail] " };

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

state_map define_quantity_map(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_vector find_duplicate_quantity_definitions(string_vector quantity_names);

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

string_set find_strictly_required_inputs(std::vector<string_vector> module_name_vectors);

string_vector find_unused_input_parameters(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_vector find_static_output_parameters(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors);

string_vector find_adaptive_incompatibility(std::vector<string_vector> module_name_vectors);

string_vector find_mischaracterized_modules(std::vector<string_vector> module_name_vectors, bool is_deriv);

module_vector get_module_vector(
    std::vector<string_vector> module_name_vectors,
    const state_map* input_parameters,
    state_map* output_parameters);

std::string add_indented_line(std::string message, std::string text_to_add, int num_spaces);

void insert_quantity_name_if_new(
    std::string quantity_name,
    string_vector& target_vector,
    string_vector& duplicate_vector);

void insert_module_param_if_undefined(
    std::string param_name,
    std::string module_name,
    string_vector defined_quantity_names,
    string_vector& undefined_module_names);
    
string_vector string_set_to_string_vector(string_set ss);
    
string_set string_vector_to_string_set(string_vector sv);

string_vector string_vector_difference(
    string_vector find_elements_of_this_vector,
    string_vector that_are_not_in_this_one);

/**
 * @brief Template function for testing a system input criterion
 *
 * This function first calls criterion_test, which should return some kind of
 * container of strings, e.g. std::vector<std::string> or std::set<std::string>.
 * A non-empty result generally corresponds to failure to meet the criterion.
 * 
 * The output of criterion_test is then passed to form_message, which should
 * create a user feedback message based on the result of the criterion test.
 * 
 * The output of form_message is added to the message input (passed by reference).
 * 
 * Finally, the number of elements in the output of criterion_test is returned.
 * Again, a non-zero value generally corresponds to a failure to meet the
 * criterion specified by criterion_test.
 *
 * @param[in,out] message A string, to which this function will append the
 *                        result of the criterion test.
 *
 * @param[in] criterion_test A function object having no input parameters and
 *                           returning a string_list_type container.
 *
 * @param[in] form_message A function object having on parameter of type
 *                         string_list_type and returning a string.  This will
 *                         generally be a function that cobbles together a
 *                         readable message from the collection of strings given
 *                         as input.
 *
 * @returns The number of elements in the container returned by criterion_test.
 *          Generally, a non-zero value corresponds to a failure to meet the
 *          criterion tested by criterion_test.
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
 * Forms a user feedback message from a list of quantities, modules, etc.
 *
 * @param[in] message_if_empty This is the string that will be returned
 *                             (surrounded by new-line characters) if
 *                             string_list is empty.
 *
 * @param[in] message_at_beginning This string is prepended (after being
 *                                 surrounded by new-line characters) to the
 *                                 body of the message if string_list is _not_
 *                                 empty.
 *
 * @param[in] message_at_end This string is appended to the body of the message
 *                           (along with a new-line character) if string_list is
 *                           not empty.
 *
 * @param[in] string_list A list of strings that will form the body of the
 *                        message.
 */
template <typename string_list_type>
std::string create_message(std::string message_if_empty,
                           std::string message_at_beginning,
                           std::string message_at_end,
                           string_list_type string_list,
                           std::string success_indicator = "",
                           std::string failure_indicator = "")
{
    std::string message;

    if (string_list.size() == 0) {
        message = std::string{"\n"} + success_indicator + message_if_empty + "\n";
    } else {
        message = std::string{"\n"} + failure_indicator + message_at_beginning + "\n";
        for (std::string text_item : string_list) {
            message = add_indented_line(message, text_item, 1);
        }
        if (message_at_end.size() > 0) {
            message += message_at_end + "\n";
        }
    }

    return message;
}

/**
 * Forms a user feedback message from a list of quantities, modules,
 * etc.  This function should be used in preference to
 * `create_message` for the case where a non-empty `string_list`
 * corresponds to some failure condition and an empty `string_list`
 * corresponds to success, and we want to prepend success/failure
 * indicators to the message.
 *
 * @param[in] message_if_empty This is the string that will be
 *                             returned (surrounded by new-line
 *                             characters and an initial "success"
 *                             indicator) if string_list is empty.
 *
 * @param[in] message_at_beginning This string is prepended (after
 *                                 being surrounded by new-line
 *                                 characters and an initial "failure"
 *                                 indicator) to the body of the
 *                                 message if string_list is _not_
 *                                 empty.
 *
 * @param[in] message_at_end This string is appended to the body of the message
 *                           (along with a new-line character) if string_list is
 *                           not empty.
 *
 * @param[in] string_list A list of strings that will form the body of the
 *                        message.
 */
template <typename string_list_type>
std::string create_marked_message(std::string message_if_empty,
                                  std::string message_at_beginning,
                                  std::string message_at_end,
                                  string_list_type string_list)
{
    return create_message(message_if_empty,
                          message_at_beginning,
                          message_at_end,
                          string_list,
                          success_mark,
                          failure_mark);
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
 * Assembles the names of all quantities defined by a group of state_maps and
 * module outputs, including any duplicates.
 *
 * @param[in] state_maps A collection of sets of named quantities presented as a
 *                       vector of map_with_string_keys objects.  Generally, this
 *                       will either be empty or will consist of the initial state
 *                       of the system, the set of invariant parameters, and the
 *                       initial values of the varying parameters.
 * @param[in] module_name_vectors A collection of sets of module names presented
 *                                as a vector of vectors.  Usually, this
 *                                collection will either be empty or will
 *                                contain only a single item---a set of
 *                                steady-state module names or a set of
 *                                derivative module names.
 * @return A vector consisting of the names of all quantities defined in either
 *         of the function arguments.  **If a quantity is defined more than
 *         once, it will appear in the output vector more than once.** A
 *         quantity is considered to be "defined" by the state_maps argument
 *         value if it is a key in one of the given maps.  It is considered
 *         "defined" by the module_name_vectors argument value if it is a name
 *         of an output variable of some module named in any set in the
 *         collection.
 */
template <typename map_with_string_keys>
string_vector get_defined_quantity_names(
    std::vector<map_with_string_keys> state_maps,
    std::vector<string_vector> module_name_vectors)
{
    std::vector<std::string> defined_quantity_names;

    // Get quantity names from the state maps
    for (map_with_string_keys const& m : state_maps) {
        insert_key_names(defined_quantity_names, m);
    }

    // Get quantity names from the modules
    for (string_vector const& names : module_name_vectors) {
        for (std::string const& module_name : names) {
            auto w = module_wrapper_factory::create(module_name);
            string_vector output_names = w->get_outputs();
            defined_quantity_names.insert(defined_quantity_names.begin(), output_names.begin(), output_names.end());
        }
    }

    return defined_quantity_names;
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
