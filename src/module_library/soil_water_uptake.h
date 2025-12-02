#ifndef SOIL_WATER_UPTAKE_H
#define SOIL_WATER_UPTAKE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"
#include <algorithm> // for std::min

namespace standardBML
{
class soil_water_uptake : public direct_module
{
    static const int num_layers = 6;
    struct soil_layer {
        const double& depth;
        const double& water_content;
        
        double* uptake_op;

        soil_layer(state_map const& input_quantities, state_map* output_quantities, int layer) :
        depth{get_input(input_quantities, "soil_depth_" + std::to_string(layer))},
        water_content{get_input(input_quantities, "soil_water_content_" + std::to_string(layer))},
        uptake_op{get_op(output_quantities, "uptake_layer_" + std::to_string(layer))} {}
    };
   public:
    soil_water_uptake(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")}

    {
        for (int i = 1;  i<num_layers +1; ++i ){
            layers.emplace_back(soil_layer(input_quantities, output_quantities, i));
        }
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_uptake"; }

   private:
    // references to input quantities
    std::vector<soil_layer> layers;
    double const& max_rooting_layer;
    double const& canopy_transpiration_rate;  // Mg/ha/hr

    // main operation
    void do_operation() const;
};

string_vector soil_water_uptake::get_inputs()
{
    string_vector names = {"soil_depth", "soil_water_content"};
    string_vector inputs;
    inputs.reserve(names.size() * num_layers + 2);
    inputs.push_back("max_rooting_layer");
    inputs.push_back("canopy_transpiration_rate");
    for(int i = 1; i < num_layers+1; ++i )
        for(auto name : names)
            inputs.push_back(name + "_" + std::to_string(i));
    return inputs;
}

string_vector soil_water_uptake::get_outputs()
{
    string_vector outputs;
    outputs.reserve(num_layers);
    for (int i = 1; i < num_layers+ 1; ++i)
        outputs.push_back("uptake_layer_" + std::to_string(i));
    return outputs;
}

void soil_water_uptake::do_operation() const
{
    // initialize root depth to be 0
    double root_depth = 0;

    // get total root depth
    int L = std::min(static_cast<int>(max_rooting_layer), num_layers);// what if max_rooting_layer > num_layers?
    for (int i = 0; i < max_rooting_layer; i++) { 
        root_depth += layers[i].depth;  // cm
    }

    // uptake in layer = ET*(depth of layer/total depth of root zone)
    // add constraint if soil_water_content_layer < uptake
    double uptake;
    for (int i = 0; i < num_layers; i++) {
        if (i < max_rooting_layer) { 
            uptake = -canopy_transpiration_rate * (layers[i].depth / root_depth);  // (Mg*hr/ha)*(cm/cm) = Mg/ha/hr
        } else {
            uptake = 0;
        }
        update(layers[i].uptake_op, uptake);
    }
}

}  // namespace standardBML
#endif
