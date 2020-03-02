#ifndef MODULE_HELPER_FUNCTIONS_H
#define MODULE_HELPER_FUNCTIONS_H

#include <stdexcept>
#include "state_map.h"

/**
 * Custom error class to indicate problems when accessing a quantity
 */
class quantity_access_error : virtual public std::runtime_error
{
   public:
    explicit quantity_access_error(const std::string& msg) : std::runtime_error(msg) {}
    virtual ~quantity_access_error() {}  // Virtual to allow subclassing
};

double* get_op(state_map* output_parameters, const std::string& name);

const double* get_ip(const state_map* input_parameters, const std::string& name);

#endif
