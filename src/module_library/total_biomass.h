#ifndef TOTAL_BIOMASS_H
#define TOTAL_BIOMASS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class total_biomass
 *
 *  @brief Calculates the total intact and senesced biomass and returns them as
 *  ``'total_intact_biomass'`` and ``'total_litter_biomass'``, respectively.
 */
class total_biomass : public direct_module
{
   public:
    total_biomass(
        const state_map& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input parameters
          Grain{get_input(input_quantities, "Grain")},
          Leaf{get_input(input_quantities, "Leaf")},
          LeafLitter{get_input(input_quantities, "LeafLitter")},
          Rhizome{get_input(input_quantities, "Rhizome")},
          RhizomeLitter{get_input(input_quantities, "RhizomeLitter")},
          Root{get_input(input_quantities, "Root")},
          RootLitter{get_input(input_quantities, "RootLitter")},
          Shell{get_input(input_quantities, "Shell")},
          Stem{get_input(input_quantities, "Stem")},
          StemLitter{get_input(input_quantities, "StemLitter")},

          // Get pointers to output parameters
          total_intact_biomass_op{get_op(output_quantities, "total_intact_biomass")},
          total_litter_biomass_op{get_op(output_quantities, "total_litter_biomass")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "total_biomass"; }

   private:
    // Pointers to input parameters
    const double& Grain;
    const double& Leaf;
    const double& LeafLitter;
    const double& Rhizome;
    const double& RhizomeLitter;
    const double& Root;
    const double& RootLitter;
    const double& Shell;
    const double& Stem;
    const double& StemLitter;

    // Pointers to output parameters
    double* total_intact_biomass_op;
    double* total_litter_biomass_op;

    // Main operation
    void do_operation() const;
};

string_vector total_biomass::get_inputs()
{
    return {
        "Grain",          // Mg / ha
        "Leaf",           // Mg / ha
        "LeafLitter",     // Mg / ha
        "Rhizome",        // Mg / ha
        "RhizomeLitter",  // Mg / ha
        "Root",           // Mg / ha
        "RootLitter",     // Mg / ha
        "Shell",          // Mg / ha
        "Stem",           // Mg / ha
        "StemLitter"      // Mg / ha
    };
}

string_vector total_biomass::get_outputs()
{
    return {
        "total_intact_biomass",  // Mg / ha
        "total_litter_biomass"   // Mg / ha
    };
}

void total_biomass::do_operation() const
{
    update(total_intact_biomass_op,
           Grain + Leaf + Rhizome + Root + Shell + Stem);

    update(total_litter_biomass_op,
           LeafLitter + RhizomeLitter + RootLitter + StemLitter);
}

}  // namespace standardBML
#endif
