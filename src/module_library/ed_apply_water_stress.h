#ifndef ED_APPLY_WATER_STRESS_H
#define ED_APPLY_WATER_STRESS_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class ed_apply_stomatal_water_stress_via_conductance
 *
 * @brief Sets the water stress adjustment factors to apply the stomatal water
 * stress only to the stomatal conductance. Currently only intended for use by Ed.
 */
class ed_apply_stomatal_water_stress_via_conductance : public direct_module
{
   public:
    ed_apply_stomatal_water_stress_via_conductance(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("ed_apply_stomatal_water_stress_via_conductance"),
          // Get pointers to input quantities
          StomataWS_ip(get_ip(input_quantities, "StomataWS")),
          // Get pointers to output quantities
          conductance_adjustment_factor_WS_op(get_op(output_quantities, "conductance_adjustment_factor_WS")),
          assimilation_adjustment_factor_WS_op(get_op(output_quantities, "assimilation_adjustment_factor_WS"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* StomataWS_ip;
    // Pointers to output quantities
    double* conductance_adjustment_factor_WS_op;
    double* assimilation_adjustment_factor_WS_op;
    // Main operation
    void do_operation() const override;
};

string_vector ed_apply_stomatal_water_stress_via_conductance::get_inputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

string_vector ed_apply_stomatal_water_stress_via_conductance::get_outputs()
{
    return {
        "conductance_adjustment_factor_WS",  // dimensionless
        "assimilation_adjustment_factor_WS"  // dimensionless
    };
}

void ed_apply_stomatal_water_stress_via_conductance::do_operation() const
{
    // Update the output quantity list
    update(conductance_adjustment_factor_WS_op, *StomataWS_ip);
    update(assimilation_adjustment_factor_WS_op, 1.0);
}

/**
 * @class ed_apply_stomatal_water_stress_via_assimilation
 *
 * @brief Sets the water stress adjustment factors to apply the stomatal water
 * stress only to the stomatal conductance.
 */
class ed_apply_stomatal_water_stress_via_assimilation : public direct_module
{
   public:
    ed_apply_stomatal_water_stress_via_assimilation(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("ed_apply_stomatal_water_stress_via_assimilation"),
          // Get pointers to input quantities
          StomataWS_ip(get_ip(input_quantities, "StomataWS")),
          // Get pointers to output quantities
          conductance_adjustment_factor_WS_op(get_op(output_quantities, "conductance_adjustment_factor_WS")),
          assimilation_adjustment_factor_WS_op(get_op(output_quantities, "assimilation_adjustment_factor_WS"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* StomataWS_ip;
    // Pointers to output quantities
    double* conductance_adjustment_factor_WS_op;
    double* assimilation_adjustment_factor_WS_op;
    // Main operation
    void do_operation() const override;
};

string_vector ed_apply_stomatal_water_stress_via_assimilation::get_inputs()
{
    return {
        "StomataWS"  // dimensionless
    };
}

string_vector ed_apply_stomatal_water_stress_via_assimilation::get_outputs()
{
    return {
        "conductance_adjustment_factor_WS",  // dimensionless
        "assimilation_adjustment_factor_WS"  // dimensionless
    };
}

void ed_apply_stomatal_water_stress_via_assimilation::do_operation() const
{
    // Update the output quantity list
    update(conductance_adjustment_factor_WS_op, 1.0);
    update(assimilation_adjustment_factor_WS_op, *StomataWS_ip);
}

#endif
