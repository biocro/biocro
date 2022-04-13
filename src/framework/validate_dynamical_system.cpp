#include <algorithm>                            // For std::find
#include <memory>                               // For std::unique_ptr
#include "state_map.h"                          // For state_map, string_vector, string_set
#include "module.h"                            // For module_vector
#include "utils/module_dependency_utilities.h"  // for has_cyclic_dependency
#include "validate_dynamical_system.h"

/**
  * @brief Checks over a group of quantities and modules to ensure they can be
  *        used to create a valid system.
  *
  * @param[in,out] message Validation feedback is added to this string.
  * @return `true` if the inputs are valid, `false` otherwise.
  *
  * The following criteria are used to determine validity:
  * 1. Each quantity is specified only once.
  * 2. All module inputs are specified.
  * 3. Derivatives are calculated only for quantities in the initial values.
  * 4. Direct modules can be ordered in such a way that inputs
  *    are calculated before they are accessed.
  *
  * We consider a quantity to have been "specified" (or "defined") if it is a
  * key in one of the maps `initial_values`, `params`, or
  * `drivers`, or, if it is an output variable of one of the direct
  * modules listed in `direct_module_names`.
  *
  * Criterion 2 and criterion 4 are related: Criterion 2 requires
  * merely that each input to a direct or differential module is either
  * a variable in the initial values, is one of the parameters, or is
  * an output of some direct module.  Criterion 4 goes further
  * for the case of a direct module where an input quantity is
  * provided by the output of some other direct module: It
  * requires that the direct modules can be ordered in such a
  * way that each of a module's input quantities that is neither a
  * parameter or a quantity in the initial values be provided by the
  * output of a module earlier in the list.
  *
  * Notably absent from these criteria is a requirement that a derivative be
  * calculated for every value given in the initial values.  Values in the
  * initial values that are not outputs of any differential module are assumed
  * to have a derivative of zero, that is, they are assumed to be constant.
  */
bool validate_dynamical_system_inputs(
    std::string& message,
    state_map initial_values,
    state_map params,
    state_vector_map drivers,
    mc_vector const& direct_mcs,
    mc_vector const& differential_mcs)
{
    size_t num_problems = 0;

    string_vector quantity_names = get_defined_quantity_names(
        std::vector<state_map>{initial_values, params, at(drivers, 0)},
        direct_mcs);

    // Criterion 1
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_duplicate_quantity_definitions(quantity_names); },
        [](string_vector string_list) -> std::string {
            return create_marked_message(
                std::string("No quantities were defined multiple times in the inputs"),
                std::string("The following quantities were defined more than once in the inputs:"),
                std::string(""),
                string_list);
        });

    // Criterion 2
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            return find_undefined_module_inputs(
                quantity_names,
                std::vector<mc_vector>{direct_mcs, differential_mcs});
        },
        [](string_vector string_list) -> std::string {
            return create_marked_message(
                std::string("All module inputs were properly defined"),
                std::string("The following module inputs were not defined:"),
                std::string(""),
                string_list);
        });

    // Criterion 3
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            return find_undefined_module_outputs(
                keys(initial_values),
                std::vector<mc_vector>{differential_mcs});
        },
        [](string_vector string_list) -> std::string {
            return create_marked_message(
                std::string("All differential module outputs were included in the initial values"),
                std::string("The following differential module outputs were not part of the initial values:"),
                std::string(""),
                string_list);
        });

    // Criterion 4
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            string_vector result{};
            if (has_cyclic_dependency(direct_mcs)) {
                // For now, we just want a non-zero vector size.  It
                // may, however, prove useful to display a set of
                // modules that comprise a cyclic dependency.
                result.push_back("");
            }
            return result;
        },
        [](string_vector string_list) -> std::string {
            return create_marked_message(
                std::string("There are no cyclic dependencies among the direct modules."),
                std::string("The direct modules have a cyclic dependency."),
                std::string(""),
                string_list);
        });

    return num_problems == 0;
}

