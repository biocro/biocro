#ifndef MULTILAYER_CANOPY_PHOTOSYNTHESIS_H
#define MULTILAYER_CANOPY_PHOTOSYNTHESIS_H

#include <algorithm>                     // for std::find
#include "../system_helper_functions.h"  // for get_pointer_pairs
#include "../modules.h"
#include "../state_map.h"

namespace MLCP  // helping functions for the MultiLayer Canopy Photosynthesis module
{
/**
 * @brief A helping function for the multilayer canopy photosynthesis module that
 * returns inputs to the leaf module that are also in the vector of reference names.
 */
template <typename leaf_module_type>
std::vector<std::string> get_leaf_input_subset(std::vector<std::string> reference_names)
{
    std::vector<std::string> leaf_inputs = leaf_module_type::get_inputs();
    std::vector<std::string> result_vector;
    for (std::string const& name : reference_names) {
        if (std::find(leaf_inputs.begin(), leaf_inputs.end(), name) != leaf_inputs.end()) {
            result_vector.push_back(name);
        }
    }
    return result_vector;
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that will change with leaf class and canopy layer.
 */
template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> get_multiclass_multilayer_leaf_inputs()
{
    return get_leaf_input_subset<leaf_module_type>(canopy_module_type::define_multiclass_multilayer_outputs());
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that will change with canopy layer but not leaf class.
 */
template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> get_pure_multilayer_leaf_inputs()
{
    return get_leaf_input_subset<leaf_module_type>(canopy_module_type::define_pure_multilayer_outputs());
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that do not change with leaf class or canopy layer.
 */
template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> get_other_leaf_inputs()
{
    std::vector<std::string> layered_canopy_outputs;
    for (std::vector<std::string> const& sv : {canopy_module_type::define_multiclass_multilayer_outputs(),
                                               canopy_module_type::define_pure_multilayer_outputs()}) {
        for (std::string const& name : sv) {
            layered_canopy_outputs.push_back(name);
        }
    }

    std::vector<std::string> other_leaf_inputs;
    std::vector<std::string> leaf_inputs = leaf_module_type::get_inputs();
    for (std::string const& name : leaf_inputs) {
        if (std::find(layered_canopy_outputs.begin(), layered_canopy_outputs.end(), name) == layered_canopy_outputs.end()) {
            other_leaf_inputs.push_back(name);
        }
    }

    return other_leaf_inputs;
}
}  // namespace MLCP

/**
 * @class multilayer_canopy_photosynthesis
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
class multilayer_canopy_photosynthesis : public SteadyModule
{
   public:
    multilayer_canopy_photosynthesis(
        const std::string& module_name,
        const int& nlayers,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters);

   private:
    // Number of layers
    const int nlayers;
    // Leaf photosynthesis module
    state_map leaf_module_quantities;
    state_map leaf_module_output_map;
    std::unique_ptr<Module> leaf_module;
    // Pointers to input parameters
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_input_ptr_pairs;
    // Pointers to output parameters
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_output_ptr_pairs;

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
multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::multilayer_canopy_photosynthesis(
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

    // Form a quantity state_map to pass to the leaf photosynthesis module
    leaf_module_quantities = make_quantity_map(leaf_module_type::get_inputs(), leaf_module_type::get_outputs());
    leaf_module_output_map = leaf_module_quantities;

    // Create the leaf photosynthesis module
    leaf_module = std::unique_ptr<Module>(new leaf_module_type(&leaf_module_quantities, &leaf_module_output_map));

    // Find subsets of the leaf model's inputs
    std::vector<std::string> multiclass_multilayer_leaf_inputs = MLCP::get_multiclass_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> multilayer_leaf_inputs = MLCP::get_pure_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();
    std::vector<std::string> other_leaf_inputs = MLCP::get_other_leaf_inputs<canopy_module_type, leaf_module_type>();

    // Create vectors of pointer pairs which will be used for passing inputs to and getting outputs from the leaf module
    for (std::string const& class_name : canopy_module_type::define_leaf_classes()) {
        for (int i = 0; i < nlayers; ++i) {
            // Get pointer pairs for the leaf module inputs and store them
            std::vector<std::pair<double*, const double*>> input_ptr_pairs;

            for (std::string const& name : multiclass_multilayer_leaf_inputs) {
                std::string specific_name = add_class_prefix_to_quantity_name(class_name, add_layer_suffix_to_quantity_name(nlayers, i, name));
                std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &input_parameters->at(specific_name));
                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : multilayer_leaf_inputs) {
                std::string specific_name = add_layer_suffix_to_quantity_name(nlayers, i, name);
                std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &input_parameters->at(specific_name));
                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : other_leaf_inputs) {
                std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &input_parameters->at(name));
                input_ptr_pairs.push_back(temporary);
            }
            
            leaf_input_ptr_pairs.push_back(input_ptr_pairs);

            // Get pointer pairs to the leaf module outputs and store them
            std::vector<std::pair<double*, const double*>> output_ptr_pairs;
            
            for (std::string const& name : leaf_module_type::get_outputs()) {
                std::string specific_name = add_class_prefix_to_quantity_name(class_name, add_layer_suffix_to_quantity_name(nlayers, i, name));
                std::pair<double*, const double*> temporary(&output_parameters->at(specific_name), &leaf_module_output_map.at(name));
                output_ptr_pairs.push_back(temporary);
            }
            
            leaf_output_ptr_pairs.push_back(output_ptr_pairs);
        }
    }
}

template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_inputs(int nlayers)
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
std::vector<std::string> multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_outputs(int nlayers)
{
    // Just add prefixes and suffixes to the leaf module outputs
    return generate_multilayer_quantity_names(
        nlayers,
        generate_multiclass_quantity_names(
            canopy_module_type::define_leaf_classes(),
            leaf_module_type::get_outputs()));
}

template <typename canopy_module_type, typename leaf_module_type>
void multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::run() const
{
    // For each combination of leaf class and layer number:
    for (size_t i = 0; i < leaf_input_ptr_pairs.size(); ++i) {
        // Update the inputs to the leaf module
        for (auto const& x : leaf_input_ptr_pairs[i]) {
            *x.first = *x.second;
        }

        // Run the leaf module
        leaf_module->run();

        // Update the outputs from the leaf module
        for (auto const& x : leaf_output_ptr_pairs[i]) {
            *x.first = *x.second;
        }
    }
}

#endif
