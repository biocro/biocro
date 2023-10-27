#ifndef MULTILAYER_CANOPY_PHOTOSYNTHESIS_H
#define MULTILAYER_CANOPY_PHOTOSYNTHESIS_H

#include <algorithm>  // for std::find
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace MLCP  // helping functions for the MultiLayer Canopy Photosynthesis module
{
/**
 * @brief A helping function for the multilayer canopy photosynthesis module
 * that returns inputs to the leaf module that are also in the vector of
 * reference names.
 */
template <typename leaf_module_type>
string_vector get_leaf_input_subset(string_vector reference_names)
{
    string_vector leaf_inputs = leaf_module_type::get_inputs();
    string_vector result_vector;
    for (std::string const& name : reference_names) {
        if (std::find(leaf_inputs.begin(),
                      leaf_inputs.end(),
                      name) != leaf_inputs.end()) {
            result_vector.push_back(name);
        }
    }
    return result_vector;
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that will change with leaf class and canopy
 * layer.
 */
template <typename canopy_module_type, typename leaf_module_type>
string_vector get_multiclass_multilayer_leaf_inputs()
{
    return get_leaf_input_subset<leaf_module_type>(
        canopy_module_type::define_multiclass_multilayer_outputs());
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that will change with canopy layer but not
 * leaf class.
 */
template <typename canopy_module_type, typename leaf_module_type>
string_vector get_pure_multilayer_leaf_inputs()
{
    return get_leaf_input_subset<leaf_module_type>(
        canopy_module_type::define_pure_multilayer_outputs());
}

/**
 * @brief A helping function for the multlayer canopy photosynthesis module that
 * returns inputs to the leaf module that do not change with leaf class or
 * canopy layer.
 *
 * First, the names of all canopy module output quantities that do change with
 * leaf class or canopy layer are stored in a `string_vector` called
 * `layered_canopy_outputs`. Then, any leaf module input quantities that are
 * not contained in `layered_canopy_outputs` are stored in another
 * `string_vector` called `leaf_inputs_constant_through_canopy`, which is
 * returned as the output of this function.
 */
template <typename canopy_module_type, typename leaf_module_type>
string_vector get_other_leaf_inputs()
{
    string_vector layered_canopy_outputs;

    std::vector<string_vector> quantities_that_change =
        {canopy_module_type::define_multiclass_multilayer_outputs(),
         canopy_module_type::define_pure_multilayer_outputs()};

    for (string_vector const& sv : quantities_that_change) {
        for (std::string const& name : sv) {
            layered_canopy_outputs.push_back(name);
        }
    }

    string_vector leaf_inputs_constant_through_canopy;

    string_vector all_leaf_inputs = leaf_module_type::get_inputs();

    for (std::string const& name : all_leaf_inputs) {
        if (std::find(layered_canopy_outputs.begin(),
                      layered_canopy_outputs.end(),
                      name) == layered_canopy_outputs.end()) {
            leaf_inputs_constant_through_canopy.push_back(name);
        }
    }

    return leaf_inputs_constant_through_canopy;
}
}  // namespace MLCP

namespace standardBML
{
/**
 * @class multilayer_canopy_photosynthesis
 *
 * @brief Applies a leaf photosynthesis module to each layer and leaf class of a
 * multilayer canopy.
 *
 * ### Basic overview
 *
 * Two modules must be specified as template arguments:
 *
 *  - canopy properties module: a module that calculates properties for each
 *    canopy layer and leaf class
 *
 *  - leaf photosynthesis module: a module that determines assimilation values
 *    (among other values)
 *
 * The canopy properties module must have the following public static methods:
 *
 *  - define_leaf_classes()
 *
 *  - define_multiclass_multilayer_outputs()
 *
 *  - define_pure_multilayer_outputs()
 *
 *  ### More operational details
 *
 * In general, a canopy can be divided into different layers and leaf classes,
 * which could be based on factors such as angle or age. The canopy properties
 * module should determine the values of any properties (e.g. air temperature
 * or incident light intensity) that may vary between layers and/or leaf
 * classes.
 *
 * The values of these quantities must be subsequently passed to a leaf-level
 * photosynthesis module in order to determine assimilation rates for each type
 * of leaf in the canopy. In order to correctly pair a photosynthesis input
 * quantity (such as `incident_par`) with a corresponding value for a layer and
 * leaf class (such as `sunlit_incident_par_layer_0`), this module needs to know
 * which quantities depend on canopy layer and leaf class. For that reason, the
 * canopy properties module must have the following methods defined:
 *
 * - `define_leaf_classes()`: specifies each leaf class
 *
 * - `define_multiclass_multilayer_outputs()`: specifies the base names for
 *   quantities that change with canopy layer and leaf class
 *
 * - `define_pure_multilayer_outputs()`: specifies the base names for quantities
 *   that change with canopy layer but not leaf class
 *
 * The layer- and class-dependent names of these quantities are formed from a
 * base name (e.g. `incident_par`), a prefix that indicates the leaf class (e.g.
 * `sunlit_`), and a suffix that indicates the layer number (e.g. `_layer_0`).
 *
 * Note that this module has a non-standard constructor, so it cannot be created
 * using the module_factory. Rather, it is expected that directly-usable
 * classes will be derived from this class.
 */
template <typename canopy_module_type, typename leaf_module_type>
class multilayer_canopy_photosynthesis : public direct_module
{
   public:
    multilayer_canopy_photosynthesis(
        const int& nlayers,
        state_map const& input_quantities,
        state_map* output_quantities);

   private:
    // Number of layers
    const int nlayers;

    // Leaf photosynthesis module
    state_map leaf_module_quantities;
    state_map leaf_module_output_map;
    std::unique_ptr<module> leaf_module;

    // Pointers to input parameters
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_input_ptr_pairs;

    // Pointers to output parameters
    std::vector<std::vector<std::pair<double*, const double*>>> leaf_output_ptr_pairs;

   protected:
    static string_vector generate_inputs(int nlayers);
    static string_vector generate_outputs(int nlayers);
    void run() const;
};

/**
 * @brief Constructor for a multilayer canopy photosynthesis module, which
 * initializes the leaf module and prepares to pass inputs to it from the canopy
 * module.
 */
template <typename canopy_module_type, typename leaf_module_type>
multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::multilayer_canopy_photosynthesis(
    const int& nlayers,
    state_map const& input_quantities,
    state_map* output_quantities)
    : direct_module{},
      nlayers(nlayers)
{
    // Define a lambda for making quantity maps from vectors of inputs and outputs
    auto make_quantity_map = [](string_vector input_names, string_vector output_names) -> state_map {
        state_map result;
        for (string_vector const& sv : {input_names, output_names}) {
            for (std::string const& quantity_name : sv) {
                result[quantity_name] = 0.0;
            }
        }
        return result;
    };

    // Form a quantity state_map to pass to the leaf photosynthesis module
    leaf_module_quantities =
        make_quantity_map(
            leaf_module_type::get_inputs(),
            leaf_module_type::get_outputs());

    leaf_module_output_map = leaf_module_quantities;

    // Create the leaf photosynthesis module
    leaf_module =
        std::unique_ptr<module>(new leaf_module_type(
            leaf_module_quantities,
            &leaf_module_output_map));

    // Find subsets of the leaf model's inputs
    string_vector multiclass_multilayer_leaf_inputs =
        MLCP::get_multiclass_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();

    string_vector multilayer_leaf_inputs =
        MLCP::get_pure_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();

    string_vector other_leaf_inputs =
        MLCP::get_other_leaf_inputs<canopy_module_type, leaf_module_type>();

    // Create vectors of pointer pairs which will be used for passing inputs to
    // and getting outputs from the leaf module
    for (std::string const& class_name : canopy_module_type::define_leaf_classes()) {
        for (int i = 0; i < nlayers; ++i) {
            // Get pointer pairs for the leaf module inputs and store them
            std::vector<std::pair<double*, const double*>> input_ptr_pairs;

            for (std::string const& name : multiclass_multilayer_leaf_inputs) {
                std::string specific_name =
                    add_class_prefix_to_quantity_name(
                        class_name,
                        add_layer_suffix_to_quantity_name(nlayers, i, name));

                std::pair<double*, const double*> temporary(
                    get_op(&leaf_module_quantities, name),
                    get_ip(input_quantities, specific_name));

                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : multilayer_leaf_inputs) {
                std::string specific_name =
                    add_layer_suffix_to_quantity_name(nlayers, i, name);

                std::pair<double*, const double*> temporary(
                    get_op(&leaf_module_quantities, name),
                    get_ip(input_quantities, specific_name));

                input_ptr_pairs.push_back(temporary);
            }

            for (std::string const& name : other_leaf_inputs) {
                std::pair<double*, const double*> temporary(
                    get_op(&leaf_module_quantities, name),
                    get_ip(input_quantities, name));

                input_ptr_pairs.push_back(temporary);
            }

            leaf_input_ptr_pairs.push_back(input_ptr_pairs);

            // Get pointer pairs to the leaf module outputs and store them
            std::vector<std::pair<double*, const double*>> output_ptr_pairs;

            for (std::string const& name : leaf_module_type::get_outputs()) {
                std::string specific_name =
                    add_class_prefix_to_quantity_name(
                        class_name,
                        add_layer_suffix_to_quantity_name(nlayers, i, name));

                std::pair<double*, const double*> temporary(
                    get_op(output_quantities, specific_name),
                    get_ip(leaf_module_output_map, name));

                output_ptr_pairs.push_back(temporary);
            }

            leaf_output_ptr_pairs.push_back(output_ptr_pairs);
        }
    }
}

template <typename canopy_module_type, typename leaf_module_type>
string_vector multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_inputs(int nlayers)
{
    // Find subsets of the leaf model's inputs
    string_vector multiclass_multilayer_leaf_inputs =
        MLCP::get_multiclass_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();

    string_vector multilayer_leaf_inputs =
        MLCP::get_pure_multilayer_leaf_inputs<canopy_module_type, leaf_module_type>();

    string_vector other_leaf_inputs =
        MLCP::get_other_leaf_inputs<canopy_module_type, leaf_module_type>();

    // Generate the full list of inputs
    string_vector multilayer_inputs = generate_multiclass_quantity_names(
        canopy_module_type::define_leaf_classes(),
        multiclass_multilayer_leaf_inputs);

    for (std::string const& name : multilayer_leaf_inputs) {
        multilayer_inputs.push_back(name);
    }

    string_vector inputs = generate_multilayer_quantity_names(nlayers, multilayer_inputs);

    for (std::string const& name : other_leaf_inputs) {
        inputs.push_back(name);
    }

    return inputs;
}

template <typename canopy_module_type, typename leaf_module_type>
string_vector multilayer_canopy_photosynthesis<canopy_module_type, leaf_module_type>::generate_outputs(int nlayers)
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

}  // namespace standardBML
#endif