/**
 * @brief Provides information about a set of system inputs that is not strictly
 *        required to check validity.
 *
 * The following information is reported:
 * 1. A list of all quantities required by the modules as inputs
 * 2. A list of unused quantities in the invariant parameter list
 * 3. A list of unused quantities in the drivers list
 * 4. A list of quantities in the initial values that lack derivatives
 * 5. A list of quantities whose derivatives have terms calculated by multiple modules
 */
std::string analyze_system_inputs(
    state_map initial_values,
    state_map params,
    state_vector_map drivers,
    mc_vector const& direct_mcs,
    mc_vector const& differential_mcs)
{
    std::string message;

    string_set all_module_inputs = find_unique_module_inputs(std::vector<mc_vector>{direct_mcs, differential_mcs});

    // List a suitable ordering for evaluation of the direct
    // modules if the given order isn't suitable.
    if (!has_cyclic_dependency(direct_mcs)) {
        process_criterion<string_vector>(
            message,
            [=]() -> string_vector {
                if (!order_ok(direct_mcs)) {
                    return get_module_names(get_evaluation_order(direct_mcs));
                } else {
                    return {};
                }
            },
            [](string_vector string_list) -> std::string {
                return create_message(
                    std::string("The direct modules are in a suitable order for evaluation."),
                    std::string("The direct modules need to be re-ordered before evaluation.\n") +
                        "(This will be done automatically during dynamical_system construction.)\n" +
                        "Here is a suitable ordering:",
                    std::string(""),
                    string_list);
            });
    }

    // List the quantities used as inputs to the modules
    process_criterion<string_set>(
        message,
        [=]() -> string_set { return all_module_inputs; },
        [](string_set string_list) -> std::string { return create_message(
                                                        std::string("No quantities were required as inputs to any of the modules"),
                                                        std::string("The following quantities were each required by at least one module:"),
                                                        std::string(""),
                                                        string_list); });

    // List any unused quantities in the parameters
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_unused_input_parameters(std::vector<state_map>{params}, std::vector<mc_vector>{direct_mcs, differential_mcs}); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("Each parameter was used as an input to one or more modules"),
                                                           std::string("The following parameters were not used as inputs to any module:"),
                                                           std::string("You may want to consider removing them for clarity"),
                                                           string_list); });

    // List any unused quantities in the drivers
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_unused_input_parameters(std::vector<state_map>{at(drivers, 0)}, std::vector<mc_vector>{direct_mcs, differential_mcs}); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("Each driver was used as an input to one or more modules"),
                                                           std::string("The following drivers were not used as inputs to any module:"),
                                                           std::string("You may want to consider removing them for clarity"),
                                                           string_list); });

    // List any quantities in the initial values that lack derivatives
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_static_output_parameters(initial_values, differential_mcs); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All quantities in the initial values have associated derivatives"),
                                                           std::string("The following quantities in the initial values lack associated derivatives:"),
                                                           std::string("These quantities will not change with time, so you may want to consider moving them to the parameters for clarity"),
                                                           string_list); });

    string_vector differential_module_outputs = get_defined_quantity_names(
        std::vector<state_map>{},
        differential_mcs);

    // List any quantities whose derivative is determined by more than one module
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_duplicate_quantity_definitions(differential_module_outputs); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("No derivative is determined by more than one module"),
                                                           std::string("Derivatives for the following quantities are each determined by more than one module:"),
                                                           std::string(""),
                                                           string_list); });

    // List any modules that require a fixed step size Euler ode_solver
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_euler_requirements(direct_mcs); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("No direct modules require a fixed step size Euler ode_solver"),
                                                           std::string("The following direct modules require a fixed step size Euler ode_solver:"),
                                                           std::string(""),
                                                           string_list); });

    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_euler_requirements(differential_mcs); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("No differential modules require a fixed step size Euler ode_solver"),
                                                           std::string("The following differential modules require a fixed step size Euler ode_solver:"),
                                                           std::string(""),
                                                           string_list); });

    // List any mischaracterized direct modules
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_mischaracterized_modules(direct_mcs, false); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All modules in the direct module list are direct modules"),
                                                           std::string("The following modules were in the list of direct modules but are actually differential modules:"),
                                                           std::string(""),
                                                           string_list); });

    // List any mischaracterized differential modules
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_mischaracterized_modules(differential_mcs, true); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("All modules in the differential module list are differential modules"),
                                                           std::string("The following modules were in the list of differential modules but are actually direct modules:"),
                                                           std::string(""),
                                                           string_list); });
    return message;
}

