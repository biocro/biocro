#ifndef MULTILAYER_CANOPY_PHOTOSYNTHESIS
#define MULTILAYER_CANOPY_PHOTOSYNTHESIS

#include <algorithm>  // for std::find
#include "../modules.h"
#include "../state_map.h"
#include "../system_helper_functions.h"    // for get_pointer_pairs
#include "multilayer_canopy_properties.h"  // for the concrete derived class (should be temporary)
#include "c4_leaf_photosynthesis.h"        // for the concrete derived class (should be temporary)

/**
 * @class multilayer_canopy_photosynthesis
 * 
 * @brief Applies a leaf photosynthesis module to each layer and leaf class of a multilayer canopy.
 * 
 * Two modules must be specified as template arguments:
 *  - canopy_properties_module: a module that calculates properties for each canopy layer
 *  - leaf_photosynthesis_module: a module that determines assimilation values (among other values)
 * 
 * The canopy properties module must require a name and the number of layers as inputs to its
 * constructor. 
 * 
 * The canopy properties module must have the following public static methods:
 *  - get_inputs(int nlayers)
 *  - get_outputs(int nlayers)
 *  - define_leaf_classes()
 *  - define_multiclass_multilayer_outputs()
 *  - define_pure_multilayer_outputs()
 *  - define_other_outputs()
 */
template <typename canopy_module_type, typename leaf_module_type>
class multilayer_canopy_photosynthesis : public SteadyModule
{
   public:
    multilayer_canopy_photosynthesis(
        const std::string& module_name,
        const int& nlayers,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule(module_name),
          // Store the number of layers
          nlayers(nlayers)
    // Get references to input parameters
    // Get pointers to output parameters
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

        // Define a lambda for getting leaf module inputs that are also contained in another string vector
        std::vector<std::string> leaf_inputs = leaf_module_type::get_inputs();
        auto get_leaf_input_subset = [&leaf_inputs](std::vector<std::string> reference_names) -> std::vector<std::string> {
            std::vector<std::string> result_vector;
            for (std::string const& name : reference_names) {
                if (std::find(leaf_inputs.begin(), leaf_inputs.end(), name) != leaf_inputs.end()) {
                    result_vector.push_back(name);
                }
            }
            return result_vector;
        };

        // Form a quantity state_map to pass to the canopy properties module
        canopy_module_quantities = make_quantity_map(canopy_module_type::get_inputs(nlayers), canopy_module_type::get_outputs(nlayers));
        canopy_module_output_map = canopy_module_quantities;

        // Create vectors of pointer pairs which will be used for passing inputs to and getting outputs from the canopy module
        canopy_input_ptr_pairs = get_pointer_pairs(canopy_module_type::get_inputs(nlayers), canopy_module_quantities, *input_parameters);
        canopy_output_ptr_pairs = get_pointer_pairs(canopy_module_type::get_outputs(nlayers), *output_parameters, canopy_module_output_map);

        // Create the canopy properties module
        std::string canopy_module_name = module_name + std::string("_canopy_module");
        canopy_module = std::unique_ptr<Module>(new canopy_module_type(canopy_module_name, nlayers, &canopy_module_quantities, &canopy_module_output_map));

        // Form a quantity state_map to pass to the leaf photosynthesis module
        leaf_module_quantities = make_quantity_map(leaf_module_type::get_inputs(), leaf_module_type::get_outputs());
        leaf_module_output_map = leaf_module_quantities;

        // Create the leaf photosynthesis module
        leaf_module = std::unique_ptr<Module>(new leaf_module_type(&leaf_module_quantities, &leaf_module_output_map));

        // Find input parameters to the leaf module that are defined by the canopy module and
        // change for each leaf class and canopy layer
        std::vector<std::string> multiclass_multilayer_leaf_inputs = get_leaf_input_subset(canopy_module_type::define_multiclass_multilayer_outputs());

        // Find input parameters to the leaf module that are defined by the canopy module and
        // change only for each layer
        std::vector<std::string> multilayer_leaf_inputs = get_leaf_input_subset(canopy_module_type::define_pure_multilayer_outputs());

        // Find input parameters to the leaf module that are defined by the canopy module and
        // do not change with each layer
        std::vector<std::string> constant_leaf_inputs = get_leaf_input_subset(canopy_module_type::define_other_outputs());

        // Find input parameters to the leaf module that are not defined by the canopy module
        std::vector<std::string> all_canopy_outputs;
        for (std::vector<std::string> const& sv : {canopy_module_type::define_multiclass_multilayer_outputs(),
                                                   canopy_module_type::define_pure_multilayer_outputs(),
                                                   canopy_module_type::define_other_outputs()}) {
            for (std::string const& name : sv) {
                all_canopy_outputs.push_back(name);
            }
        }

        std::vector<std::string> other_leaf_inputs;
        for (std::string const& name : leaf_inputs) {
            if (std::find(all_canopy_outputs.begin(), all_canopy_outputs.end(), name) == all_canopy_outputs.end()) {
                other_leaf_inputs.push_back(name);
            }
        }

        // Create vectors of pointer pairs which will be used for passing inputs to and getting outputs from the leaf module
        for (std::string const& class_name : canopy_module_type::define_leaf_classes()) {
            for (int i = 0; i < nlayers; ++i) {
                // Initialize a vector of pointer pairs for the input pointer pairs
                std::vector<std::pair<double*, const double*>> input_ptr_pairs;

                // Get pointer pairs to the leaf inputs that depend on leaf class and canopy layer
                for (std::string const& name : multiclass_multilayer_leaf_inputs) {
                    std::string specific_name = add_class_prefix_to_quantity_name(class_name, add_layer_suffix_to_quantity_name(nlayers, i, name));
                    std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &canopy_module_output_map.at(specific_name));
                    input_ptr_pairs.push_back(temporary);
                }

                // Get pointer pairs to the leaf inputs that depend on just canopy layer
                for (std::string const& name : multilayer_leaf_inputs) {
                    std::string specific_name = add_layer_suffix_to_quantity_name(nlayers, i, name);
                    std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &canopy_module_output_map.at(specific_name));
                    input_ptr_pairs.push_back(temporary);
                }

                // Get pointer pairs to leaf inputs that are canopy outputs but do not depend on the layer
                for (std::string const& name : constant_leaf_inputs) {
                    std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &canopy_module_output_map.at(name));
                    input_ptr_pairs.push_back(temporary);
                }

