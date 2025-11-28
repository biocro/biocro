#ifndef MULTI_LAYER_SOIL_PROFILE_H
#define MULTI_LAYER_SOIL_PROFILE_H

#include <cmath>  // for std::fmax
#include <vector> // std::vector
#include <string> // std::to_string
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class multi_layer_soil_profile
 *
 */

class multi_layer_soil_profile : public differential_module
{
    static const size_t num_layers = 6;

    struct soil_layer {
        const double& depth;
        const double& water_content;
        const double& deltaS;
        const double& deltaU;
        const double& deltaT;
        const double& uptake;
        
        double* water_content_op;
        
        soil_layer(state_map const& input_quantities, state_map* output_quantities, size_t layer_num) :
            depth{get_input(input_quantities, "soil_depth_" + std::to_string(layer_num))},
            water_content{get_input(input_quantities, "soil_water_content_" + std::to_string(layer_num))},
            deltaS{get_input(input_quantities, "deltaS_" + std::to_string(layer_num))},
            deltaU{get_input(input_quantities, "deltaU_" + std::to_string(layer_num))},
            deltaT{get_input(input_quantities, "deltaT_" + std::to_string(layer_num))},
            uptake{get_input(input_quantities, "uptake_layer_" + std::to_string(layer_num))},
            water_content_op{get_op(output_quantities, "soil_water_content_" + std::to_string(layer_num))} {
                

            }
    }; 

   public:
    multi_layer_soil_profile(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")}

    {   
        layers.reserve(num_layers);
        for (size_t i = 1; i < num_layers + 1; ++i)
            layers.emplace_back(soil_layer(input_quantities, output_quantities, i));
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "multi_layer_soil_profile"; }

   private:
    // References to input quantities
    double const& soil_evaporation_rate;
    std::vector <soil_layer> layers;
   
    // Main operation
    void do_operation() const;
};

string_vector multi_layer_soil_profile::get_inputs()
{
    string_vector names = {
        "soil_depth",
        "soil_water_content",
        "deltaS",
        "deltaU",
        "deltaT",
        "uptake_layer"
    };
    
    string_vector inputs;    
    inputs.reserve(1 + names.size() * num_layers);
    inputs.push_back("soil_evaporation_rate");
    for (size_t i = 1; i < num_layers + 1; ++i) { // index by 1
        for (auto name : names) 
            inputs.push_back(name + "_" + std::to_string(i));
    }
    return inputs;    
}

string_vector multi_layer_soil_profile::get_outputs()
{
    string_vector out;
    out.reserve(num_layers); 
    for (size_t i = 1; i < num_layers + 1; ++i) // index by 1
        out.push_back("soil_water_content_" + std::to_string(i));
    return out;
}

void multi_layer_soil_profile::do_operation() const
{
   

    double ds;
    for (int l = 0; l < layers.size(); l++) {
     // Calculate total change in soil water content

        constexpr double cm_per_m = 100;
        // adding uptake because value is negative
        ds = layers[l].deltaS + layers[l].deltaU + layers[l].deltaT + (layers[l].uptake / (cm_per_m * layers[l].depth));
        // ensure output is not negative; this is an Euler method step.     
        if (layers[l].water_content + ds < 0) { 
            ds = -layers[l].water_content;
        }
            
        if (l == 0) {
            ds -= soil_evaporation_rate / (10.0 * layers[l].depth);
        }      
        // Mg/ha/hr
    // double soil_evap = soil_evaporation_rate/ 10.0; // Mg/ha/hr to mm/hr
    // remove soil evaporation from first layer, but don't let water content go
    // negative. This is a crude, and hopefully temporary, fix. -mlm/    
        update(layers[l].water_content_op, ds);
    }
 
}
}  // namespace standardBML
#endif
