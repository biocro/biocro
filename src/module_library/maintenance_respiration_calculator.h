#ifndef MAINTENANCE_RESPIRATION_CALCULATOR_H
#define MAINTENANCE_RESPIRATION_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "temperature_response_functions.h"  // for Q10_temprature_response

namespace standardBML
{
/**
 * @class maintenance_respiration_calculator
 *
 * @brief Calculates the rate of change in plant organ biomasses due to
 * maintenance respiration; these are referred to as "maintenance respiration
 * rates" (mrr).
 *
 * This module is intended to be used along with the `maintenance_respiration`
 * module.
 *
 * The amount that each plant component respires is determined as a percentage
 * of its current biomass. This ideas is from this paper:
 * https://doi.org/10.1016/j.fcr.2010.07.007
 */
class maintenance_respiration_calculator : public direct_module
{
   public:
    maintenance_respiration_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          Leaf{get_input(input_quantities, "Leaf")},
          Stem{get_input(input_quantities, "Stem")},
          Root{get_input(input_quantities, "Root")},
          Rhizome{get_input(input_quantities, "Rhizome")},
          Grain{get_input(input_quantities, "Grain")},
          Shell{get_input(input_quantities, "Shell")},
          temp{get_input(input_quantities, "temp")},
          mrc_leaf{get_input(input_quantities, "mrc_leaf")},
          mrc_stem{get_input(input_quantities, "mrc_stem")},
          mrc_root{get_input(input_quantities, "mrc_root")},
          mrc_grain{get_input(input_quantities, "mrc_grain")},

          // Get pointers to output quantities
          Leaf_mrr_op{get_op(output_quantities, "Leaf_mrr")},
          Stem_mrr_op{get_op(output_quantities, "Stem_mrr")},
          Root_mrr_op{get_op(output_quantities, "Root_mrr")},
          Rhizome_mrr_op{get_op(output_quantities, "Rhizome_mrr")},
          Grain_mrr_op{get_op(output_quantities, "Grain_mrr")},
          Shell_mrr_op{get_op(output_quantities, "Shell_mrr")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "maintenance_respiration_calculator"; }

   private:
    // References to input quantities
    const double& Leaf;
    const double& Stem;
    const double& Root;
    const double& Rhizome;
    const double& Grain;
    const double& Shell;
    const double& temp;
    const double& mrc_leaf;
    const double& mrc_stem;
    const double& mrc_root;
    const double& mrc_grain;

    // Pointers to output quantities
    double* Leaf_mrr_op;
    double* Stem_mrr_op;
    double* Root_mrr_op;
    double* Rhizome_mrr_op;
    double* Grain_mrr_op;
    double* Shell_mrr_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector maintenance_respiration_calculator::get_inputs()
{
    return {
        "Leaf",      // Mg / ha
        "Stem",      // Mg / ha
        "Root",      // Mg / ha
        "Rhizome",   // Mg / ha
        "Shell",     // Mg / ha
        "Grain",     // Mg / ha
        "temp",      // degree C
        "mrc_leaf",  // kg / kg / hr
        "mrc_stem",  // kg / kg / hr
        "mrc_root",  // kg / kg / hr
        "mrc_grain"  // kg / kg / hr
    };
}

string_vector maintenance_respiration_calculator::get_outputs()
{
    return {
        "Leaf_mrr",     // Mg / ha / hr
        "Stem_mrr",     // Mg / ha / hr
        "Root_mrr",     // Mg / ha / hr
        "Rhizome_mrr",  // Mg / ha / hr
        "Shell_mrr",    // Mg / ha / hr
        "Grain_mrr"     // Mg / ha / hr
    };
}

void maintenance_respiration_calculator::do_operation() const
{
    // Define the reference temperature for the Q10 function
    double constexpr Tref = 25.0;  // degrees C

    // Calculate each maintenance respiration rate (mrr). We assume that
    // the rhizome has the same maintenance respiration coefficient (mrc) as
    // the root, and that the shell has the same mrc as the grain.
    double const Leaf_mrr = Leaf * mrc_leaf * Q10_temperature_response(temp, Tref);        // Mg / ha / hr
    double const Stem_mrr = Stem * mrc_stem * Q10_temperature_response(temp, Tref);        // Mg / ha / hr
    double const Root_mrr = Root * mrc_root * Q10_temperature_response(temp, Tref);        // Mg / ha / hr
    double const Rhizome_mrr = Rhizome * mrc_root * Q10_temperature_response(temp, Tref);  // Mg / ha / hr
    double const Grain_mrr = Grain * mrc_grain * Q10_temperature_response(temp, Tref);     // Mg / ha / hr
    double const Shell_mrr = Shell * mrc_grain * Q10_temperature_response(temp, Tref);     // Mg / ha / hr

    update(Leaf_mrr_op, Leaf_mrr);        // Mg / ha
    update(Stem_mrr_op, Stem_mrr);        // Mg / ha
    update(Root_mrr_op, Root_mrr);        // Mg / ha
    update(Rhizome_mrr_op, Rhizome_mrr);  // Mg / ha
    update(Grain_mrr_op, Grain_mrr);      // Mg / ha
    update(Shell_mrr_op, Shell_mrr);      // Mg / ha
}

}  // namespace standardBML
#endif
