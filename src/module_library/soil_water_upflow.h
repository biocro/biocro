#ifndef SOIL_WATER_UPFLOW_H
#define SOIL_WATER_UPFLOW_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_water_flow_functions.h"
#include <algorithm> // for std::max

namespace standardBML
{
/**
 * @class soil_water_downflow
 *
 * @brief Calculates water flow through the soil.
 *
 */
class soil_water_upflow : public direct_module
{
    static const int num_layers = 6;
    
   public:
    soil_water_upflow(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),
          profile(input_quantities, output_quantities){
            for (size_t i = 1; i < num_layers+1; ++i)
                upflow_op.emplace_back(get_op(output_quantities, "upflow_" + std::to_string(i)));            
          }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_upflow"; }

   private:
    // References to input quantities
    soil_water_profile<num_layers> profile;
    std::vector<double*> upflow_op;
    // Main operation
    void do_operation() const;
};

string_vector soil_water_upflow::get_inputs()
{
    return soil_water_profile<num_layers>::inputs();
}

string_vector soil_water_upflow::get_outputs()
{
    string_vector out;
    out.reserve(num_layers);
    for (size_t i = 1; i < num_layers + 1; ++i) 
        out.push_back("upflow_" + std::to_string(i));
    return out;
}

void soil_water_upflow::do_operation() const
{
   
    for (size_t i =0; i < num_layers-1; ++i) {
        double U = profile.upflow(profile.layers[i],profile.layers[i+1]);
        update(upflow_op[i], U);
    }
   
}

}  // namespace standardBML
#endif
