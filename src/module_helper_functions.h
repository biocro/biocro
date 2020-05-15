#ifndef MODULE_HELPER_FUNCTIONS_H
#define MODULE_HELPER_FUNCTIONS_H

#include <stdexcept>
#include <map>
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

double* get_op(state_map* output_parameters, std::string const& name);

std::vector<double*> get_op(state_map* output_parameters, std::vector<std::string> const& names);

const double* get_ip(const state_map* input_parameters, std::string const& name);

std::vector<const double*> get_ip(const state_map* input_parameters, std::vector<std::string> const& names);

double const& get_input(const state_map* input_parameters, const std::string& name);

std::string add_class_prefix_to_quantity_name(std::string class_name, std::string quantity_name);

std::vector<std::string> generate_multiclass_quantity_names(std::vector<std::string> class_names, std::vector<std::string> quantity_names);

std::string add_layer_suffix_to_quantity_name(int nlayers, int current_layer, std::string quantity_name);

std::vector<std::string> generate_multilayer_quantity_names(int nlayers, std::vector<std::string> quantity_names);

std::vector<double*> get_multilayer_op(state_map* output_parameters, int nlayers, std::string const& name);

std::vector<const double*> get_multilayer_ip(const state_map* input_parameters, int nlayers, std::string const& name);

void check_error_conditions(std::map<std::string, bool> errors_to_check, std::string module_name);

#endif
