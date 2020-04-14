#ifndef ED_APPLY_WATER_STRESS_H
#define ED_APPLY_WATER_STRESS_H

#include "../modules.h"

/**
 * @class ed_apply_stomatal_water_stress_via_conductance
 * 
 * @brief Sets the water stress adjustment factors to apply the stomatal water
 * stress only to the stomatal conductance. Currently only intended for use by Ed.
 */
class ed_apply_stomatal_water_stress_via_conductance : public SteadyModule
{
   public:
    ed_apply_stomatal_water_stress_via_conductance(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_apply_stomatal_water_stress_via_conductance"),
          // Get pointers to input parameters
          StomataWS_ip(get_ip(input_parameters, "StomataWS")),
          // Get pointers to output parameters
          conductance_adjustment_factor_WS_op(get_op(output_parameters, "conductance_adjustment_factor_WS")),
          assimilation_adjustment_factor_WS_op(get_op(output_parameters, "assimilation_adjustment_factor_WS"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* StomataWS_ip;
    // Pointers to output parameters
    double* conductance_adjustment_factor_WS_op;
    double* assimilation_adjustment_factor_WS_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_apply_stomatal_water_stress_via_conductance::get_inputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

std::vector<std::string> ed_apply_stomatal_water_stress_via_conductance::get_outputs()
{
    return {
        "conductance_adjustment_factor_WS",  // dimensionless
        "assimilation_adjustment_factor_WS"  // dimensionless
    };
}

void ed_apply_stomatal_water_stress_via_conductance::do_operation() const
{
    // Update the output parameter list
    update(conductance_adjustment_factor_WS_op, *StomataWS_ip);
    update(assimilation_adjustment_factor_WS_op, 1.0);
}

/**
 * @class ed_apply_stomatal_water_stress_via_assimilation
 * 
 * @brief Sets the water stress adjustment factors to apply the stomatal water
 * stress only to the stomatal conductance.
 */
class ed_apply_stomatal_water_stress_via_assimilation : public SteadyModule
{
   public:
    ed_apply_stomatal_water_stress_via_assimilation(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_apply_stomatal_water_stress_via_assimilation"),
          // Get pointers to input parameters
          StomataWS_ip(get_ip(input_parameters, "StomataWS")),
          // Get pointers to output parameters
          conductance_adjustment_factor_WS_op(get_op(output_parameters, "conductance_adjustment_factor_WS")),
          assimilation_adjustment_factor_WS_op(get_op(output_parameters, "assimilation_adjustment_factor_WS"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* StomataWS_ip;
    // Pointers to output parameters
    double* conductance_adjustment_factor_WS_op;
    double* assimilation_adjustment_factor_WS_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_apply_stomatal_water_stress_via_assimilation::get_inputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

std::vector<std::string> ed_apply_stomatal_water_stress_via_assimilation::get_outputs()
{
    return {
        "conductance_adjustment_factor_WS",  // dimensionless
        "assimilation_adjustment_factor_WS"  // dimensionless
    };
}

void ed_apply_stomatal_water_stress_via_assimilation::do_operation() const
{
    // Update the output parameter list
    update(conductance_adjustment_factor_WS_op, 1.0);
    update(assimilation_adjustment_factor_WS_op, *StomataWS_ip);
}

#endif