/**
 * @brief Assembles a "master" state_map from a group of state_maps
 * and a list of modules.
 *
 * The resulting state_map will essentially be the union of all the
 * maps in parameter `state_maps` and an additional map formed by
 * taking as keys all of the output variables from the modules in
 * mcs and setting the corresponding values to zero.
 *
 * The set of keys in the maps in `state_maps` should not overlap with
 * one another and should be distinct from the module output variable
 * names.
 *
 * @param state_maps A list (presented as a `std::vector`) of
 *                   state_maps.
 *
 * @param mcs A list of modules (presented as a `std::vector` of pointers
 *                             to `module_creator` objects).
 *
 * @returns a `state_map` which is the union of all maps in
 *          `state_maps` together with a map whose keys are all the output
 *          parameters of modules in `mcs` and whose
 *          values are all zero.
 */
state_map define_quantity_map(
    std::vector<state_map> state_maps,
    mc_vector mcs)
{
    state_map quantities;

    // Process the state maps
    for (state_map const& m : state_maps) {
        quantities.insert(m.begin(), m.end());
    }

    // Get additional quantities from the modules
    for (auto const& w : mcs) {
        string_vector module_outputs = w->get_outputs();
        for (auto const& o : module_outputs) {
            quantities[o] = 0;
        }
    }

    return quantities;
}

/**
 * @brief Finds quantities that are defined multiple times in a vector
 * of strings
 *
 * @param quantity_names A list (presented as a vector of strings) of names of
 *                       quantities.  Generally these will be names of state
 *                       variables, environment variables ("varying
 *                       parameters"), system constants ("invariant
 *                       parameters"), or derived quantites (outputs of
 *                       direct modules).
 *
 * @returns A alphabetized list (presented as a vector of strings) of all
 *          quantities appearing more than once in the input list.
 */
string_vector find_duplicate_quantity_definitions(string_vector quantity_names)
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
 *  @brief Finds quantities that are required by modules but are not defined by
 *  quantity_names. The output includes the module associated with each
 *  undefined input.
 */
string_vector find_undefined_module_inputs(
    string_vector quantity_names,
    std::vector<mc_vector> mc_vectors)
{
    string_vector undefined_module_inputs;

    for (mc_vector const& mcv : mc_vectors) {
        for (auto const& w : mcv) {
            string_vector input_names = w->get_inputs();
            std::string module_name = w->get_name();
            for (std::string input : input_names) {
                insert_module_param_if_undefined(
                    input,
                    module_name,
                    quantity_names,
                    undefined_module_inputs);
            }
        }
    }

    return undefined_module_inputs;
}

/**
 *  @brief Finds quantities that are output by modules but are not defined by
 *  quantity_names.  The output includes the module associated with each
 *  undefined output.
 */
string_vector find_undefined_module_outputs(
    string_vector quantity_names,
    std::vector<mc_vector> mc_vectors)
{
    string_vector undefined_module_outputs;

    for (mc_vector const& mcv : mc_vectors) {
        for (auto const& w : mcv) {
            string_vector output_names = w->get_outputs();
            std::string module_name = w->get_name();
            for (std::string output : output_names) {
                insert_module_param_if_undefined(
                    output,
                    module_name,
                    quantity_names,
                    undefined_module_outputs);
            }
        }
    }

    return undefined_module_outputs;
}

/**
 * @brief Returns a set containing all unique inputs to the modules
 */
string_set find_unique_module_inputs(std::vector<mc_vector> mc_vectors)
{
    string_set module_inputs;

    for (mc_vector const& mcs : mc_vectors) {
        for (auto const& w : mcs) {
            string_vector input_names = w->get_inputs();
            module_inputs.insert(input_names.begin(), input_names.end());
        }
    }

    return module_inputs;
}

