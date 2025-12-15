#ifndef MAINTENANCE_RESPIRATION_H
#define MAINTENANCE_RESPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class maintenance_respiration
 *
 * @brief Includes maintenance respiration losses in the rate of change of each
 * organ biomass.
 *
 * This module is intended to be used along with the
 * `maintenance_respiration_calculator` module; see that module for more
 * information.
 */
class maintenance_respiration : public differential_module
{
   public:
    maintenance_respiration(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          Grain_mr_rate{get_input(input_quantities, "Grain_mr_rate")},
          Leaf_mr_rate{get_input(input_quantities, "Leaf_mr_rate")},
          Rhizome_mr_rate{get_input(input_quantities, "Rhizome_mr_rate")},
          Root_mr_rate{get_input(input_quantities, "Root_mr_rate")},
          Shell_mr_rate{get_input(input_quantities, "Shell_mr_rate")},
          Stem_mr_rate{get_input(input_quantities, "Stem_mr_rate")},

          // Get pointers to output quantities
          Grain_op{get_op(output_quantities, "Grain")},
          Leaf_op{get_op(output_quantities, "Leaf")},
          Rhizome_op{get_op(output_quantities, "Rhizome")},
          Root_op{get_op(output_quantities, "Root")},
          Shell_op{get_op(output_quantities, "Shell")},
          Stem_op{get_op(output_quantities, "Stem")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "maintenance_respiration"; }

   private:
    // References to input quantities
    const double& Grain_mr_rate;
    const double& Leaf_mr_rate;
    const double& Rhizome_mr_rate;
    const double& Root_mr_rate;
    const double& Shell_mr_rate;
    const double& Stem_mr_rate;

    // Pointers to output quantities
    double* Grain_op;
    double* Leaf_op;
    double* Rhizome_op;
    double* Root_op;
    double* Shell_op;
    double* Stem_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector maintenance_respiration::get_inputs()
{
    return {
        "Grain_mr_rate",    // Mg / ha / hr
        "Leaf_mr_rate",     // Mg / ha / hr
        "Rhizome_mr_rate",  // Mg / ha / hr
        "Root_mr_rate",     // Mg / ha / hr
        "Shell_mr_rate",    // Mg / ha / hr
        "Stem_mr_rate"      // Mg / ha / hr
    };
}

string_vector maintenance_respiration::get_outputs()
{
    return {
        "Grain",    // Mg / ha / hr
        "Leaf",     // Mg / ha / hr
        "Rhizome",  // Mg / ha / hr
        "Root",     // Mg / ha / hr
        "Shell",    // Mg / ha / hr
        "Stem"      // Mg / ha / hr
    };
}

void maintenance_respiration::do_operation() const
{
    update(Grain_op, -Grain_mr_rate);      // Mg / ha / hr
    update(Leaf_op, -Leaf_mr_rate);        // Mg / ha / hr
    update(Rhizome_op, -Rhizome_mr_rate);  // Mg / ha / hr
    update(Root_op, -Root_mr_rate);        // Mg / ha / hr
    update(Shell_op, -Shell_mr_rate);      // Mg / ha / hr
    update(Stem_op, -Stem_mr_rate);        // Mg / ha / hr
}

}  // namespace standardBML
#endif
