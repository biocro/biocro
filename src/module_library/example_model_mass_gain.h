#ifndef EXAMPLE_MODEL_MASS_GAIN_H
#define EXAMPLE_MODEL_MASS_GAIN_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class example_model_mass_gain
 *
 * @brief An example for the BioCro II manuscript
 *
 * ### Model overview
 *
 * Model mass gain as a linear function of other quantities.
 *
 * It is not meant to be biologically realistic.
 *
 */
class example_model_mass_gain : public direct_module
{
   public:
    example_model_mass_gain(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("example_model_mass_grain"),

          // Get pointers to input quantities
          Q(get_input(input_quantities, "Q")),
          alpha_rue(get_input(input_quantities, "alpha_rue")),
          SLA(get_input(input_quantities, "SLA")),
          C_conv(get_input(input_quantities, "C_conv")),
          Leaf(get_input(input_quantities, "Leaf")),

          // Get pointers to output quantities
          mass_gain_op(get_op(output_quantities, "mass_gain"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& Q;
    double const& alpha_rue;
    double const& SLA;
    double const& C_conv;
    double const& Leaf;

    // Pointers to output quantities
    double* mass_gain_op;

    // Main operation
    void do_operation() const;
};

string_vector example_model_mass_gain::get_inputs()
{
    return {
        "Q",            // mol / m^2 / s
        "alpha_rue",    // mol / mol
        "SLA",          // m^2 / kg
        "C_conv",       // kg / mol
        "Leaf"          // kg
    };
}

string_vector example_model_mass_gain::get_outputs()
{
    return {
        "mass_gain"  // kg / s
    };
}

void example_model_mass_gain::do_operation() const
{
    double const assimilation = Q * alpha_rue;
    double const mass_gain = assimilation * Leaf * SLA * C_conv;

    update(mass_gain_op, mass_gain);
}

#endif
