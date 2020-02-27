#include <algorithm>
#include <set>
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
  * 4) All steady state module inputs are calculated before they are accessed
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

    std::vector<std::string> quantity_names = define_quantity_names(initial_state, invariant_params, varying_params, ss_module_names);

    // Criterion 1
    std::vector<std::string> duplicated_quantities = find_multiple_quantity_definitions(quantity_names);

    message += create_message(
        std::string("No quantities were defined multiple times in the inputs"),
        std::string("The following quantities were defined more than once in the inputs:"),
        std::string(""),
        duplicated_quantities);

    if (duplicated_quantities.size() > 0) {
        valid = false;
    }

    // Criterion 2
    std::vector<std::string> undefined_module_inputs = find_undefined_module_inputs(quantity_names, ss_module_names, deriv_module_names);

    message += create_message(
        std::string("All module inputs were properly defined"),
        std::string("The following module inputs were not defined:"),
        std::string(""),
        undefined_module_inputs);

    if (undefined_module_inputs.size() > 0) {
        valid = false;
    }

    // Criterion 3
    std::vector<std::string> undefined_module_outputs = find_undefined_module_outputs(initial_state, deriv_module_names);

    message += create_message(
        std::string("All derivative module outputs were included in the initial state"),
        std::string("The following derivative module outputs were not part of the initial state:"),
        std::string(""),
        undefined_module_outputs);

    if (undefined_module_outputs.size() > 0) {
        valid = false;
    }

    // Criterion 4
    std::vector<std::string> misordered_modules = find_misordered_modules(initial_state, invariant_params, varying_params, ss_module_names);

    message += create_message(
        std::string("All modules are ordered in a consistent way"),
        std::string("The following modules are out of order, i.e., they require input variables whose values have not yet been calculated:"),
        std::string(""),
        misordered_modules);

    if (misordered_modules.size() > 0) {
        valid = false;
    }

    return valid;
}

/**
 * @brief Provides information about a set of system inputs that is not strictly required to check validity
 * 
 * The following information is reported:
 * 1) A list of all quantities required by the modules as inputs
 * 2) A list of unused quantities in the invariant parameter list
 * 3) A list of quantities in the initial state that lack derivatives
 * 4) A list of quantities whose derivatives have terms calculated by multiple modules
 */
std::string analyze_system_inputs(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names)
{
    std::string message;

    // Get a list of all quantities used as inputs to modules and add them to the message
    std::set<std::string> all_module_inputs = find_all_module_inputs(ss_module_names, deriv_module_names);

    message += create_message(
        std::string("No quantities were required as inputs to any of the modules"),
        std::string("The following quantities were each required by at least one module:"),
        std::string(""),
        all_module_inputs);

    // Get a list of all the unused quantities in the invariant parameters and add them to the message
    std::vector<std::string> unused_invariant_params = find_unused_parameters(all_module_inputs, invariant_params);

    message += create_message(
        std::string("Each invariant parameter was used as an input to one or more modules"),
        std::string("The following invariant parameters were not used as inputs to any module:"),
        std::string("You may want to consider removing them for clarity"),
        unused_invariant_params);

    /*
    // Get a list of all derivative module outputs
    state_map empty_state_map;
    std::vector<std::string> all_derivative_outputs = find_undefined_module_outputs(empty_state_map, deriv_module_names);

    // Get a list of all variables in the initial state that lack associated derivatives and add them to the message
    std::vector<std::string> initial_state_names = keys(initial_state);
    std::vector<std::string> no_derivative_defined;
    for (std::string name : initial_state_names) {
        insert_quantity_if_undefined(name, all_derivative_outputs, no_derivative_defined);
    }
    if (no_derivative_defined.size() == 0) {
        message += std::string("\nAll quantities in the initial state have associated derivatives\n");
    } else {
        message += std::string("\nThe following quantities in the initial state lack associated derivatives:\n");
        for (std::string name : no_derivative_defined) {
            message += std::string(" ") + name + std::string("\n");
        }
        message += std::string("These quantities will not change with time,\n");
        message += std::string("so you may want to consider moving them to the invariant parameters for clarity or provide derivatives\n");
    }

    // Get a list of all derivatives that are calculated by more than one module and add them to the message
    */

    return message;
}

/**
 * Assembles the names of all quantities defined by a group of input lists and modules
 */
