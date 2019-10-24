#include "modules.h"

// A small number to use when checking to see if a double is different than 0, positive, equal to another number, etc
const double Module::eps = 1e-10;

// This is a helping function that returns the value pointed to by a pointer, throwing an error if NaN occurs
// This should only be used for debugging purposes since it will slow down the module execution
double Module::get_val_debug(const double* ptr, const std::string name) const {
    double val = *ptr;
    //if(std::isnan(val)) throw std::logic_error(std::string("Found NaN when accessing parameter '") + name + ("'.\n"));    // What is wrong with this line??
    if(fabs(val) > 10000) throw std::logic_error(std::string("Parameter '") + name + ("' has a huge value: ") + std::to_string(val) + std::string("\n"));
    return val;
}

// This function generates a list of parameter names based on the number of layers
std::vector<std::string> MultilayerModule::get_multilayer_param_names(int nlayers, std::vector<std::string> param_names) {
    char buff[128];     // Hopefully no parameter names are this long!
    std::vector<std::string> full_multilayer_output_vector;
    for(size_t i = 0; i < param_names.size(); i++) {
        std::string basename = param_names[i];
        for(int j = 0; j < nlayers; j++) {
            if(nlayers < 10) sprintf(buff, "%.1i", j);
            else sprintf(buff, "%.2i", j);
            full_multilayer_output_vector.push_back(basename + std::string(buff));
        }
    }
    return full_multilayer_output_vector;
}

// Analagous to Module::get_op
const std::vector<double*> MultilayerModule::get_multilayer_op(std::unordered_map<std::string, double>* output_parameters, int nlayers, const std::string& name) const {
    std::vector<std::string> param_base_name_vector(1, name);
    std::vector<std::string> param_names = get_multilayer_param_names(nlayers, param_base_name_vector);
    std::vector<double*> multilayer_op(nlayers);
    for(int i = 0; i < nlayers; i++) {
        std::string layer_name = param_names[i];
        if(output_parameters->find(layer_name) != output_parameters->end()) multilayer_op[i] = &((*output_parameters).at(layer_name));
        else throw std::logic_error(layer_name);
    }
    return multilayer_op;
}

// Analagous to Module::get_ip
const std::vector<const double*> MultilayerModule::get_multilayer_ip(const std::unordered_map<std::string, double>* input_parameters, int nlayers, const std::string& name) const {
    std::vector<std::string> param_base_name_vector(1, name);
    std::vector<std::string> param_names = get_multilayer_param_names(nlayers, param_base_name_vector);
    std::vector<const double*> multilayer_ip(nlayers);
    for(int i = 0; i < nlayers; i++) {
        std::string layer_name = param_names[i];
        if(input_parameters->find(layer_name) != input_parameters->end()) multilayer_ip[i] = &((*input_parameters).at(layer_name));
        else throw std::logic_error(layer_name);
    }
    return multilayer_ip;
}