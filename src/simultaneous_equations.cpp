#include <algorithm>  // for std:sort
#include "simultaneous_equations.h"
#include "system_helper_functions.h"

/**
  * @brief Checks over a group of quantities and modules to ensure they can be
  *        used to create a valid set of simultaneous equations.
  *
  * @param[in,out] message Validation feedback is added to this string.
  * @return true if the inputs are valid, false otherwise.
  * 
  * When discussing a simultaneous_equations object, it is helpful to define several terms
  * as follows:
  * 
  * We consider a quantity to have been "specified" (or "defined") if it is a
  * key in `known_quantities` or if it is an output variable of one of the steady-state
  * modules listed in `ss_module_names`.
  * 
  * We consider a quantity to be strictly required as a module input if (i) a module
  * requires it as an input and (ii) it has not been produced as the output of a previous
  * module. Thus the set of strictly required inputs depends on the order of the modules.
  * 
  * If a quantity is (i) strictly required as an input and (ii) is produced as the output
  * of a module, it is said to be an "unknown quantity" since its value will be determined
  * by the set of simultaneous equations that this class represents. Thus the set of unknown
  * quantities also depends on the order of the modules.
  * 
  * While constructing a simultaneous_equations object, the following criteria determine the
  * validity of the inputs:
  * - 1. Each quantity is specified only once.
  * - 2. All module inputs are specified.
  * - 3. Each quantity name in `unknown_quantities` is an unknown quantity.
  * - 4. Each unknown quantity is included in `unknown_quantities`.
 */
bool validate_simultaneous_equations_inputs(
    std::string& message,
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& ss_module_names)
{
    size_t num_problems = 0;

    string_vector quantity_names = get_defined_quantity_names(
        std::vector<state_map>{known_quantities},
        std::vector<string_vector>{ss_module_names});

    // Criterion 1
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return find_duplicate_quantity_definitions(quantity_names); },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("No quantities were defined multiple times"),
                std::string("The following quantities were defined more than once:"),
                std::string(""),
                string_list);
        });

    // Criterion 2
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            return find_undefined_module_inputs(
                quantity_names,
                std::vector<string_vector>{ss_module_names});
        },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All module inputs were properly defined"),
                std::string("The following module inputs were not defined:"),
                std::string(""),
                string_list);
        });

    string_vector unknown_quantities_from_modules = get_unknown_quantities(std::vector<string_vector>{ss_module_names});

    // Criterion 3
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return string_vector_difference(unknown_quantities, unknown_quantities_from_modules); },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All quantities in 'unknown_quantities' are truly unknown quantities"),
                std::string("The following quantities in 'unknown_quantities' are not actually unknown quantities:"),
                std::string(""),
                string_list);
        });

    // Criterion 4
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return string_vector_difference(unknown_quantities_from_modules, unknown_quantities); },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All unknown quantities are included in 'unknown_quantities'"),
                std::string("The following unknown quantities are not included in 'unknown_quantities':"),
                std::string(""),
                string_list);
        });

    return num_problems == 0;
}

/**
 * @brief Provides information about a set of simultaneous_equations inputs that is not
 *        strictly required to check validity.
 * 
 * The following information is reported:
 * 1. A list of all quantities required by the modules as inputs
 * 2. A list of all quantities produced by the modules as outputs
 * 3. A list of all unknown quantities specified by the modules
 */
std::string analyze_simultaneous_equations_inputs(string_vector const& ss_module_names)
{
    std::string message;

    // List the required inputs
    process_criterion<string_set>(
        message,
        [=]() -> string_set { return find_strictly_required_inputs(std::vector<string_vector>{ss_module_names}); },
        [](string_set string_list) -> std::string { return create_message(
                                                        std::string("No quantities were required by the set of modules"),
                                                        std::string("The following quantities were required by the set of modules:"),
                                                        std::string(""),
                                                        string_list); });

    // List the outputs
    process_criterion<string_set>(
        message,
        [=]() -> string_set { return find_unique_module_outputs(std::vector<string_vector>{ss_module_names}); },
        [](string_set string_list) -> std::string { return create_message(
                                                        std::string("No quantities were produced by any of the modules"),
                                                        std::string("The following quantities were each produced by at least one module:"),
                                                        std::string(""),
                                                        string_list); });

    // List the unknown quantities
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return get_unknown_quantities(std::vector<string_vector>{ss_module_names}); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("The modules did not specify any unknown quantities"),
                                                           std::string("The modules specified the following unknown quantities:"),
                                                           std::string(""),
                                                           string_list); });

    return message;
}

