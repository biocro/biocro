#ifndef MULTILAYER_CANOPY_PHOTOSYNTHESIS_PARALLEL_H
#define MULTILAYER_CANOPY_PHOTOSYNTHESIS_PARALLEL_H

#include <algorithm>                     // for std::find
#include <omp.h>                         // for #pragma omp parallel
#include "../system_helper_functions.h"  // for get_pointer_pairs
#include "../modules.h"
#include "../state_map.h"
//#include "multilayer_canopy_photosynthesis.h" // for MLCP namespace?


/**
 * @class multilayer_canopy_photosynthesis_parallel
 *
 * @brief Applies a leaf photosynthesis module to each layer and leaf class of a multilayer canopy.
 * Note that this module cannot be created via the module_wrapper_factory since it is a template
 * class with a different constructor than a usual module. Rather, it is expected that directly-usable
 * classes will be derived from this class.
 *
 * Two modules must be specified as template arguments:
 *  - canopy_properties_module: a module that calculates properties for each canopy layer
 *  - leaf_photosynthesis_module: a module that determines assimilation values (among other values)
 *
 * The canopy properties module must have the following public static methods:
 *  - define_leaf_classes()
 *  - define_multiclass_multilayer_outputs()
 *  - define_pure_multilayer_outputs()
 */
template <typename canopy_module_type, typename leaf_module_type>
class multilayer_canopy_photosynthesis_parallel : public SteadyModule
{
   public:
    multilayer_canopy_photosynthesis_parallel(
        const std::string& module_name,
        const int& nlayers,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters);

   private:
    // Number of layers
    const int nlayers;
    // Leaf photosynthesis module
    std::vector<state_map> leaf_module_quantities;
    std::vector<state_map> leaf_module_output_map;
    std::vector<std::unique_ptr<Module>> leaf_modules;
    // Pointers to input parameters
    std::vector<std::vector<std::vector<std::pair<double*, const double*>>>> leaf_input_ptr_pairs;
    // Pointers to output parameters
    std::vector<std::vector<std::vector<std::pair<double*, const double*>>>> leaf_output_ptr_pairs;

   protected:
    static std::vector<std::string> generate_inputs(int nlayers);
    static std::vector<std::string> generate_outputs(int nlayers);
    void run() const;
};

/**
 * @brief Constructor for a multilayer canopy photosynthesis module, which initializes the
 * leaf module and prepares to pass inputs to it from the canopy module.
 */
template <typename canopy_module_type, typename leaf_module_type>
multilayer_canopy_photosynthesis_parallel<canopy_module_type, leaf_module_type>::multilayer_canopy_photosynthesis_parallel(
    const std::string& module_name,
    const int& nlayers,
    const std::unordered_map<std::string, double>* input_parameters,
    std::unordered_map<std::string, double>* output_parameters)
    : SteadyModule(module_name),
      nlayers(nlayers)
{
    // Define a lambda for making quantity maps from vectors of inputs and outputs
    auto make_quantity_map = [](std::vector<std::string> input_names, std::vector<std::string> output_names) -> state_map {
        state_map result;
        for (std::vector<std::string> const& sv : {input_names, output_names}) {
            for (std::string const& quantity_name : sv) {
                result[quantity_name] = 0.0;
            }
        }
        return result;
    };
    
    // For each canopy layer create a copy of the leaf-level module
    for (int i = 0; i < nlayers; ++i){
        // Form a quantity state_map to pass to leaf photosynthesis module
        leaf_module_quantities.push_back(make_quantity_map(leaf_module_type::get_inputs(), leaf_module_type::get_outputs()));
        leaf_module_output_map.push_back(leaf_module_quantities.at(i));
        
        // Create the leaf photosynthesis module
        leaf_modules.push_back(std::unique_ptr<Module>(new leaf_module_type(&leaf_module_quantities.at(i), &leaf_module_output_map.at(i))));
    }

    // Find subsets of the leaf model's inputs
    std::vector<std::string> multiclass_multilayer_leaf_inputs = MLCP::get_multiclass_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> multilayer_leaf_inputs = MLCP::get_pure_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> other_leaf_inputs = MLCP::get_other_leaf_inputs<canopy_module_type, leaf_module_type>();

    // Create vectors of pointer pairs which will be used for passing inputs to and getting outputs from the leaf module
    leaf_input_ptr_pairs.resize(nlayers);
    leaf_output_ptr_pairs.resize(nlayers);
    for (std::string const& class_name : canopy_module_type::define_leaf_classes()) {
        for (int i = 0; i < nlayers; ++i) {
            // Get pointer pairs for the leaf module inputs and store them
            std::vector<std::pair<double*, const double*>> input_ptr_pairs;

            for (std::string const& name : multiclass_multilayer_leaf_inputs) {
                std::string specific_name = add_class_prefix_to_quantity_name(class_name, add_layer_suffix_to_quantity_name(nlayers, i, name));
                std::pair<double*, const double*> temporary(&leaf_module_quantities[i].at(name), &input_parameters->at(specific_name));
                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : multilayer_leaf_inputs) {
                std::string specific_name = add_layer_suffix_to_quantity_name(nlayers, i, name);
                std::pair<double*, const double*> temporary(&leaf_module_quantities[i].at(name), &input_parameters->at(specific_name));
                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : other_leaf_inputs) {
                std::pair<double*, const double*> temporary(&leaf_module_quantities[i].at(name), &input_parameters->at(name));
                input_ptr_pairs.push_back(temporary);
            }

            leaf_input_ptr_pairs[i].push_back(input_ptr_pairs);

            // Get pointer pairs to the leaf module outputs and store them
            std::vector<std::pair<double*, const double*>> output_ptr_pairs;

            for (std::string const& name : leaf_module_type::get_outputs()) {
                std::string specific_name = add_class_prefix_to_quantity_name(class_name, add_layer_suffix_to_quantity_name(nlayers, i, name));
                std::pair<double*, const double*> temporary(&output_parameters->at(specific_name), &leaf_module_output_map[i].at(name));
                output_ptr_pairs.push_back(temporary);
            }

            leaf_output_ptr_pairs[i].push_back(output_ptr_pairs);
        }
    }
}

