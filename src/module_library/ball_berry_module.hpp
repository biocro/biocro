#ifndef BALL_BERRY_MODULE_H
#define BALL_BERRY_MODULE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "ball_berry.hpp"

namespace standardBML
{
class ball_berry_module : public direct_module
{
   public:
    ball_berry_module(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          net_assimilation_rate{get_input(input_quantities, "net_assimilation_rate")},
          atmospheric_co2_concentration{get_input(input_quantities, "atmospheric_co2_concentration")},
          rh{get_input(input_quantities, "rh")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},

          // Get pointers to output quantities
          leaf_stomatal_conductance_op{get_op(output_quantities, "leaf_stomatal_conductance")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ball_berry_module"; }

   private:
    // Pointers to input quantities
    double const& net_assimilation_rate;
    double const& atmospheric_co2_concentration;
    double const& rh;
    double const& b0;
    double const& b1;

    // Pointers to output quantities
    double* leaf_stomatal_conductance_op;

    // Main operation
    void do_operation() const;
};

string_vector ball_berry_module::get_inputs()
{
    return {
        "net_assimilation_rate",          // mol / m^2 / s
        "atmospheric_co2_concentration",  // mol / mol
        "rh",                             // Pa / Pa
        "b0",                             // mol / m^2 / s
        "b1"                              // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    };
}

string_vector ball_berry_module::get_outputs()
{
    return {
        "leaf_stomatal_conductance"  // mmol / m^2 / s
    };
}

void ball_berry_module::do_operation() const
{
    // Collect input quantities and make calculations
    double stomatal_conductance = ball_berry(
        net_assimilation_rate,
        atmospheric_co2_concentration,
        rh,
        b0,
        b1);

    // Update the output quantity list
    update(leaf_stomatal_conductance_op, stomatal_conductance);
}

}  // namespace standardBML
#endif
