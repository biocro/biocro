#ifndef TOTAL_GROWTH_AND_MAINTENANCE_RESPIRATION_H
#define TOTAL_GROWTH_AND_MAINTENANCE_RESPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class total_growth_and_maintenance_respiration
 *
 *  @brief Calculates the total growth and maintenance respiration and returns
 *  them as ``'total_growth_respiration'`` and
 *  ``'total_maintenance_respiration'``, respectively.
 *
 *  This module is intended to be used along with the
 *  `BioCro:cumulative_carbon_dynamics` module, which is necessary to calculate
 *  all of its input quantities.
 */
class total_growth_and_maintenance_respiration : public direct_module
{
   public:
    total_growth_and_maintenance_respiration(
        const state_map& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input parameters
          Grain_gr{get_input(input_quantities, "Grain_gr")},
          Grain_mr{get_input(input_quantities, "Grain_mr")},
          Leaf_gr{get_input(input_quantities, "Leaf_gr")},
          Leaf_mr{get_input(input_quantities, "Leaf_mr")},
          Leaf_WS_loss{get_input(input_quantities, "Leaf_WS_loss")},
          Rhizome_gr{get_input(input_quantities, "Rhizome_gr")},
          Rhizome_mr{get_input(input_quantities, "Rhizome_mr")},
          Root_gr{get_input(input_quantities, "Root_gr")},
          Root_mr{get_input(input_quantities, "Root_mr")},
          Shell_gr{get_input(input_quantities, "Shell_gr")},
          Shell_mr{get_input(input_quantities, "Shell_mr")},
          Stem_gr{get_input(input_quantities, "Stem_gr")},
          Stem_mr{get_input(input_quantities, "Stem_mr")},
          whole_plant_growth_respiration{get_input(input_quantities, "whole_plant_growth_respiration")},

          // Get pointers to output parameters
          total_growth_respiration_op{get_op(output_quantities, "total_growth_respiration")},
          total_maintenance_respiration_op{get_op(output_quantities, "total_maintenance_respiration")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "total_growth_and_maintenance_respiration"; }

   private:
    // Pointers to input parameters
    const double& Grain_gr;
    const double& Grain_mr;
    const double& Leaf_gr;
    const double& Leaf_mr;
    const double& Leaf_WS_loss;
    const double& Rhizome_gr;
    const double& Rhizome_mr;
    const double& Root_gr;
    const double& Root_mr;
    const double& Shell_gr;
    const double& Shell_mr;
    const double& Stem_gr;
    const double& Stem_mr;
    const double& whole_plant_growth_respiration;

    // Pointers to output parameters
    double* total_growth_respiration_op;
    double* total_maintenance_respiration_op;

    // Main operation
    void do_operation() const;
};

string_vector total_growth_and_maintenance_respiration::get_inputs()
{
    return {
        "Grain_gr",                       // Mg / ha
        "Grain_mr",                       // Mg / ha
        "Leaf_gr",                        // Mg / ha
        "Leaf_mr",                        // Mg / ha
        "Leaf_WS_loss",                   // Mg / ha
        "Rhizome_gr",                     // Mg / ha
        "Rhizome_mr",                     // Mg / ha
        "Root_gr",                        // Mg / ha
        "Root_mr",                        // Mg / ha
        "Shell_gr",                       // Mg / ha
        "Shell_mr",                       // Mg / ha
        "Stem_gr",                        // Mg / ha
        "Stem_mr",                        // Mg / ha
        "whole_plant_growth_respiration"  // Mg / ha
    };
}

string_vector total_growth_and_maintenance_respiration::get_outputs()
{
    return {
        "total_growth_respiration",      // Mg / ha
        "total_maintenance_respiration"  // Mg / ha
    };
}

void total_growth_and_maintenance_respiration::do_operation() const
{
    update(total_growth_respiration_op,
           Grain_gr + Leaf_gr + Leaf_WS_loss + Rhizome_gr + Root_gr +
               Shell_gr + Stem_gr + whole_plant_growth_respiration);

    update(total_maintenance_respiration_op,
           Grain_mr + Leaf_mr + Rhizome_mr + Root_mr + Shell_mr + Stem_mr);
}

}  // namespace standardBML
#endif
