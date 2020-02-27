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
    string_vector ss_module_names,
    string_vector deriv_module_names)
{
    size_t num_problems = 0;

    string_vector quantity_names = define_quantity_names(initial_state, invariant_params, varying_params, ss_module_names);

    // Criterion 1
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_multiple_quantity_definitions(quantity_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("No quantities were defined multiple times in the inputs"),
                                                           std::string("The following quantities were defined more than once in the inputs:"),
                                                           std::string(""),
                                                           string_list); });
    // Criterion 2
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_undefined_module_inputs(quantity_names, ss_module_names, deriv_module_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All module inputs were properly defined"),
                                                           std::string("The following module inputs were not defined:"),
                                                           std::string(""),
                                                           string_list); });
    // Criterion 3
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_undefined_module_outputs(initial_state, deriv_module_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All derivative module outputs were included in the initial state"),
                                                           std::string("The following derivative module outputs were not part of the initial state:"),
                                                           std::string(""),
                                                           string_list); });
    // Criterion 4
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_misordered_modules(initial_state, invariant_params, varying_params, ss_module_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All steady state modules are ordered in a consistent way"),
                                                           std::string("The following steady state modules are out of order, i.e., they require input variables whose values have not yet been calculated:"),
                                                           std::string(""),
                                                           string_list); });

    return num_problems == 0;
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
    string_vector ss_module_names,
    string_vector deriv_module_names)
{
    std::string message;

    string_set all_module_inputs = find_all_module_inputs(ss_module_names, deriv_module_names);

    // List the quantities used as inputs to the modules
    process_criterion<string_set>(
        message,
        [=]() -> string_set { return all_module_inputs; },
        [](string_set string_list) -> std::string { return create_message(
                                                        std::string("No quantities were required as inputs to any of the modules"),
                                                        std::string("The following quantities were each required by at least one module:"),
                                                        std::string(""),
                                                        string_list); });

    // List any unused quantities in the invariant parameters
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_unused_parameters(all_module_inputs, invariant_params); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("Each invariant parameter was used as an input to one or more modules"),
                                                           std::string("The following invariant parameters were not used as inputs to any module:"),
                                                           std::string("You may want to consider removing them for clarity"),
                                                           string_list); });

    // List any quantities in the initial state that lack derivatives
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_zero_derivatives(initial_state, deriv_module_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All quantities in the initial state have associated derivatives"),
                                                           std::string("The following quantities in the initial state lack associated derivatives:"),
                                                           std::string("These quantities will not change with time, so you may want to consider moving them to the invariant parameters for clarity"),
                                                           string_list); });

    // List any quantities whose derivative is determined by more than one module
    /*
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_distributed_derivatives(deriv_module_names); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("Each derivative is determined by a single module"),
                                                           std::string("Derivatives for the following quantities are each determined by more than one module:"),
                                                           std::string(""),
                                                           string_list); });*/
    return message;
}

/**
 * Assembles the names of all quantities defined by a group of input lists and modules
 */
string_vector define_quantity_names(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    string_vector ss_module_names)
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
 * Assembles a map of all quantities defined by a group of state_maps and module outputs
 */
state_map define_quantity_map(
    std::vector<state_map> state_maps,
    std::vector<string_vector> module_name_vectors)
{
    state_map quantities;

    // Process the state maps
    for (state_map const& m : state_maps) {
        quantities.insert(m.begin(), m.end());
    }

    // Get additional quantities from the modules
    for (string_vector const& names : module_name_vectors) {
        for (std::string const& module_name : names) {
            auto w = module_wrapper_factory::create(module_name);
            string_vector module_outputs = w->get_outputs();
            for (auto const& o : module_outputs) {
                quantities[o] = 0;
            }
        }
    }

    return quantities;
}

/**
 * Finds quantities that are defined multiple times
 */
string_vector find_multiple_quantity_definitions(string_vector quantity_names)
{
    string_vector defined_quantities;
    string_vector duplicated_quantities;
    for (std::string name : quantity_names) {
        insert_quantity_name_if_new(name, defined_quantities, duplicated_quantities);
    }

    std::sort(duplicated_quantities.begin(), duplicated_quantities.end());

    return duplicated_quantities;
}

/**
 * Finds quantities that are required by modules but are not defined
 */
