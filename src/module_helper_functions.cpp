#include "module_helper_functions.h"

/**
 * Returns a pointer to an element of a module output map, i.e., an output pointer (op)
 */
double* get_op(state_map* output_parameters, const std::string& name)
{
    if (output_parameters->find(name) == output_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_op: the quantity '") + name + std::string("' was not defined in the output_parameter state_map."));
    }

    return &((*output_parameters).at(name));
}

/**
 * Returns a pointer to an element of a module input map, i.e., an input pointer (ip)
 */
const double* get_ip(const state_map* input_parameters, const std::string& name)
{
    if (input_parameters->find(name) == input_parameters->end()) {
        throw quantity_access_error(std::string("Thrown by get_ip: the quantity '") + name + std::string("' was not defined in the input_parameter state_map."));
    }

    return &((*input_parameters).at(name));
}