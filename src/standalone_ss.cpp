#include "standalone_ss.h"
#include "validate_system.h"
#include "system_helper_functions.h"
#include "state_map.h"

/**
  * @brief Checks over a group of quantities and modules to ensure they can be
  *        used to create a valid system.
  *
  * @param[in,out] message Validation feedback is added to this string.
  * @return true if the inputs are valid, false otherwise.
  * 
  * While constructing a Standalone_SS object, the following criteria determine the
  * validity of the inputs:
  * - 1. Each quantity is specified only once.
  * - 2. All module inputs are specified.
  * - 3. All of the quantities in the output pointers are module outputs.
  * - 4. All module inputs are calculated before they are accessed.
  * 
  * We consider a quantity to have been "specified" (or "defined") if it is a
  * key in `input_ptrs` or if it is an output variable of one of the steady-state
  * modules listed in `ss_module_names`.
  * 
  * We consider a quantity to have been "calculated" if its value is tied to an external
  * object via a pointer in `input_ptrs` or if it has been produced as the output of a
  * steady-state module.
 */
bool validate_standalone_ss_inputs(
    std::string& message,
    string_vector const& ss_module_names,
    std::unordered_map<std::string, const double*> const& input_ptrs,
    std::unordered_map<std::string, double*> const& output_ptrs)
{
    size_t num_problems = 0;

    string_vector quantity_names = get_defined_quantity_names(
        std::vector<std::unordered_map<std::string, const double*>>{input_ptrs},
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

    // Criterion 3
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            return find_static_output_parameters(
                std::vector<state_map>{state_map_from_names(keys(output_ptrs))},
                std::vector<string_vector>{ss_module_names});
        },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All quantities in the output pointers are produced as outputs by modules"),
                std::string("The following quantities in the output pointers are not produced as outputs by modules:"),
                std::string(""),
                string_list);
        });

    // Criterion 4
    num_problems += process_criterion<string_vector>(
        message,
        [=]() -> string_vector {
            return find_misordered_modules(
                std::vector<state_map>{state_map_from_names(keys(input_ptrs))},
                std::vector<string_vector>{ss_module_names});
        },
        [](string_vector string_list) -> std::string {
            return create_message(
                std::string("All steady-state modules are ordered in a consistent way"),
                std::string("The following steady-state modules are out of order, i.e., they require input variables whose values have not yet been calculated:"),
                std::string(""),
                string_list);
        });

    return num_problems == 0;
}

Standalone_SS::Standalone_SS(
    string_vector const& ss_module_names,
    std::unordered_map<std::string, const double*> const& input_ptrs,
    std::unordered_map<std::string, double*> const& output_ptrs) : steady_state_module_names(ss_module_names)
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid Standalone_SS
    bool valid = validate_standalone_ss_inputs(startup_message, ss_module_names, input_ptrs, output_ptrs);
    if (!valid) {
        throw std::logic_error("Thrown by Standalone_SS::Standalone_SS: the supplied inputs cannot form a valid Standalone_SS.\n\n" + startup_message);
    }

    // Make the central list of quantities and the module output map
    quantities = define_quantity_map(
        std::vector<state_map>{state_map_from_names(keys(input_ptrs))},
        std::vector<string_vector>{ss_module_names});
    module_output_map = quantities;

    // Instantiate the modules
    steady_state_modules = get_module_vector(std::vector<string_vector>{ss_module_names}, &quantities, &module_output_map);

    // Make lists of subsets of quantity names
    string_set module_outputs = find_unique_module_outputs(std::vector<string_vector>{ss_module_names});

    // Get vectors of pointers to important subsets of the quantities
    // These pointers allow us to efficiently reset portions of the
    //  module output map before running the modules
    module_output_ptrs = get_pointers(module_outputs, quantities);

    // Get pairs of pointers to important subsets of the quantities
    // These pairs allow us to efficiently retrieve the output of each
    //  module and store it in the main quantity map when running the system,
    //  report the results of a calculation using the output_ptrs, etc
    module_output_ptr_pairs = get_pointer_pairs(module_outputs, quantities, module_output_map);
    input_ptr_pairs = get_input_pointer_pairs(quantities, input_ptrs);
    output_ptr_pairs = get_output_pointer_pairs(quantities, output_ptrs);
}

/**
 * @brief Gets values from the objects pointed to by the `input_ptrs`,
 * runs each module, and exports the results to the objects pointed to by
 * the `output_ptrs`.
 */
void Standalone_SS::run() const
{
    // Get the input parameter values
    for (auto const& x : input_ptr_pairs) {
        *x.first = *x.second;
    }

    // Clear the module output map
    for (double* const& x : module_output_ptrs) {
        *x = 0.0;
    }

    // Run each module and store its output in the main quantity map
    for (std::unique_ptr<Module> const& m : steady_state_modules) {
        m->run();
        for (auto const& x : module_output_ptr_pairs) {
            *x.first = *x.second;
        }
    }

    // Export the parameter values
    for (auto const& x : output_ptr_pairs) {
        *x.second = *x.first;
    }
}