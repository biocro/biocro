#ifndef ED_STOMATA_WATER_STRESS_LINEAR_H
#define ED_STOMATA_WATER_STRESS_LINEAR_H

#include "../modules.h"

/**
 * @class ed_stomata_water_stress_linear
 * 
 * @brief Determine the stomata water stress factor.
 * If the soil water content is below the wilting point,
 * the stress factor takes its minimum value. If the water
 * content is at or above the field capacity, the stress
 * factor reaches its maximum of 1. Between the wilting
 * point and the field capacity, the stress factor is
 * determined by a straight line.
 */
class ed_stomata_water_stress_linear : public SteadyModule
{
   public:
    ed_stomata_water_stress_linear(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_stomata_water_stress_linear"),
          // Get pointers to input parameters
          soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
          soil_wilting_point_ip(get_ip(input_parameters, "soil_wilting_point")),
          soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
          StomataWS_min_ip(get_ip(input_parameters, "StomataWS_min")),
          // Get pointers to output parameters
          StomataWS_op(get_op(output_parameters, "StomataWS"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;
    const double* StomataWS_min_ip;
    // Pointers to output parameters
    double* StomataWS_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> ed_stomata_water_stress_linear::get_inputs()
{
    return {
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content",
        "StomataWS_min"};
}

std::vector<std::string> ed_stomata_water_stress_linear::get_outputs()
{
    return {
        "StomataWS"};
}

void ed_stomata_water_stress_linear::do_operation() const
{
    // Collect inputs and make calculations
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
    double StomataWS_min = *StomataWS_min_ip;

    double StomataWS;
    if (soil_water_content < soil_wilting_point) {
        StomataWS = StomataWS_min;
    } else if (soil_water_content < soil_field_capacity) {
        const double slope = (1.0 - StomataWS_min) / (soil_field_capacity - soil_wilting_point);
        const double intercept = 1.0 - soil_field_capacity * slope;
        StomataWS = slope * soil_water_content + intercept;
    } else {
        StomataWS = 1.0;
    }

    // Update the output parameter list
    update(StomataWS_op, StomataWS);
}

#endif
