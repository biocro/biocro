#ifndef ED_MULTILAYER_C4_CANOPY_H
#define ED_MULTILAYER_C4_CANOPY_H

#include "../state_map.h"
#include "multilayer_canopy_photosynthesis.h"
#include "ed_multilayer_canopy_properties.h"
#include "ed_c4_leaf_photosynthesis4.h"

/**
 * @class ed_ten_layer_c4_canopy
 *
 * @brief A child class of multilayer_canopy_photosynthesis where:
 *  - The canopy module is set to the `ed_ten_layer_canopy_properties` module
 *  - The leaf module is set to the `ed_c4_leaf_photosynthesis4` module
 *  - The number of layers is set to 10
 * Instances of this class can be created using the module factory.
 */
class ed_ten_layer_c4_canopy : public multilayer_canopy_photosynthesis<ed_ten_layer_canopy_properties, ed_c4_leaf_photosynthesis4>
{
   public:
    ed_ten_layer_c4_canopy(
        state_map const& input_quantities,
        state_map& output_quantities)
        : multilayer_canopy_photosynthesis<ed_ten_layer_canopy_properties, ed_c4_leaf_photosynthesis4>(
              "ed_ten_layer_c4_canopy",
              ed_ten_layer_c4_canopy::nlayers,
              input_quantities,
              output_quantities)  // Create the base class with the appropriate number of layers
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Number of layers
    static int const nlayers;
    // Main operation
    void do_operation() const;
};

int const ed_ten_layer_c4_canopy::nlayers = 10;  // Set the number of layers

string_vector ed_ten_layer_c4_canopy::get_inputs()
{
    // Just call the parent class's input function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<ed_ten_layer_canopy_properties, ed_c4_leaf_photosynthesis4>::generate_inputs(ed_ten_layer_c4_canopy::nlayers);
}

string_vector ed_ten_layer_c4_canopy::get_outputs()
{
    // Just call the parent class's output function with the appropriate number of layers
    return multilayer_canopy_photosynthesis<ed_ten_layer_canopy_properties, ed_c4_leaf_photosynthesis4>::generate_outputs(ed_ten_layer_c4_canopy::nlayers);
}

void ed_ten_layer_c4_canopy::do_operation() const
{
    // Just call the parent class's run operation
    multilayer_canopy_photosynthesis<ed_ten_layer_canopy_properties, ed_c4_leaf_photosynthesis4>::run();
}

#endif
