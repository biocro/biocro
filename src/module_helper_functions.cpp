#include "module_helper_functions.h"
#include <cmath>    // For log10
#include <sstream>  // For replicating old sprintf functionality with strings
#include <iomanip>  // For replicating old sprintf functionality with strings

/**
 * @brief Returns a pointer to an element of a module output map, i.e., an output pointer (op)
 */
double* get_op(state_map* output_parameters, const std::string& name)
{
    if (output_parameters->find(name) == output_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_op: the quantity '") + name + std::string("' was not defined in the output_parameter state_map."));
    }

    return &((*output_parameters).at(name));
}

/**
 * @brief Returns a vector of pointers to elements of a module output map, i.e., a vector of output pointers (op)
 */
std::vector<double*> get_op(state_map* output_parameters, std::vector<std::string> const& names)
{
    std::vector<double*> ops;
    for (std::string const& n : names) {
        ops.push_back(get_op(output_parameters, n));
    }
    return ops;
}

/**
 * @brief Returns a pointer to an element of a module input map, i.e., an input pointer (ip)
 */
const double* get_ip(const state_map* input_parameters, const std::string& name)
{
    if (input_parameters->find(name) == input_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_ip: the quantity '") + name + std::string("' was not defined in the input_parameter state_map."));
    }

    return &((*input_parameters).at(name));
}

/**
 * @brief Returns a vector of pointers to elements of a module input map, i.e., a vector of input pointers (ip)
 */
std::vector<const double*> get_ip(const state_map* input_parameters, std::vector<std::string> const& names)
{
    std::vector<const double*> ips;
    for (std::string const& n : names) {
        ips.push_back(get_ip(input_parameters, n));
    }
    return ips;
}

/**
 * @brief Returns a reference to an element of a module input map
 */
double const& get_input(const state_map* input_parameters, const std::string& name)
{
    if (input_parameters->find(name) == input_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_input: the quantity '") + name + std::string("' was not defined in the input_parameter state_map."));
    }

    return (*input_parameters).at(name);
}

/**
 * @brief Adds a class name prefix to the quantity name indicating it was calculated for a certain leaf class,
 * e.g. sunlit or shaded.
 */
std::string add_class_prefix_to_quantity_name(std::string class_name, std::string quantity_name)
{
    return class_name + std::string("_") + quantity_name;
}

/**
 * @brief Adds class name prefixes to each quantity name indicating that it was calculated for a certain class.
 * E.g., a multilayer canopy model may calculate different incident light irradiance values for sunlit and shaded
 * leaves. In this case "sunlit" and "shaded" would be the two leaf classes.
 */
std::vector<std::string> generate_multiclass_quantity_names(std::vector<std::string> class_names, std::vector<std::string> quantity_names)
{
    std::vector<std::string> full_multiclass_output_vector;
    for (std::string const& cn : class_names) {
        for (std::string const& qn : quantity_names) {
            full_multiclass_output_vector.push_back(add_class_prefix_to_quantity_name(cn, qn));
        }
    }
    return full_multiclass_output_vector;
}

/**
 * @brief Adds a suffix to a quantity name indicating that it was calculated for a certain layer.
 */
std::string add_layer_suffix_to_quantity_name(int nlayers, int current_layer, std::string quantity_name)
{
    std::stringstream param_name_stream;

    int num_digits = ceil(log10(nlayers - 1.0));  // Get the number of digits in the largest layer number

    param_name_stream << quantity_name
                      << "_layer_"
                      << std::setfill('0')
                      << std::setw(num_digits)
                      << current_layer;  // Add the suffix

    return param_name_stream.str();
}

/**
 * @brief For each entry in the `quantity_names` vector, multiple copies are produced, each of which
 * has a suffix indicating the corresponding layer.
 */
std::vector<std::string> generate_multilayer_quantity_names(int nlayers, std::vector<std::string> quantity_names)
{
    std::vector<std::string> full_multilayer_output_vector;
    for (size_t i = 0; i < quantity_names.size(); i++) {
        for (int j = 0; j < nlayers; j++) {
            full_multilayer_output_vector.push_back(add_layer_suffix_to_quantity_name(nlayers, j, quantity_names[i]));
        }
    }
    return full_multilayer_output_vector;
}

/**
 * @brief Analagous to `get_op` but returns a vector of output pointers, as required for a multilayer module
 */
std::vector<double*> get_multilayer_op(state_map* output_parameters, int nlayers, std::string const& name)
{
    std::vector<std::string> quantity_base_name_vector = {name};
    std::vector<std::string> quantity_names = generate_multilayer_quantity_names(nlayers, quantity_base_name_vector);
    std::vector<double*> multilayer_references(nlayers);
    for (int i = 0; i < nlayers; i++) {
        multilayer_references[i] = get_op(output_parameters, quantity_names[i]);
    }
    return multilayer_references;
}

/**
 * @brief Analagous to `get_ip` but returns a vector of output pointers, as required for a multilayer module
 */
std::vector<const double*> get_multilayer_ip(const state_map* input_parameters, int nlayers, std::string const& name)
{
    std::vector<std::string> quantity_base_name_vector = {name};
    std::vector<std::string> quantity_names = generate_multilayer_quantity_names(nlayers, quantity_base_name_vector);
    std::vector<const double*> multilayer_references(nlayers);
    for (int i = 0; i < nlayers; i++) {
        multilayer_references[i] = get_ip(input_parameters, quantity_names[i]);
    }
    return multilayer_references;
}

/**
 * @brief Checks over an `error_map` for problems. The `error_map` should pair a string description
 * of an important criterion which must be met, e.g. "Parameter_A must be non-zero", with a boolean
 * indicating whether it has been met. If the condition was not met, an appropriate error message will
 * be generated indicating both the problem and the module in which it occurred.
 */
void check_error_conditions(std::map<std::string, bool> errors_to_check, std::string module_name)
{
    for (auto const& x : errors_to_check) {
        if (x.second) {
            throw std::out_of_range(std::string("Thrown by the '") + module_name + std::string("' module: ") + x.first);
        }
    }
}