/**
 * @brief Returns a set containing all unique outputs produced by the
 * modules
 */
string_set find_unique_module_outputs(mc_vector mcs)
{
    string_set module_outputs;

    for (auto const& w : mcs) {
        string_vector output_names = w->get_outputs();
        module_outputs.insert(output_names.begin(), output_names.end());
    }

    return module_outputs;
}

/**
 * @brief Returns a vector containing all inputs to the set of modules that are not outputs
 * produced by previous modules. Note that the order of modules is important here.
 */
string_set find_strictly_required_inputs(mc_vector mcs)
{
    string_vector required_module_inputs;
    string_set outputs_from_previous_modules;

    for (auto const& w : mcs) {
        for (std::string const& input_name : w->get_inputs()) {
            insert_quantity_if_undefined(input_name, outputs_from_previous_modules, required_module_inputs);
        }
        string_vector output_names = w->get_outputs();
        outputs_from_previous_modules.insert(output_names.begin(), output_names.end());
    }

    return string_vector_to_string_set(required_module_inputs);
}

/**
 * @brief Returns parameters in the state maps that are not used as
 * inputs to the modules
 */
string_vector find_unused_input_parameters(
    std::vector<state_map> state_maps,
    std::vector<mc_vector> mc_vectors)
{
    string_vector unused_params;

    string_set all_module_inputs = find_unique_module_inputs(mc_vectors);

    // For now, there are a few names we should ignore since they may be
    // required for other reasons even if they are not used as module inputs
    string_vector ignored_names{
        "timestep",
        "time",
        "doy",
        "hour"};

    for (state_map const& m : state_maps) {
        string_vector parameters = keys(m);
        for (std::string const& name : parameters) {
            if (std::find(ignored_names.begin(), ignored_names.end(), name) == ignored_names.end()) {
                insert_quantity_if_undefined(name, all_module_inputs, unused_params);
            }
        }
    }

    std::sort(unused_params.begin(), unused_params.end());

    return unused_params;
}

/**
 * @brief Returns quantities in the state map that are not provided
 * as outputs from the modules
 */
string_vector find_static_output_parameters(
    state_map quantities,
    mc_vector mcs)
{
    string_vector unused_params;

    string_set all_module_outputs = find_unique_module_outputs(mcs);

    string_vector quantity_names = keys(quantities);
    for (std::string const& name : quantity_names) {
        insert_quantity_if_undefined(name, all_module_outputs, unused_params);
    }

    std::sort(unused_params.begin(), unused_params.end());

    return unused_params;
}

/**
 * @brief Returns modules that require a fixed step size Euler ode_solver
 */
string_vector find_euler_requirements(mc_vector mcs)
{
    // Get all the module inputs and outputs
    string_set all_module_inputs =
        find_unique_module_inputs(std::vector<mc_vector>{mcs});

    string_set all_module_outputs =
        find_unique_module_outputs(mcs);

    // Make an appropriate state_map that contains them all
    state_map quantities;
    for (string_set const& names : std::vector<string_set>{all_module_inputs, all_module_outputs}) {
        for (std::string const& n : names) {
            quantities[n] = 0;
        }
    }

    // Instantiate each module and check its characterization
    string_vector euler_requiring_modules;
    module_vector modules = get_module_vector(mcs, quantities, &quantities);
    for (size_t i = 0; i < modules.size(); ++i) {
        if (modules[i]->requires_euler_ode_solver()) {
            euler_requiring_modules.push_back(mcs[i]->get_name());
        }
    }

    return euler_requiring_modules;
}

/**
 * @brief Returns mischaracterized modules, i.e., direct modules
 * in the differential module list or vice-versa
 */
