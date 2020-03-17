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
  * We consider a quantity to be strictly required as a module input if a module requires
  * it as an input and it has not been produced as the output of a previous module. Thus
  * the set of strictly required inputs depends on the order of the modules.
  * 
  * If a quantity is strictly required as an input and is also produced as the output of
  * a module, it is said to be an "unknown quantity" since its value will be determined
  * by the set of simultaneous equations represented by this class object. Thus the set
  * of unknown quantities also depends on the order of the modules. For these quantities,
  * we can refer to an "assumed" value (the input value) and a "calculated" value (the
  * output value).
  * 
  * Suppose the calculated value of unknown_quantity_1 is used to calculate the value of
  * unknown_quantity_2, i.e., the calculated value of unknown_quantity_1 is an input to
  * a module that produces unknown_quantity_2 as an output. When solving the set of
  * equations, unknown_quantity_1 will be automatically determined when the value of
  * unknown_quantity_2 is determined. In this case we refer to unknown_quantity_2 as an
  * "independent unknown quantity" or "independent quantity" and unknown_quantity_1 as
  * a "dependent unknown quantity." This distinction will also depend on the order of
  * the modules.
  * 
  * While constructing a simultaneous_equations object, the following criteria determine the
  * validity of the inputs:
  * - 1. Each quantity is specified only once.
  * - 2. All module inputs are specified.
  * - 3. Each quantity name in `unknown_quantities` is an unknown quantity.
  * - 4. Each unknown quantity is included in `unknown_quantities`.
  * - 5. Each quantity in `independent_quantities` is an independent quantity.
  * - 6. Each independent quantity is included in `independent_quantities`.
 */
bool validate_simultanous_equations_inputs(
    std::string& message,
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& independent_quantities,
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

    string_vector independent_quantities_from_modules = get_independent_quantities(std::vector<string_vector>{ss_module_names});

    // Criterion 5
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return string_vector_difference(independent_quantities, independent_quantities_from_modules); },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All quantities in 'independent_quantities' are truly independent quantities"),
                std::string("The following quantities in 'independent_quantities' are not actually independent quantities:"),
                std::string(""),
                string_list);
        });

    // Criterion 6
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return string_vector_difference(independent_quantities_from_modules, independent_quantities); },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All independent quantities are included in 'independent_quantities'"),
                std::string("The following independent quantities are not included in 'independent_quantities':"),
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
 * 4. A list of all independent quantities specified by the modules
 */
std::string analyze_simultanous_equations_inputs(
    state_map const& known_quantities,
    string_vector const& ss_module_names)
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

    // List the independent quantities
    process_criterion<string_vector>(
        message,
        [=]() -> string_vector { return get_independent_quantities(std::vector<string_vector>{ss_module_names}); },
        [](string_vector string_list) -> std::string { return create_message(
                                                           std::string("The modules did not specify any independent quantities"),
                                                           std::string("The modules specified the following independent quantities:"),
                                                           std::string(""),
                                                           string_list); });

    return message;
}

simultaneous_equations::simultaneous_equations(
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& independent_quantities,
    string_vector const& ss_module_names)
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid set of simultaneous equations
    bool valid = validate_simultanous_equations_inputs(startup_message, known_quantities, unknown_quantities, independent_quantities, ss_module_names);
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
    
    // Make lists of subsets of quantity names
    string_vector steady_state_output_names = string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{ss_module_names}));

    // Get vectors of pointers to important subsets of the quantities
    // These pointers allow us to efficiently alter portions of the
    //  quantity maps before running the modules
    unknown_ptrs = get_pointers(unknown_quantities, quantities);
    independent_ptrs = get_pointers(independent_quantities, module_output_map);
    steady_state_ptrs = get_pointers(steady_state_output_names, module_output_map);

    // Get pairs of pointers to important subsets of the quantities
    // These pairs allow us to efficiently retrieve the output of each
    //  module and store it in the main quantity map
    unknown_ptr_pairs = get_pointer_pairs(unknown_quantities, quantities, module_output_map);
    independent_ptr_pairs = get_pointer_pairs(independent_quantities, quantities, module_output_map);
    steady_state_ptr_pairs = get_pointer_pairs(steady_state_output_names, quantities, module_output_map);
}

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

string_vector get_independent_quantities(std::vector<string_vector> module_name_vector)
{
    string_vector unknown_quantities_from_modules = get_unknown_quantities(module_name_vector);

    string_set independent_quantities;
    for (std::string const& uq : unknown_quantities_from_modules) {
        bool calculated = false;
        bool used = false;

        for (string_vector const& names : module_name_vector) {
            for (std::string const& module_name : names) {
                auto w = module_wrapper_factory::create(module_name);
                string_vector module_outputs = w->get_outputs();
                string_vector module_inputs = w->get_inputs();
                if (calculated == false && std::find(module_outputs.begin(), module_outputs.end(), uq) != module_outputs.end()) {
                    calculated = true;
                } else if (calculated == true && used == false && std::find(module_inputs.begin(), module_inputs.end(), uq) != module_inputs.end()) {
                    used = true;
                }
            }
        }

        if (used == false) {
            independent_quantities.insert(uq);
        }
    }

    return string_set_to_string_vector(independent_quantities);
}