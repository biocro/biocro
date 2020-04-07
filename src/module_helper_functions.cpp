#include "module_helper_functions.h"

/**
 * @brief Returns a pointer to an element of a module output map, i.e., an output pointer (op)
 */
double* get_op(state_map* output_parameters, std::string const& name)
{
    if (output_parameters->find(name) == output_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_op: the quantity '") + name + std::string("' was not defined in the output_parameter state_map."));
    }

    return &((*output_parameters).at(name));
}

/**
 * @brief Returns a vector of output pointers
 */
std::vector<double*> get_op(state_map* output_parameters, std::vector<std::string> const& names)
{
    std::vector<double*> ops(names.size());
    for (size_t i = 0; i < names.size(); ++i) {
        ops[i] = get_op(output_parameters, names[i]);
    }
    return ops;
}

/**
 * @brief Returns a pointer to an element of a module input map, i.e., an input pointer (ip)
 */
const double* get_ip(const state_map* input_parameters, std::string const& name)
{
    if (input_parameters->find(name) == input_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_ip: the quantity '") + name + std::string("' was not defined in the input_parameter state_map."));
    }

    return &((*input_parameters).at(name));
}

/**
 * @brief Returns a vector of input pointers
 */
std::vector<const double*> get_ip(const state_map* input_parameters, std::vector<std::string> const& names)
{
    std::vector<const double*> ips(names.size());
    for (size_t i = 0; i < names.size(); ++i) {
        ips[i] = get_ip(input_parameters, names[i]);
    }
    return ips;
}
