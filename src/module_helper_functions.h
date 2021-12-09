#ifndef MODULE_HELPER_FUNCTIONS_H
#define MODULE_HELPER_FUNCTIONS_H

#include <stdexcept>
#include <map>
#include <vector>
#include "state_map.h"  // for state_map and string_vector

/**
 * @brief Custom error class to indicate problems when accessing a
 * quantity
 */
class quantity_access_error : virtual public std::runtime_error
{
   public:
    explicit quantity_access_error(const std::string& msg) : std::runtime_error(msg) {}
    virtual ~quantity_access_error() {}  // Virtual to allow subclassing
};

double* get_op(state_map* output_quantities, std::string const& name);

std::vector<double*> get_op(state_map* output_quantities, string_vector const& names);

const double* get_ip(state_map const& input_quantities, std::string const& name);

std::vector<const double*> get_ip(state_map const& input_quantities, string_vector const& names);

double const& get_input(state_map const& input_quantities, const std::string& name);

std::string add_class_prefix_to_quantity_name(std::string class_name, std::string quantity_name);

string_vector generate_multiclass_quantity_names(string_vector class_names, string_vector quantity_names);

std::string add_layer_suffix_to_quantity_name(int nlayers, int current_layer, std::string quantity_name);

string_vector generate_multilayer_quantity_names(int nlayers, string_vector quantity_names);

std::vector<double*> get_multilayer_op(state_map* output_quantities, int nlayers, std::string const& name);

std::vector<const double*> get_multilayer_ip(state_map const& input_quantities, int nlayers, std::string const& name);

void check_error_conditions(std::map<std::string, bool> errors_to_check, std::string module_name);

#endif
