#ifndef BALL_BERRY_H
#define BALL_BERRY_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "ball_berry_gs.h"

namespace standardBML
{
/**
 * @class ball_berry
 *
 * @brief Uses `ball_berry_gs()` to evaluate the Ball-Berry model for stomatal
 * conductance.
 */
class ball_berry : public direct_module
{
   public:
    ball_berry(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          net_assimilation_rate{get_input(input_quantities, "net_assimilation_rate")},
          ambient_c{get_input(input_quantities, "Catm")},
          ambient_rh{get_input(input_quantities, "rh")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          gbw{get_input(input_quantities, "gbw")},
          leaf_temperature{get_input(input_quantities, "leaf_temperature")},
          ambient_air_temperature{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          cs_op{get_op(output_quantities, "cs")},
          hs_op{get_op(output_quantities, "hs")},
          leaf_stomatal_conductance_op{get_op(output_quantities, "leaf_stomatal_conductance")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ball_berry"; }

   private:
    // Pointers to input quantities
    double const& net_assimilation_rate;
    double const& ambient_c;
    double const& ambient_rh;
    double const& b0;
    double const& b1;
    double const& gbw;
    double const& leaf_temperature;
    double const& ambient_air_temperature;

    // Pointers to output quantities
    double* cs_op;
    double* hs_op;
    double* leaf_stomatal_conductance_op;

    // Main operation
    void do_operation() const;
};

string_vector ball_berry::get_inputs()
{
    return {
        "net_assimilation_rate",  // micromol / m^2 / s
        "Catm",                   // micromol / mol
        "rh",                     // Pa / Pa
        "b0",                     // mol / m^2 / s
        "b1",                     // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
        "gbw",                    // mol / m^2 / s
        "leaf_temperature",       // degrees C
        "temp"                    // degrees C
    };
}

string_vector ball_berry::get_outputs()
{
    return {
        "cs", // micromol / mol
        "hs", // dimensionless from Pa / Pa
        "leaf_stomatal_conductance"  // mmol / m^2 / s
    };
}

void ball_berry::do_operation() const
{
    stomata_outputs result = ball_berry_gs(
        net_assimilation_rate * 1e-6,
        ambient_c * 1e-6,
        ambient_rh,
        b0,
        b1,
        gbw,
        leaf_temperature,
        ambient_air_temperature);

    update(cs_op, result.cs);
    update(hs_op, result.hs);
    update(leaf_stomatal_conductance_op, result.gsw);
}

}  // namespace standardBML
#endif
