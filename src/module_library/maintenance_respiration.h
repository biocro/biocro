#ifndef MAINTENANCE_RESPIRATION_H
#define MAINTENANCE_RESPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "temperature_response_functions.h"  // for Q10_temprature_response

namespace standardBML
{
/**
 * @class maintenance_respiration
 *
 * @brief Calculates the change in plant organ biomasses due to maintenance respiration.
 *
 * The amount that each plant component respires is determined as a
 * percentage of its current biomass.
 * This ideas is from this paper:
 * (https://doi.org/10.1016/j.fcr.2010.07.007)
 */
class maintenance_respiration : public differential_module
{
   public:
    maintenance_respiration(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

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
          Leaf_op{get_op(output_quantities, "Leaf")},
          Stem_op{get_op(output_quantities, "Stem")},
          Root_op{get_op(output_quantities, "Root")},
          Rhizome_op{get_op(output_quantities, "Rhizome")},
          Grain_op{get_op(output_quantities, "Grain")},
          Shell_op{get_op(output_quantities, "Shell")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "maintenance_respiration"; }

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
    double* Leaf_op;
    double* Stem_op;
    double* Root_op;
    double* Rhizome_op;
    double* Grain_op;
    double* Shell_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector maintenance_respiration::get_inputs()
{
    return {
        "Leaf",       // Mg / ha
        "Stem",       // Mg / ha
        "Root",       // Mg / ha
        "Rhizome",    // Mg / ha
        "Shell",      // Mg / ha
        "Grain",      // Mg / ha
        "temp",       // degree C
        "mrc_leaf",   // kg / kg / hr
        "mrc_stem",   // kg / kg / hr
        "mrc_root",   // kg / kg / hr
        "mrc_grain"   // kg / kg / hr
    };
}

string_vector maintenance_respiration::get_outputs()
{
    return {
        "Leaf",     // Mg / ha
        "Stem",     // Mg / ha
        "Root",     // Mg / ha
        "Rhizome",  // Mg / ha
        "Shell",    // Mg / ha
        "Grain"     // Mg / ha
    };
}

void maintenance_respiration::do_operation() const
{
    double Tref = 25.0;  // reference temperature for the Q10 function

    double dLeaf = -Leaf * mrc_leaf * Q10_temperature_response(temp, Tref);  // Mg / ha

    double dStem = -Stem * mrc_stem * Q10_temperature_response(temp, Tref);

    double dRoot = -Root * mrc_root * Q10_temperature_response(temp, Tref);
    //assume rhizome has the same maintenance_respiration_coef as root
    double dRhizome = -Rhizome * mrc_root * Q10_temperature_response(temp, Tref);

    double dGrain = -Grain * mrc_grain * Q10_temperature_response(temp, Tref);
    //assume shell has the same maintenance_respiration_coef as grain
    double dShell = -Shell * mrc_grain * Q10_temperature_response(temp, Tref);  // Mg / ha

    update(Leaf_op, dLeaf);        // Mg / ha
    update(Stem_op, dStem);        // Mg / ha
    update(Root_op, dRoot);        // Mg / ha
    update(Rhizome_op, dRhizome);  // Mg / ha
    update(Grain_op, dGrain);      // Mg / ha
    update(Shell_op, dShell);      // Mg / ha
}

}  // namespace standardBML
#endif
