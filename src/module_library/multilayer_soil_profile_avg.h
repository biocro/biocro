#ifndef MULTILAYER_SOIL_PROFILE_AVG_H
#define MULTILAYER_SOIL_PROFILE_AVG_H

#include <cmath>  // for std::fmax
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class multilayer_soil_profile_avg
 * provides the average soil values from the multilayer soil profile
 */

class multilayer_soil_profile_avg : public direct_module

{
    static const size_t num_layers = 6;

    struct soil_layer {
        const double& depth;
        const double& water_content;
        const double& saturated_conductivity;
        const double& saturation_capacity;
        const double& field_capacity;
        const double& wilting_point;
        
        soil_layer(state_map const& input_quantities, state_map* output_quantities, size_t layer_num) :
            depth{get_input(input_quantities, "soil_depth_" + std::to_string(layer_num))},
            water_content{get_input(input_quantities, "soil_water_content_" + std::to_string(layer_num))},
            saturated_conductivity{get_input(input_quantities, "soil_saturated_conductivity_" + std::to_string(layer_num))},
            saturation_capacity{get_input(input_quantities, "soil_saturation_capacity_" + std::to_string(layer_num))},
            field_capacity{get_input(input_quantities, "soil_field_capacity_" + std::to_string(layer_num))},
            wilting_point{get_input(input_quantities, "soil_wilting_point_" + std::to_string(layer_num))}
           {
                

            }
    }; 
   public:
    multilayer_soil_profile_avg(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},

          // Get pointers to output quantities - Change in water content of each layer
          soil_water_content_op{get_op(output_quantities, "soil_water_content")},
          soil_saturated_conductivity_op{get_op(output_quantities, "soil_saturated_conductivity")},
          soil_saturation_capacity_op{get_op(output_quantities, "soil_saturation_capacity")},
          soil_field_capacity_op{get_op(output_quantities, "soil_field_capacity")},
          soil_wilting_point_op{get_op(output_quantities, "soil_wilting_point")}
    {
        
        layers.reserve(num_layers);
        for (size_t i = 1; i < num_layers + 1; ++i)
            layers.emplace_back(soil_layer(input_quantities, output_quantities, i));
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "multilayer_soil_profile_avg"; }

   private:
    // References to input quantities

    double const& max_rooting_layer;
    std::vector<soil_layer> layers;

    // Pointers to output parameters
    double* soil_water_content_op;
    double* soil_saturated_conductivity_op;
    double* soil_saturation_capacity_op;
    double* soil_field_capacity_op;
    double* soil_wilting_point_op;

    // Main operation
    void do_operation() const;
};

string_vector multilayer_soil_profile_avg::get_inputs()
{   
    string_vector names = {
        "soil_depth",
        "soil_water_content",
        "soil_saturated_conductivity",
        "soil_saturation_capacity",
        "soil_field_capacity",
        "soil_wilting_point"};
        
    string_vector inputs;
    inputs.reserve(names.size() * num_layers + 1);
    for (size_t i = 1; i < num_layers + 1; ++i) {
        for (auto name : names) 
            inputs.push_back(name + "_" + std::to_string(i));
    }
    inputs.push_back("max_rooting_layer");
    return inputs;
}

string_vector multilayer_soil_profile_avg::get_outputs()
{
    return {
        "soil_water_content",
        "soil_saturated_conductivity",
        "soil_saturation_capacity",
        "soil_field_capacity",
        "soil_wilting_point"};
}

void multilayer_soil_profile_avg::do_operation() const
{
    if (max_rooting_layer > num_layers) {
        throw std::out_of_range("`max_rooting_layer` exceeds the number of layers in the module `multilayer_soil_profile_avg`. Expected `max_rooting_layer` < " + std::to_string(num_layers));
    }

    // total soil depth - first four layers for Miscanthus roots (30-45cm)
    // double total_soil_depth = 0.0;
    // int total_number_of_layers = 0;
    // double max_depth = 45.0;
    // while (total_soil_depth <= max_depth) {
    // total_soil_depth += soil_depth_arr[total_number_of_layers];
    // total_number_of_layers += 1;
    // }
    double tot_soil_depth = 0.0;
    double tot_soil_water_content = 0.0;
    double tot_soil_saturated_conductivity = 0.0;
    double tot_soil_saturation_capacity = 0.0;
    double tot_soil_field_capacity = 0.0;
    double tot_soil_wilting_point = 0.0;

    for (int l = 0; l < max_rooting_layer; l++) {
        tot_soil_depth += layers[l].depth;
        tot_soil_water_content += layers[l].water_content * layers[l].depth;
        tot_soil_saturated_conductivity += layers[l].saturated_conductivity * layers[l].depth;
        tot_soil_saturation_capacity += layers[l].saturation_capacity * layers[l].depth;
        tot_soil_field_capacity += layers[l].field_capacity * layers[l].depth;
        tot_soil_wilting_point += layers[l].wilting_point * layers[l].depth;
    }
    // Voulmetric average soil parametric values
    double soil_water_content = tot_soil_water_content / tot_soil_depth;
    double soil_saturated_conductivity = tot_soil_saturated_conductivity / tot_soil_depth;
    double soil_saturation_capacity = tot_soil_saturation_capacity / tot_soil_depth;
    double soil_field_capacity = tot_soil_field_capacity / tot_soil_depth;
    double soil_wilting_point = tot_soil_wilting_point / tot_soil_depth;

    update(soil_water_content_op, soil_water_content);
    update(soil_saturated_conductivity_op, soil_saturated_conductivity);
    update(soil_saturation_capacity_op, soil_saturation_capacity);
    update(soil_field_capacity_op, soil_field_capacity);
    update(soil_wilting_point_op, soil_wilting_point);
}
}  // namespace standardBML
#endif
