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

          // Get references to input quantities
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          StomataWS_gradient{get_input(input_quantities, "StomataWS_gradient")},
          StomataWS_intercept{get_input(input_quantities, "StomataWS_intercept")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_linear_v2"; }

   private:
    // References to input quantities
    const double& soil_field_capacity;
    const double& soil_wilting_point;
    const double& soil_water_content;
    const double& StomataWS_gradient;
    const double& StomataWS_intercept;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_linear_v2::get_inputs()
{
    return {
        "soil_field_capacity",   // cm3 [water] /cm3 [soil]. Volumetric water content
        "soil_wilting_point",    // cm3 / cm3
        "soil_water_content",    // cm3 / cm3
        "StomataWS_gradient",    // dimensionless
        "StomataWS_intercept"};  // dimensionless
}

string_vector stomata_water_stress_linear_v2::get_outputs()
{
    return {
        "StomataWS"};  // dimensionless
}

void stomata_water_stress_linear_v2::do_operation() const
{
    // We assume StomataWS is between 0-1
    constexpr double StomataWS_min = 0.0;
    constexpr double StomataWS_max = 1.0;
    // Relative extractable water. Dimensionless
    const double REW = (soil_water_content - soil_wilting_point) / (soil_field_capacity - soil_wilting_point);
    // This linear fitted equation is from a reverse engineering of fitting observed gs data
    // Ref: Gray, S., Dermody, O., Klein, S. et al.
    // Intensifying drought eliminates the expected benefits of elevated carbon dioxide for soybean.
    // Nature Plants 2, 16132 (2016). https://doi.org/10.1038/nplants.2016.132
    double linear_fit = StomataWS_intercept + StomataWS_gradient * REW;

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(linear_fit, StomataWS_min), StomataWS_max));
}

}  // namespace standardBML
#endif
