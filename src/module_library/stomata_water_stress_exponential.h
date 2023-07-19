#ifndef STOMATA_WATER_STRESS_EXPONENTIAL_H
#define STOMATA_WATER_STRESS_EXPONENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>  // for exp

namespace standardBML
{
class stomata_water_stress_exponential : public direct_module
{
   public:
    stomata_water_stress_exponential(state_map const& input_quantities, state_map* output_quantities)
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
    static std::string get_name() { return "stomata_water_stress_exponential"; }

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

string_vector stomata_water_stress_exponential::get_inputs()
{
    return {
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content"};
}

string_vector stomata_water_stress_exponential::get_outputs()
{
    return {
        "StomataWS"};
}

void stomata_water_stress_exponential::do_operation() const
{
    // Collect inputs and make calculations
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;

    double slope = (1.0 - soil_wilting_point) / (soil_field_capacity - soil_wilting_point);
    double intercept = 1.0 - soil_field_capacity * slope;
    double theta = slope * soil_water_content + intercept;

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max((1.0 - exp(-2.5 * (theta - soil_wilting_point) / (1.0 - soil_wilting_point))) / (1.0 - exp(-2.5)), 1e-10), 1.0));
}

}  // namespace standardBML
#endif
