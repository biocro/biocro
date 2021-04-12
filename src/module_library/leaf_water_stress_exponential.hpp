#ifndef LEAF_WATER_STRESS_EXPONENTIAL_H
#define LEAF_WATER_STRESS_EXPONENTIAL_H

#include "../modules.h"
#include "../constants.h"  // For calculation_constants::eps_zero

class leaf_water_stress_exponential : public SteadyModule
{
   public:
    leaf_water_stress_exponential(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("leaf_water_stress_exponential"),

          // Get pointers to input parameters
          soil_water_content_ip(get_ip(input_parameters, "soil_water_content")),
          soil_field_capacity_ip(get_ip(input_parameters, "soil_field_capacity")),
          phi2_ip(get_ip(input_parameters, "phi2")),

          // Get pointers to output parameters
          LeafWS_op(get_op(output_parameters, "LeafWS"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* soil_water_content_ip;
    const double* soil_field_capacity_ip;
    const double* phi2_ip;

    // Pointers to output parameters
    double* LeafWS_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> leaf_water_stress_exponential::get_inputs()
{
    return {
        "soil_field_capacity",  // dimensionless (from m^3 / m^3)
        "soil_water_content",   // dimensionless (from m^3 / m^3)
        "phi2"                  // dimensionless
    };
}

std::vector<std::string> leaf_water_stress_exponential::get_outputs()
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

#endif