                // Get pointer pairs to leaf inputs that are not canopy module outputs
                for (std::string const& name : other_leaf_inputs) {
                    std::pair<double*, const double*> temporary(&leaf_module_quantities.at(name), &input_parameters->at(name));
                    input_ptr_pairs.push_back(temporary);
                }

                // Store the input pointer pairs
                leaf_input_ptr_pairs.push_back(input_ptr_pairs);

                // Get pointer pairs to the leaf outputs
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

   private:
    // Number of layers
    const int nlayers;
    // Canopy properties module
    state_map canopy_module_quantities;
    state_map canopy_module_output_map;
    std::unique_ptr<Module> canopy_module;
    // Leaf photosynthesis module
    state_map leaf_module_quantities;
    state_map leaf_module_output_map;
    std::unique_ptr<Module> leaf_module;
    // Pointers to input parameters
    std::vector<std::pair<double*, const double*>> canopy_input_ptr_pairs;
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_input_ptr_pairs;
    // Pointers to output parameters
    std::vector<std::pair<double*, const double*>> canopy_output_ptr_pairs;
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_output_ptr_pairs;

   protected:
    static std::vector<std::string> generate_inputs(int nlayers);
    static std::vector<std::string> generate_outputs(int nlayers);
    void run() const;
};

template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_inputs(int nlayers)
{
    // First get all the inputs required by the canopy module
    std::vector<std::string> inputs = canopy_module_type::get_inputs(nlayers);

    // Now find all the outputs produced by the canopy module
    // Here we are only interested in the base outputs without
    // any prefixes or suffixes that describe the leaf class or
    // canopy layer.
    std::vector<std::string> canopy_outputs;
    for (std::vector<std::string> const& sv : {canopy_module_type::define_multiclass_multilayer_outputs(),
                                               canopy_module_type::define_pure_multilayer_outputs(),
                                               canopy_module_type::define_other_outputs()}) {
        for (std::string const& name : sv) {
            canopy_outputs.push_back(name);
        }
    }

    // Now we can find inputs required by the leaf module that are
    // not provided as outputs of the canopy module.
    for (std::string const& name : leaf_module_type::get_inputs()) {
        if (std::find(canopy_outputs.begin(), canopy_outputs.end(), name) == canopy_outputs.end()) {
            inputs.push_back(name);
        }
    }

    return inputs;
}

template <typename canopy_module_type, typename leaf_module_type>
std::vector<std::string> multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_outputs(int nlayers)
{
    // First add prefixes and suffixes to the leaf module outputs
    std::vector<std::string> outputs = generate_multilayer_quantity_names(
        nlayers,
        generate_multiclass_quantity_names(
            canopy_module_type::define_leaf_classes(),
            leaf_module_type::get_outputs()));

    // Now add all the outputs produced by the canopy module
    for (std::string const& name : canopy_module_type::get_outputs(nlayers)) {
        outputs.push_back(name);
    }

    return outputs;
}

template <typename canopy_module_type, typename leaf_module_type>
void multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::run() const
{
    // Update the inputs to the canopy module
    for (auto const& x : canopy_input_ptr_pairs) {
        *x.first = *x.second;
    }

    // Run the canopy module
    canopy_module->run();

    // Update the outputs from the canopy module
    for (auto const& x : canopy_output_ptr_pairs) {
        *x.first = *x.second;
    }

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

////////////////////////////////
// TEN LAYER C4 CANOPY MODULE //
////////////////////////////////

/**
 * @class ten_layer_c4_canopy
 * 
 * @brief A child class of multilayer_canopy_properties where the number of layers has been defined.
 * Instances of this class can be created using the module factory.
 */
class ten_layer_c4_canopy : public multilayer_canopy_photosynthesis<multilayer_canopy_properties, c4_leaf_photosynthesis>
{
   public:
    ten_layer_c4_canopy(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : multilayer_canopy_photosynthesis<multilayer_canopy_properties, c4_leaf_photosynthesis>(
              "ten_layer_c4_canopy",
              ten_layer_c4_canopy::nlayers,
              input_parameters,
              output_parameters)  // Create the base class with the appropriate number of layers
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    static std::string get_description();

   private:
    // Number of layers
    static const int nlayers;
    // Main operation
    void do_operation() const;
};

const int ten_layer_c4_canopy::nlayers = 10;  // Set the number of layers

std::vector<std::string> ten_layer_c4_canopy::get_inputs()
{
    // Just call the parent class's input function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<multilayer_canopy_properties, c4_leaf_photosynthesis>::generate_inputs(ten_layer_c4_canopy::nlayers);
}

std::vector<std::string> ten_layer_c4_canopy::get_outputs()
{
    // Just call the parent class's output function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<multilayer_canopy_properties, c4_leaf_photosynthesis>::generate_outputs(ten_layer_c4_canopy::nlayers);
}

void ten_layer_c4_canopy::do_operation() const
{
    // Just call the parent class's run operation
    multilayer_canopy_photosynthesis<multilayer_canopy_properties, c4_leaf_photosynthesis>::run();
}

#endif
