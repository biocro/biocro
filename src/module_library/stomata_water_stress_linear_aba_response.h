#ifndef STOMATA_WATER_STRESS_LINEAR_AND_ABA_RESPONSE_H
#define STOMATA_WATER_STRESS_LINEAR_AND_ABA_RESPONSE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>  // for exp

namespace standardBML
{
class stomata_water_stress_linear_and_aba_response : public direct_module
{
   public:
    stomata_water_stress_linear_and_aba_response(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},
          soil_aba_concentration_ip{get_ip(input_quantities, "soil_aba_concentration")},
          aba_influence_coefficient_ip{get_ip(input_quantities, "aba_influence_coefficient")},
          max_b1_ip{get_ip(input_quantities, "max_b1")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")},
          b1_op{get_op(output_quantities, "b1")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_linear_and_aba_response"; }

   private:
    // Pointers to input quantities
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;
    const double* soil_aba_concentration_ip;
    const double* aba_influence_coefficient_ip;
    const double* max_b1_ip;

    // Pointers to output quantities
    double* StomataWS_op;
    double* b1_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_linear_and_aba_response::get_inputs()
{
    return {
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content",
        "soil_aba_concentration",
        "aba_influence_coefficient",
        "max_b1"};
}

string_vector stomata_water_stress_linear_and_aba_response::get_outputs()
{
    return {
        "StomataWS",
        "b1"};
}

void stomata_water_stress_linear_and_aba_response::do_operation() const
{
    // Collect inputs and make calculations
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
    double soil_aba_concentration = *soil_aba_concentration_ip;
    double aba_influence_coefficient = *aba_influence_coefficient_ip;
    double max_b1 = *max_b1_ip;

    double const slope = 1.0 / (soil_field_capacity - soil_wilting_point);
    double const intercept = 1.0 - soil_field_capacity * slope;
    double const aba_effect = exp(soil_aba_concentration / aba_influence_coefficient);  // dimensionless. A value in the interval (0, 1] that will reduce the slope of the Ball-Berry model.

    // Update the output quantity list
    update(StomataWS_op, std::min(std::max(slope * soil_water_content + intercept, 1e-10), 1.0));
    update(b1_op, max_b1 * aba_effect);
}

}  // namespace standardBML
#endif
