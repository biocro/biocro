#include "multilayer_rue_canopy.h"

using standardBML::ten_layer_rue_canopy;
using standardBML::ten_layer_rue_canopy_parent;

int const ten_layer_rue_canopy::nlayers = 10;  // Set the number of layers

string_vector ten_layer_rue_canopy::get_inputs()
{
    // Just call the parent class's input function with the appropriate number
    // of layers
    return ten_layer_rue_canopy_parent::generate_inputs(
        ten_layer_rue_canopy::nlayers);
}

string_vector ten_layer_rue_canopy::get_outputs()
{
    // Just call the parent class's output function with the appropriate number
    // of layers
    return ten_layer_rue_canopy_parent::generate_outputs(
        ten_layer_rue_canopy::nlayers);
}

void ten_layer_rue_canopy::do_operation() const
{
    // Just call the parent class's run operation
    ten_layer_rue_canopy_parent::run();
}
