#ifndef LEAF_WATER_STRESS_EXPONENTIAL_H
#define LEAF_WATER_STRESS_EXPONENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  // For calculation_constants::eps_zero

namespace standardBML
{
class leaf_water_stress_exponential : public direct_module
{
   public:
    leaf_water_stress_exponential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          phi2_ip{get_ip(input_quantities, "phi2")},

          // Get pointers to output quantities
          LeafWS_op{get_op(output_quantities, "LeafWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_water_stress_exponential"; }

   private:
    // Pointers to input quantities
    const double* soil_water_content_ip;
    const double* soil_field_capacity_ip;
    const double* phi2_ip;

    // Pointers to output quantities
    double* LeafWS_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_water_stress_exponential::get_inputs()
{
    return {
        "soil_field_capacity",  // dimensionless (from m^3 / m^3)
        "soil_water_content",   // dimensionless (from m^3 / m^3)
        "phi2"                  // dimensionless
    };
}

string_vector leaf_water_stress_exponential::get_outputs()
{
    return {
        "LeafWS"  // dimensionless
    };
}

void leaf_water_stress_exponential::do_operation() const
{
    // Calculate the water stress factor, ensuring it lies on the interval (0,1]
    double const base = (*soil_water_content_ip) / (*soil_field_capacity_ip);
    double const leaf_water_stress_raw = pow(base, *phi2_ip);
    double const leaf_water_stress = std::min(
        std::max(
            leaf_water_stress_raw,
            calculation_constants::eps_zero),
        1.0);

    update(LeafWS_op, leaf_water_stress);
}

}  // namespace standardBML
#endif
