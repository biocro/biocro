#ifndef STOMATA_WATER_STRESS_LINEAR_V2_H
#define STOMATA_WATER_STRESS_LINEAR_V2_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class stomata_water_stress_linear_v2 : public direct_module
{
   public:
    stomata_water_stress_linear_v2(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_linear_v2"; }

   private:
    // Pointers to input quantities
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_linear_v2::get_inputs()
{
    return {
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content"};
}

string_vector stomata_water_stress_linear_v2::get_outputs()
{
    return {
        "StomataWS"};
}

void stomata_water_stress_linear_v2::do_operation() const
{
    // Collect inputs and make calculations
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
     
    double REW = (soil_water_content - soil_wilting_point) / (soil_field_capacity - soil_wilting_point);
    // This linear fitted equation is from a reverse engineering of fitting observed gs data
    // Ref: Gray, S., Dermody, O., Klein, S. et al. Intensifying drought eliminates the expected benefits of elevated carbon dioxide for soybean. Nature Plants 2, 16132 (2016). https://doi.org/10.1038/nplants.2016.132
    double linear_fit = 0.641 + 0.139 * REW; 
    // We assume StomataWS is between 0-1 
    const double StomataWS_min = 0.0;
    const double StomataWS_max = 1.0;

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(linear_fit, StomataWS_min), StomataWS_max));
}

}  // namespace standardBML
#endif