string_vector find_mischaracterized_modules(mc_vector mcs, bool is_differential)
{
    // Get all the module inputs and outputs
    string_set all_module_inputs =
        find_unique_module_inputs(std::vector<mc_vector>{mcs});

    string_set all_module_outputs =
        find_unique_module_outputs(mcs);

    // Make an appropriate state_map that contains them all
    state_map quantities;
    for (string_set const& names : std::vector<string_set>{all_module_inputs, all_module_outputs}) {
        for (std::string const& n : names) {
            quantities[n] = 0;
        }
    }

    // Instantiate each module and check its characterization
    string_vector mischaracterized_modules;
    module_vector modules = get_module_vector(mcs, quantities, &quantities);
    for (size_t i = 0; i < modules.size(); ++i) {
        if (modules[i]->is_differential() != is_differential) {
            mischaracterized_modules.push_back(mcs[i]->get_name());
        }
    }

    return mischaracterized_modules;
}

/**
 * @brief Returns a vector of unique_ptrs to module objects
 */
module_vector get_module_vector(
    mc_vector mcs,
    state_map const& input_quantities,
    state_map* output_quantities)
{
    module_vector modules;
    for (auto const& w : mcs) {
        modules.push_back(w->create_module(input_quantities, output_quantities));
    }
    return modules;
}

/**
 *  @brief Generates a vector of module names from a vector of module wrapper
 *  pointers
 */
string_vector get_module_names(mc_vector mcs)
{
    string_vector module_names;
    for (auto const& w : mcs) {
        module_names.push_back(w->get_name());
    }
    return module_names;
}

/**
 * @brief Adds an indented line to a message
 *
 * @param[in] message The message being added to.
 *
 * @param[in] text_to_add The text being appended to message.
 *
 * @param[in] num_spaces The number of spaces of indentation for the appended
 *                       text.
 *
 * @returns A string consisting of the input message followed num_spaces space
 *          characters followed by text_to_add.
 *
 */
std::string add_indented_line(std::string message, std::string text_to_add, int num_spaces)
{
    for (int i = 0; i < num_spaces; i++) {
        message += std::string(" ");
    }
    message += text_to_add + std::string("\n");

    return message;
}

/**
 * @brief Inserts quantity_name into target_vector if it is not
 * already there.  In the case of a duplicate, quantity_name is
 * inserted into duplicates.
 *
 * @param[in] quantity_name A string (the name of a quantity).
 *
 * @param[in/out] target_vector A vector of strings.  If quantity_name wasn't in
 *                              the list to begin with, it will be appended.
 *
 * @param[in/out] duplicates A vector of strings.  If quantity_name was already
 *                           in target_vector, it will be appended to
 *                           duplicates.
 */
void insert_quantity_name_if_new(
    std::string quantity_name,
    string_vector& target_vector,
    string_vector& duplicates)
{
    if (std::find(target_vector.begin(), target_vector.end(), quantity_name) == target_vector.end()) {
        target_vector.push_back(quantity_name);
    } else {
        duplicates.push_back(quantity_name);
    }
}

/**
 * @brief If param_name is not included in defined_quantity_names, a
 * new entry is inserted into undefined_module_names that includes
 * param_name and its associated module_name
 */
void insert_module_param_if_undefined(
    std::string param_name,
    std::string module_name,
    string_vector defined_quantity_names,
    string_vector& undefined_module_names)
{
    if (std::find(defined_quantity_names.begin(), defined_quantity_names.end(), param_name) == defined_quantity_names.end()) {
        undefined_module_names.push_back(param_name + std::string(" from the '") + module_name + std::string("' module"));
    }
}

/**
 * @brief Converts a string_set to a string_vector
 */
string_vector string_set_to_string_vector(string_set ss)
{
    string_vector sv;
    for (std::string const& s : ss) {
        sv.push_back(s);
    }
    return sv;
}

/**
 * @brief Creates a string_set from all the elements of a
 * string_vector.
 */
string_set string_vector_to_string_set(string_vector sv)
{
    string_set ss;
    for (std::string const& s : sv) {
        ss.insert(s);
    }
    return ss;
}

string_vector string_vector_difference(
    string_vector find_elements_of_this_vector,
    string_vector that_are_not_in_this_one)
{
    string_vector difference;
    for (std::string const& s : find_elements_of_this_vector) {
        insert_quantity_if_undefined(s, that_are_not_in_this_one, difference);
    }
    return difference;
}