string_vector find_undefined_module_inputs(
    string_vector quantity_names,
    string_vector ss_module_names,
    string_vector deriv_module_names)
{
    // Combine the lists of module names
    string_vector module_names = ss_module_names;
    module_names.insert(module_names.begin(), deriv_module_names.begin(), deriv_module_names.end());

    // Check all the inputs
    string_vector undefined_module_inputs;
    for (std::string module_name : module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector input_names = module->get_inputs();
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
string_vector find_undefined_module_outputs(
    state_map initial_state,
    string_vector deriv_module_names)
{
    string_vector initial_state_names = keys(initial_state);

    string_vector undefined_module_outputs;
    for (std::string module_name : deriv_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector output_names = module->get_outputs();
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
string_vector find_misordered_modules(
    state_map initial_state,
    state_map invariant_params,
    state_vector_map varying_params,
    string_vector ss_module_names)
{
    // Get quantity names from the input lists
    string_vector defined_quantity_names;
    insert_key_names(defined_quantity_names, initial_state);
    insert_key_names(defined_quantity_names, invariant_params);
    insert_key_names(defined_quantity_names, varying_params);

    // Check the steady state module order
    string_vector misordered_modules;
    for (std::string module_name : ss_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector input_names = module->get_inputs();
        if (!all_are_in_list(input_names, defined_quantity_names)) {
            misordered_modules.push_back(module_name);
        }
        string_vector output_names = module->get_outputs();
        defined_quantity_names.insert(defined_quantity_names.begin(), output_names.begin(), output_names.end());
    }

    return misordered_modules;
}

/**
 * Returns a set containing all inputs to the modules
 */
string_set find_all_module_inputs(
    string_vector ss_module_names,
    string_vector deriv_module_names)
{
    // Combine the lists of module names
    string_vector module_names = ss_module_names;
    module_names.insert(module_names.begin(), deriv_module_names.begin(), deriv_module_names.end());

    // Assemble all the inputs
    string_set module_inputs;
    for (std::string module_name : module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector input_names = module->get_inputs();
        for (std::string input : input_names) {
            module_inputs.insert(input);
        }
    }

    return module_inputs;
}

/**
 * Returns a set containing invariant parameters that are not used as inputs
 */
string_vector find_unused_parameters(
    string_set all_module_inputs,
    state_map invariant_params)
{
    string_vector invariant_param_names = keys(invariant_params);
    string_vector unused_invariant_params;
    for (std::string name : invariant_param_names) {
        insert_quantity_if_undefined(name, all_module_inputs, unused_invariant_params);
    }

    std::sort(unused_invariant_params.begin(), unused_invariant_params.end());

    return unused_invariant_params;
}

/**
 * Returns all derivative module outputs
 */
string_set find_all_deriv_outputs(
    string_vector deriv_module_names)
{
    string_set module_outputs;
    for (std::string module_name : deriv_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector output_names = module->get_outputs();
        for (std::string output : output_names) {
            module_outputs.insert(output);
        }
    }

    return module_outputs;
}

/**
 * Returns a list of quantities in the initial state that lack derivatives
 */
string_vector find_zero_derivatives(
    state_map initial_state,
    string_vector deriv_module_names)
{
    string_vector zero_derivatives;

    string_set all_derivative_outputs = find_all_deriv_outputs(deriv_module_names);

    string_vector istate_names = keys(initial_state);
    for (std::string name : istate_names) {
        insert_quantity_if_undefined(name, all_derivative_outputs, zero_derivatives);
    }

    return zero_derivatives;
}

/**
 * Returns a list of quantities that have derivatives calculated by more than one module
 */
string_vector find_distributed_derivatives(
    string_vector deriv_module_names)
{
    string_vector unique_derivatives;
    string_vector distributed_derivatives;

    for (std::string module_name : deriv_module_names) {
        auto module = module_wrapper_factory::create(module_name);
        string_vector output_names = module->get_outputs();
        for (std::string output : output_names) {
            insert_quantity_if_undefined(output, unique_derivatives, distributed_derivatives);
        }
    }

    return distributed_derivatives;
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
    string_vector& target_vector,
    string_vector& duplicate_vector)
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
    string_vector defined_quantity_names,
    string_vector& undefined_module_inputs)
{
    if (std::find(defined_quantity_names.begin(), defined_quantity_names.end(), param_name) == defined_quantity_names.end()) {
        undefined_module_inputs.push_back(param_name + std::string(" from the '") + module_name + std::string("' module"));
    }
}