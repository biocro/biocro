#ifndef MULTILAYER_C3_CANOPY_H
#define MULTILAYER_C3_CANOPY_H

#include "multilayer_canopy_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "c3_leaf_photosynthesis.h"

/**
 * @class ten_layer_c3_canopy
 * 
 * @brief A child class of multilayer_canopy_photosynthesis where:
 *  - The canopy module is set to the `ten_layer_canopy_properties` module
 *  - The leaf module is set to the `c3_leaf_photosynthesis` module
 *  - The number of layers is set to 10
 * Instances of this class can be created using the module factory.
 */
class ten_layer_c3_canopy : public multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c3_leaf_photosynthesis>
{
   public:
    ten_layer_c3_canopy(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c3_leaf_photosynthesis>(
              "ten_layer_c3_canopy",
              ten_layer_c3_canopy::nlayers,
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

const int ten_layer_c3_canopy::nlayers = 10;  // Set the number of layers

std::vector<std::string> ten_layer_c3_canopy::get_inputs()
{
    // Just call the parent class's input function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c3_leaf_photosynthesis>::generate_inputs(ten_layer_c3_canopy::nlayers);
}

std::vector<std::string> ten_layer_c3_canopy::get_outputs()
{
    // Just call the parent class's output function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c3_leaf_photosynthesis>::generate_outputs(ten_layer_c3_canopy::nlayers);
}

void ten_layer_c3_canopy::do_operation() const
{
    // Just call the parent class's run operation
    multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c3_leaf_photosynthesis>::run();
}

#endif
