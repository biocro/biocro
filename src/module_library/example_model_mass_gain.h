#ifndef EXAMPLE_MODEL_MASS_GAIN_H
#define EXAMPLE_MODEL_MASS_GAIN_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class example_model_mass_gain
 *
 *  @brief An example for the BioCro II manuscript
 *
 *  ### Model overview
 *
 *  Here we model mass gain as a linear function of other quantities. This model
 *  is not meant to be biologically realistic.
 *
 *  In this model, the carbon assimilation rate per unit leaf area (`A_leaf`) is
 *  assumed to be directly proportional to the incident photosynthetically
 *  active photon flux density (`Q`):
 *
 *  > `A_leaf = alpha * Q` (1)
 *
 *  where the proportionality constant `alpha` is the radiation use efficiency.
 *
 *  The total canopy assimilation rate can then be determined by the product of
 *  `A_leaf` and the total leaf area. In turn, leaf area can be calculated from
 *  the total leaf mass (`m_leaf`) and the specific leaf area (`SLA`), which
 *  expresses the ratio of leaf area to mass.
 *
 *  Finally, the carbon assimilation rate can be converted to a rate of mass
 *  increase by assuming a particular value for `C_conv`, the conversion ratio
 *  of carbon to mass for the plant.
 *
 *  Combining these considerations, we can relate the rate of mass gain to the
 *  leaf-level assimilation rate as follows:
 *
 *  > `mass_gain = A_leaf * m_leaf * SLA * C_conv` (2)
 *
 */
class example_model_mass_gain : public direct_module
{
   public:
    example_model_mass_gain(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Q{get_input(input_quantities, "Q")},
          alpha_rue{get_input(input_quantities, "alpha_rue")},
          SLA{get_input(input_quantities, "SLA")},
          C_conv{get_input(input_quantities, "C_conv")},
          Leaf{get_input(input_quantities, "Leaf")},

          // Get pointers to output quantities
          mass_gain_op{get_op(output_quantities, "mass_gain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "example_model_mass_gain"; }

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
        "Q",          // mol / m^2 / s
        "alpha_rue",  // mol / mol
        "SLA",        // m^2 / kg
        "C_conv",     // kg / mol
        "Leaf"        // kg
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

}  // namespace standardBML
#endif
