#ifndef STANDARDBML_CUMULATIVE_CARBON_DYNAMICS_H
#define STANDARDBML_CUMULATIVE_CARBON_DYNAMICS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class cumulative_carbon_dynamics
 *
 * @brief Enables calculations of cumulative carbon dynamics.
 *
 * Cumulative canopy gas exchange will be included in the simulation output as
 * differential quantities called:
 * - ``'canopy_assimilation'``: The cumulative net CO2 assimilation
 * - ``'canopy_gross_assimilation'``: The cumulative gross CO2 assimilation
 * - ``'canopy_non_photorespiratory_CO2_release'``: The cumulative CO2 lost to
 *   non-photorespiratory CO2 release
 * - ``'canopy_photorespiration'``: The cumulative CO2 lost to photorespiration
 *
 * Cumulative maintenance respiration will be included in the simulation output
 * as differential quantities called ``'Leaf_mr'``, ``'Stem_mr'``, etc, where
 * "mr" standard for "maintenance respiration."
 *
 * Cumulative growth respiration will be included in the simulation output
 * as differential quantities called ``'Stem_gr'``, ``'Root_gr'``, etc, where
 * "gr" standard for "growth respiration." There is also a cumulative
 * whole-plant growth respiration called ``'whole_plant_growth_respiration'``.
 * This is not the sum of the individual tissue rates, but rather a separate
 * carbon sink that is applied before calculating growth respiration losses from
 * each tissue.
 */