simultaneous_equations::simultaneous_equations(
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& ss_module_names)
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid set of simultaneous equations
    bool valid = validate_simultaneous_equations_inputs(startup_message, known_quantities, unknown_quantities, ss_module_names);
    if (!valid) {
        throw std::logic_error("Thrown by simultaneous_equations::simultaneous_equations: the supplied inputs cannot form a valid set of simultaneous equations.\n\n" + startup_message);
    }

    // Make the central list of quantities and the module output map
    quantities = define_quantity_map(
        std::vector<state_map>{known_quantities},
        std::vector<string_vector>{ss_module_names});
    module_output_map = quantities;

    // Instantiate the modules
    steady_state_modules = get_module_vector(std::vector<string_vector>{ss_module_names}, &quantities, &module_output_map);

    // Make sorted lists of subsets of quantity names
    // Note that the output of string_set_to_string_vector is automatically
    //  sorted, because a string_set is always sorted by definition.
    string_vector steady_state_output_names = string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{ss_module_names}));
    string_vector known_quantity_names = keys(known_quantities);
    std::sort(known_quantity_names.begin(), known_quantity_names.end());

    // Get vectors of pointers to important subsets of the quantities.
    // These pointers allow the user to update the values of the known
    //  quantities and to retrieve the values of all module outputs.
    // Here it is important to use sorted vectors of names so the 
    //  quantities will have a definite order that the user can safely
    //  assume.
    known_ptrs = get_pointers(known_quantity_names, quantities);
    output_ptrs = get_const_pointers(steady_state_output_names, quantities);

    // Get vectors of pointers to important subsets of the quantities.
    // These pointers allow us to efficiently alter portions of the
    //  quantity maps before running the modules.
    unknown_ptrs = get_pointers(unknown_quantities, quantities);
    steady_state_ptrs = get_pointers(steady_state_output_names, module_output_map);

    // Get pairs of pointers to important subsets of the quantities.
    // These pairs allow us to efficiently retrieve the output of each
    //  module and store it in the main quantity map
    unknown_ptr_pairs = get_pointer_pairs(unknown_quantities, quantities, module_output_map);
    steady_state_ptr_pairs = get_pointer_pairs(steady_state_output_names, quantities, module_output_map);
}

/**
 * @brief Updates the values of the known quantities using the values pointed to by the elements
 * of ptrs_to_values. The elements of `ptr_to_values` must be sorted according to quantity name.
 */
void simultaneous_equations::update_known_quantities(std::vector<const double*> const& ptrs_to_values)
{
    if (ptrs_to_values.size() != known_ptrs.size()) {
        throw std::logic_error("Thrown by simultaneous_equations::update_known_quantities: ptrs_to_values has the wrong size.");
    }

    for (size_t i = 0; i < known_ptrs.size(); ++i) {
        *known_ptrs[i] = *ptrs_to_values[i];
    }
}

/**
 * @brief Finds the unknown quantities defined by the set of modules.
 * 
 * @return A sorted list of quantity names
 */
string_vector get_unknown_quantities(std::vector<string_vector> module_name_vector)
{
    string_set required_inputs = find_strictly_required_inputs(module_name_vector);

    string_set module_outputs = find_unique_module_outputs(module_name_vector);

    string_set unknown_quantities;
    for (std::string const& input : required_inputs) {
        if (module_outputs.find(input) != module_outputs.end()) {
            unknown_quantities.insert(input);
        }
    }

    return string_set_to_string_vector(unknown_quantities);
}
