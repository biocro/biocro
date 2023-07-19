#ifndef STOMATA_WATER_STRESS_SIGMOID_H
#define STOMATA_WATER_STRESS_SIGMOID_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>  // for exp

namespace standardBML
{
class stomata_water_stress_sigmoid : public direct_module
{
   public:
    stomata_water_stress_sigmoid(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},
          phi1_ip{get_ip(input_quantities, "phi1")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_sigmoid"; }

   private:
    // Pointers to input quantities
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;
    const double* phi1_ip;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_sigmoid::get_inputs()
{
    return {
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content",
        "phi1"};
}

string_vector stomata_water_stress_sigmoid::get_outputs()
{
    return {
        "StomataWS"};
}

void stomata_water_stress_sigmoid::do_operation() const
{
    // Collect inputs and make calculations
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
    double phi1 = *phi1_ip;

    const double phi10 = (soil_field_capacity + soil_wilting_point) / 2;

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(1.0 / (1.0 + exp((phi10 - soil_water_content) / phi1)), 1e-10), 1.0));
}

}  // namespace standardBML
#endif