class cumulative_carbon_dynamics : public differential_module
{
   public:
    cumulative_carbon_dynamics(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          canopy_assimilation_rate{get_input(input_quantities, "canopy_assimilation_rate")},
          canopy_gross_assimilation_rate{get_input(input_quantities, "canopy_gross_assimilation_rate")},
          canopy_photorespiration_rate{get_input(input_quantities, "canopy_photorespiration_rate")},
          canopy_RL_rate{get_input(input_quantities, "canopy_non_photorespiratory_CO2_release_rate")},
          Grain_gr_rate{get_input(input_quantities, "Grain_gr_rate")},
          Grain_mr_rate{get_input(input_quantities, "Grain_mr_rate")},
          Leaf_gr_rate{get_input(input_quantities, "Leaf_gr_rate")},
          Leaf_mr_rate{get_input(input_quantities, "Leaf_mr_rate")},
          Leaf_WS_loss_rate{get_input(input_quantities, "Leaf_WS_loss_rate")},
          Rhizome_gr_rate{get_input(input_quantities, "Rhizome_gr_rate")},
          Rhizome_mr_rate{get_input(input_quantities, "Rhizome_mr_rate")},
          Root_gr_rate{get_input(input_quantities, "Root_gr_rate")},
          Root_mr_rate{get_input(input_quantities, "Root_mr_rate")},
          Shell_gr_rate{get_input(input_quantities, "Shell_gr_rate")},
          Shell_mr_rate{get_input(input_quantities, "Shell_mr_rate")},
          Stem_gr_rate{get_input(input_quantities, "Stem_gr_rate")},
          Stem_mr_rate{get_input(input_quantities, "Stem_mr_rate")},
          whole_plant_growth_respiration_rate{get_input(input_quantities, "whole_plant_growth_respiration_rate")},

          // Get pointers to output quantities
          canopy_assimilation_op{get_op(output_quantities, "canopy_assimilation")},
          canopy_gross_assimilation_op{get_op(output_quantities, "canopy_gross_assimilation")},
          canopy_photorespiration_op{get_op(output_quantities, "canopy_photorespiration")},
          canopy_RL_op{get_op(output_quantities, "canopy_non_photorespiratory_CO2_release")},
          Grain_gr_op{get_op(output_quantities, "Grain_gr")},
          Grain_mr_op{get_op(output_quantities, "Grain_mr")},
          Leaf_gr_op{get_op(output_quantities, "Leaf_gr")},
          Leaf_mr_op{get_op(output_quantities, "Leaf_mr")},
          Leaf_WS_loss_op{get_op(output_quantities, "Leaf_WS_loss")},
          Rhizome_gr_op{get_op(output_quantities, "Rhizome_gr")},
          Rhizome_mr_op{get_op(output_quantities, "Rhizome_mr")},
          Root_gr_op{get_op(output_quantities, "Root_gr")},
          Root_mr_op{get_op(output_quantities, "Root_mr")},
          Shell_gr_op{get_op(output_quantities, "Shell_gr")},
          Shell_mr_op{get_op(output_quantities, "Shell_mr")},
          Stem_gr_op{get_op(output_quantities, "Stem_gr")},
          Stem_mr_op{get_op(output_quantities, "Stem_mr")},
          whole_plant_growth_respiration_op{get_op(output_quantities, "whole_plant_growth_respiration")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_carbon_dynamics"; }

   private:
    // References to input quantities
    double const& canopy_assimilation_rate;
    double const& canopy_gross_assimilation_rate;
    double const& canopy_photorespiration_rate;
    double const& canopy_RL_rate;
    double const& Grain_gr_rate;
    double const& Grain_mr_rate;
    double const& Leaf_gr_rate;
    double const& Leaf_mr_rate;
    double const& Leaf_WS_loss_rate;
    double const& Rhizome_gr_rate;
    double const& Rhizome_mr_rate;
    double const& Root_gr_rate;
    double const& Root_mr_rate;
    double const& Shell_gr_rate;
    double const& Shell_mr_rate;
    double const& Stem_gr_rate;
    double const& Stem_mr_rate;
    double const& whole_plant_growth_respiration_rate;

    // Pointers to output quantities
    double* canopy_assimilation_op;
    double* canopy_gross_assimilation_op;
    double* canopy_photorespiration_op;
    double* canopy_RL_op;
    double* Grain_gr_op;
    double* Grain_mr_op;
    double* Leaf_gr_op;
    double* Leaf_mr_op;
    double* Leaf_WS_loss_op;
    double* Rhizome_gr_op;
    double* Rhizome_mr_op;
    double* Root_gr_op;
    double* Root_mr_op;
    double* Shell_gr_op;
    double* Shell_mr_op;
    double* Stem_gr_op;
    double* Stem_mr_op;
    double* whole_plant_growth_respiration_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_carbon_dynamics::get_inputs()
{
    return {
        "canopy_assimilation_rate",                      // Mg / ha / hr
        "canopy_gross_assimilation_rate",                // Mg / ha / hr
        "canopy_non_photorespiratory_CO2_release_rate",  // Mg / ha / hr
        "canopy_photorespiration_rate",                  // Mg / ha / hr
        "Grain_gr_rate",                                 // Mg / ha / hr
        "Grain_mr_rate",                                 // Mg / ha / hr
        "Leaf_gr_rate",                                  // Mg / ha / hr
        "Leaf_mr_rate",                                  // Mg / ha / hr
        "Leaf_WS_loss_rate",                             // Mg / ha / hr
        "Rhizome_gr_rate",                               // Mg / ha / hr
        "Rhizome_mr_rate",                               // Mg / ha / hr
        "Root_gr_rate",                                  // Mg / ha / hr
        "Root_mr_rate",                                  // Mg / ha / hr
        "Shell_gr_rate",                                 // Mg / ha / hr
        "Shell_mr_rate",                                 // Mg / ha / hr
        "Stem_gr_rate",                                  // Mg / ha / hr
        "Stem_mr_rate",                                  // Mg / ha / hr
        "whole_plant_growth_respiration_rate"            // Mg / ha / hr
    };
}

string_vector cumulative_carbon_dynamics::get_outputs()
{
    return {
        "canopy_assimilation",                      // Mg / ha / hr
        "canopy_gross_assimilation",                // Mg / ha / hr
        "canopy_non_photorespiratory_CO2_release",  // Mg / ha / hr
        "canopy_photorespiration",                  // Mg / ha / hr
        "Grain_gr",                                 // Mg / ha / hr
        "Grain_mr",                                 // Mg / ha / hr
        "Leaf_gr",                                  // Mg / ha / hr
        "Leaf_mr",                                  // Mg / ha / hr
        "Leaf_WS_loss",                             // Mg / ha / hr
        "Rhizome_gr",                               // Mg / ha / hr
        "Rhizome_mr",                               // Mg / ha / hr
        "Root_gr",                                  // Mg / ha / hr
        "Root_mr",                                  // Mg / ha / hr
        "Shell_gr",                                 // Mg / ha / hr
        "Shell_mr",                                 // Mg / ha / hr
        "Stem_gr",                                  // Mg / ha / hr
        "Stem_mr",                                  // Mg / ha / hr
        "whole_plant_growth_respiration"            // Mg / ha / hr
    };
}

void cumulative_carbon_dynamics::do_operation() const
{
    update(canopy_assimilation_op, canopy_assimilation_rate);
    update(canopy_gross_assimilation_op, canopy_gross_assimilation_rate);
    update(canopy_photorespiration_op, canopy_photorespiration_rate);
    update(canopy_RL_op, canopy_RL_rate);
    update(Grain_gr_op, Grain_gr_rate);
    update(Grain_mr_op, Grain_mr_rate);
    update(Leaf_gr_op, Leaf_gr_rate);
    update(Leaf_mr_op, Leaf_mr_rate);
    update(Leaf_WS_loss_op, Leaf_WS_loss_rate);
    update(Rhizome_gr_op, Rhizome_gr_rate);
    update(Rhizome_mr_op, Rhizome_mr_rate);
    update(Root_gr_op, Root_gr_rate);
    update(Root_mr_op, Root_mr_rate);
    update(Shell_gr_op, Shell_gr_rate);
    update(Shell_mr_op, Shell_mr_rate);
    update(Stem_gr_op, Stem_gr_rate);
    update(Stem_mr_op, Stem_mr_rate);
    update(whole_plant_growth_respiration_op, whole_plant_growth_respiration_rate);
}

}  // namespace standardBML
#endif
