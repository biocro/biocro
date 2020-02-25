#include <algorithm>
#include "validate_system.h"
#include "state_map.h"
#include "module_wrapper_factory.h"

/**
  * @brief Checks over a group of quantities and modules to ensure they can be used to create a valid system
  * @param message User feedback is added to this string
  * @return Indicates whether the inputs are valid
  * 
  * The following criteria are used to determine validity:
  * 1) Each quantity is defined only once
  * 2) All module inputs are defined
  * 3) Derivatives are calculated only for quantities in the initial state
  */
bool validate_system_inputs(
    std::string& message,
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names)
{
    bool valid = true;

    std::vector<std::string> quantity_names = define_quantities(initial_state, invariant_params, varying_params, ss_module_names);

    // Criterion 1
    std::vector<std::string> duplicated_quantities = find_multiple_quantity_definitions(quantity_names);
    message += create_message_from_duplicated_quantities(duplicated_quantities);
    if (duplicated_quantities.size() > 0) {
        valid = false;
    }

    // Criterion 2
    std::vector<std::string> undefined_module_inputs = find_undefined_module_inputs(quantity_names, ss_module_names, deriv_module_names);
    message += create_message_from_undefined_module_inputs(undefined_module_inputs);
    if (undefined_module_inputs.size() > 0) {
        valid = false;
    }

    // Criterion 3
    std::vector<std::string> undefined_module_outputs = find_undefined_module_outputs(initial_state, deriv_module_names);
    message += create_message_from_undefined_module_outputs(undefined_module_outputs);
    if (undefined_module_outputs.size() > 0) {
        valid = false;
    }

    return valid;
}

/**
 * Assembles all quantity definitions from a group of input lists and modules
 */
std::vector<std::string> define_quantities(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names)
{
    std::vector<std::string> defined_quantity_names;

    // Get quantity names from the input lists
    insert_key_names(defined_quantity_names, initial_state);
    insert_key_names(defined_quantity_names, invariant_params);
    insert_key_names(defined_quantity_names, varying_params);

    // Get quantity names from the steady state modules
    for (std::string module_name : ss_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        std::vector<std::string> names = module->get_outputs();
        defined_quantity_names.insert(defined_quantity_names.begin(), names.begin(), names.end());
    }

    return defined_quantity_names;
}

/**
 * Finds quantities that are defined multiple times
 */
std::vector<std::string> find_multiple_quantity_definitions(std::vector<std::string> quantity_names)
{
    std::vector<std::string> defined_quantities;
    std::vector<std::string> duplicated_quantities;
    for (std::string name : quantity_names) {
        insert_quantity_name_if_new(name, defined_quantities, duplicated_quantities);
    }

    return duplicated_quantities;
}

/**
 * Finds quantities that are required by modules but not already defined
 */
std::vector<std::string> find_undefined_module_inputs(
    std::vector<std::string> quantity_names,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names)
{
    // Combine the lists of module names
    std::vector<std::string> module_names = ss_module_names;
    module_names.insert(module_names.begin(), deriv_module_names.begin(), deriv_module_names.end());

    // Check all the inputs
    std::vector<std::string> undefined_module_inputs;
    for (std::string module_name : module_names) {
        auto module = module_wrapper_factory::create(module_name);
        std::vector<std::string> input_names = module->get_inputs();
        for (std::string input : input_names) {
            insert_module_input_if_undefined(input, module_name, quantity_names, undefined_module_inputs);
        }
    }

    return undefined_module_inputs;
}

/**
 * Finds quantities output by derivative modules that are not included in the initial state
 */
std::vector<std::string> find_undefined_module_outputs(
    state_map initial_state,
    std::vector<std::string> deriv_module_names)
{
    std::vector<std::string> initial_state_names = keys(initial_state);
    std::vector<std::string> undefined_module_outputs;
    for (std::string module_name : deriv_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        std::vector<std::string> output_names = module->get_outputs();
        for (std::string output : output_names) {
            insert_module_input_if_undefined(output, module_name, initial_state_names, undefined_module_outputs);
        }
    }
    return undefined_module_outputs;
}

/**
 * Forms a user feedback message from a list of duplicated quantities
 */
std::string create_message_from_duplicated_quantities(std::vector<std::string> duplicated_quantities)
{
    std::string message;

    if (duplicated_quantities.size() == 0) {
        message = std::string("No quantities were defined multiple times in the inputs\n");
    } else {
        message = std::string("The following quantities were defined more than once in the inputs:\n");
        for (std::string name : duplicated_quantities) {
            message += std::string(" ") + name + std::string("\n");
        }
    }

    message += std::string("\n");

    return message;
}

/**
 * Forms a user feedback message from a list of undefined module inputs
 */
std::string create_message_from_undefined_module_inputs(std::vector<std::string> undefined_module_inputs)
{
    std::string message;

    if (undefined_module_inputs.size() == 0) {
        message = std::string("All module inputs were properly defined\n");
    } else {
        message = std::string("The following module inputs were not defined:\n");
        for (std::string name : undefined_module_inputs) {
            message += std::string(" ") + name + std::string("\n");
        }
    }

    message += std::string("\n");

    return message;
}

/**
 * Forms a user feedback message from a list of undefined module outputs
 */
std::string create_message_from_undefined_module_outputs(std::vector<std::string> undefined_module_outputs)
{
    std::string message;

    if (undefined_module_outputs.size() == 0) {
        message = std::string("All derivative module outputs were included in the initial state\n");
    } else {
        message = std::string("The following derivative module outputs were not part of the initial state:\n");
        for (std::string name : undefined_module_outputs) {
            message += std::string(" ") + name + std::string("\n");
        }
    }

    message += std::string("\n");

    return message;
}

/**
 * Attempts to insert quantity_name into target_vector.
 * In the case of a duplicate, quantity_name is inserted into duplicate_vector.
 */
void insert_quantity_name_if_new(
    std::string quantity_name,
    std::vector<std::string>& target_vector,
    std::vector<std::string>& duplicate_vector)
{
    if (std::find(target_vector.begin(), target_vector.end(), quantity_name) == target_vector.end()) {
        target_vector.push_back(quantity_name);
    } else {
        duplicate_vector.push_back(quantity_name);
    }
}

/**
 * If input_name is not included in defined_quantity_names,
 * a new entry is inserted into undefined_module_inputs
 * that includes input_name and its associated module_name
 */
void insert_module_input_if_undefined(
    std::string input_name,
    std::string module_name,
    std::vector<std::string> defined_quantity_names,
    std::vector<std::string>& undefined_module_inputs)
{
    if (std::find(defined_quantity_names.begin(), defined_quantity_names.end(), input_name) == defined_quantity_names.end()) {
        undefined_module_inputs.push_back(input_name + std::string(" from the '") + module_name + std::string("' module"));
    }
}