std::vector<std::string> define_quantity_names(
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
        std::vector<std::string> output_names = module->get_outputs();
        defined_quantity_names.insert(defined_quantity_names.begin(), output_names.begin(), output_names.end());
    }

    return defined_quantity_names;
}

/**
 * Assembles a map of all quantities defined by a group of input lists and modules
 */
state_map define_quantity_map(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names)
{
    state_map quantities;

    std::vector<state_map> quantities_vector{initial_state, invariant_params, at(varying_params, 0)};

    for (auto const& v : quantities_vector) {
        quantities.insert(v.begin(), v.end());
    }

    for (auto const& m : ss_module_names) {
        auto w = module_wrapper_factory::create(m);
        std::vector<std::string> names = w->get_outputs();
        for (auto const& n : names) {
            quantities[n] = 0;
        }
    }

    return quantities;
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

    std::sort(duplicated_quantities.begin(), duplicated_quantities.end());

    return duplicated_quantities;
}

/**
 * Finds quantities that are required by modules but are not defined
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
            insert_module_param_if_undefined(input, module_name, quantity_names, undefined_module_inputs);
        }
    }

    std::sort(undefined_module_inputs.begin(), undefined_module_inputs.end());

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
            insert_module_param_if_undefined(output, module_name, initial_state_names, undefined_module_outputs);
        }
    }

    std::sort(undefined_module_outputs.begin(), undefined_module_outputs.end());

    return undefined_module_outputs;
}

/**
 * Finds modules that access variables before their values have been calculated
 */
std::vector<std::string> find_misordered_modules(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    std::vector<std::string> ss_module_names)
{
    // Get quantity names from the input lists
    std::vector<std::string> defined_quantity_names;
    insert_key_names(defined_quantity_names, initial_state);
    insert_key_names(defined_quantity_names, invariant_params);
    insert_key_names(defined_quantity_names, varying_params);

    // Check the steady state module order
    std::vector<std::string> misordered_modules;
    for (std::string module_name : ss_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        std::vector<std::string> input_names = module->get_inputs();
        if (!all_are_in_list(input_names, defined_quantity_names)) {
            misordered_modules.push_back(module_name);
        }
        std::vector<std::string> output_names = module->get_outputs();
        defined_quantity_names.insert(defined_quantity_names.begin(), output_names.begin(), output_names.end());
    }

    return misordered_modules;
}

/**
 * Returns a set containing all inputs to the modules
 */
std::set<std::string> find_all_module_inputs(
    std::vector<std::string> ss_module_names,
    std::vector<std::string> deriv_module_names)
{
    // Combine the lists of module names
    std::vector<std::string> module_names = ss_module_names;
    module_names.insert(module_names.begin(), deriv_module_names.begin(), deriv_module_names.end());

    // Assemble all the inputs
    std::set<std::string> module_inputs;
    for (std::string module_name : module_names) {
        auto module = module_wrapper_factory::create(module_name);
        std::vector<std::string> input_names = module->get_inputs();
        for (std::string input : input_names) {
            module_inputs.insert(input);
        }
    }

    return module_inputs;
}

/**
 * Returns a set containing invariant parameters that are not used as inputs
 */
std::vector<std::string> find_unused_parameters(
    std::set<std::string> all_module_inputs,
    state_map invariant_params)
{
    std::vector<std::string> invariant_param_names = keys(invariant_params);
    std::vector<std::string> unused_invariant_params;
    for (std::string name : invariant_param_names) {
        insert_quantity_if_undefined(name, all_module_inputs, unused_invariant_params);
    }

    std::sort(unused_invariant_params.begin(), unused_invariant_params.end());

    return unused_invariant_params;
}

/**
 * Adds an indented line to a message
 */
void add_indented_line(std::string& message, std::string text_to_add, int num_spaces)
{
    for (int i = 0; i < num_spaces; i++) {
        message += std::string(" ");
    }
    message += text_to_add + std::string("\n");
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
void insert_module_param_if_undefined(
    std::string param_name,
    std::string module_name,
    std::vector<std::string> defined_quantity_names,
    std::vector<std::string>& undefined_module_inputs)
{
    if (std::find(defined_quantity_names.begin(), defined_quantity_names.end(), param_name) == defined_quantity_names.end()) {
        undefined_module_inputs.push_back(param_name + std::string(" from the '") + module_name + std::string("' module"));
    }
}