template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> multilayer_canopy_photosynthesis_parallel<canopy_module_type, leaf_module_type>::generate_inputs(int nlayers)
{
    // Find subsets of the leaf model's inputs
    std::vector<std::string> multiclass_multilayer_leaf_inputs = MLCP::get_multiclass_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> multilayer_leaf_inputs = MLCP::get_pure_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> other_leaf_inputs = MLCP::get_other_leaf_inputs<canopy_module_type, leaf_module_type>();

    // Generate the full list of inputs
    std::vector<std::string> multilayer_inputs = generate_multiclass_quantity_names(
        canopy_module_type::define_leaf_classes(),
        multiclass_multilayer_leaf_inputs);

    for (std::string const& name : multilayer_leaf_inputs) {
        multilayer_inputs.push_back(name);
    }

    std::vector<std::string> inputs = generate_multilayer_quantity_names(nlayers, multilayer_inputs);

    for (std::string const& name : other_leaf_inputs) {
        inputs.push_back(name);
    }

    return inputs;
}

template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> multilayer_canopy_photosynthesis_parallel<canopy_module_type, leaf_module_type>::generate_outputs(int nlayers)
{
    // Just add prefixes and suffixes to the leaf module outputs
    return generate_multilayer_quantity_names(
        nlayers,
        generate_multiclass_quantity_names(
            canopy_module_type::define_leaf_classes(),
            leaf_module_type::get_outputs()));
}

template <typename canopy_module_type, typename leaf_module_type>
void multilayer_canopy_photosynthesis_parallel<canopy_module_type, leaf_module_type>::run() const
{

    omp_set_num_threads(leaf_input_ptr_pairs.size());
//    int num_calls = (int) leaf_input_ptr_pairs.size();
    #pragma omp parallel for default(none), shared(leaf_input_ptr_pairs, leaf_output_ptr_pairs)//, shared(num_calls)
    // For each combination of leaf class and layer number:
//    for (size_t i = 0; i < leaf_input_ptr_pairs.size(); ++i) {
    for (int i = 0; i < nlayers; ++i){
    for (size_t j = 0; j < leaf_input_ptr_pairs[i].size(); ++j) {
        // Update the inputs to the leaf module
        for (auto const& x : leaf_input_ptr_pairs[i][j]) {
            *x.first = *x.second;
        }

        // Run the leaf module
        leaf_modules[i]->run();

        // Update the outputs from the leaf module
        for (auto const& x : leaf_output_ptr_pairs[i][j]) {
            *x.first = *x.second;
        }
    }
}
}

#endif

