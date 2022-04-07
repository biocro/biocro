#include "multilayer_c4_canopy.h"

int const standardBML::ten_layer_c4_canopy::nlayers = 10;  // Set the number of layers

string_vector standardBML::ten_layer_c4_canopy::get_inputs()
{
    // Just call the parent class's input function with the appropriate number
    // of layers
    return ten_layer_c4_canopy_parent::generate_inputs(
        ten_layer_c4_canopy::nlayers);
}

string_vector standardBML::ten_layer_c4_canopy::get_outputs()
{
    // Just call the parent class's output function with the appropriate number
    // of layers
    return ten_layer_c4_canopy_parent::generate_outputs(
        ten_layer_c4_canopy::nlayers);
}

void standardBML::ten_layer_c4_canopy::do_operation() const
{
    // Just call the parent class's run operation
    ten_layer_c4_canopy_parent::run();
}
