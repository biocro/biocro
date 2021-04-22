#ifndef TOTAL_BIOMASS_H
#define TOTAL_BIOMASS_H

#include "../modules.h"
#include "../state_map.h"

/**
 *  @class total_biomass
 *
 *  @brief Calculates the total biomass by adding together the masses of the
 *  `Leaf`, `Stem`, `Root`, `Rhizome`, and `Grain` tissues.
 */
class total_biomass : public SteadyModule
{
   public:
    total_biomass(
        const state_map* input_parameters,
        state_map* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("total_biomass"),

          // Get pointers to input parameters
          Leaf{get_input(input_parameters, "Leaf")},
          Stem{get_input(input_parameters, "Stem")},
          Root{get_input(input_parameters, "Root")},
          Rhizome{get_input(input_parameters, "Rhizome")},
          Grain{get_input(input_parameters, "Grain")},

          // Get pointers to output parameters
          total_biomass_op{get_op(output_parameters, "total_biomass")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input parameters
    const double& Leaf;
    const double& Stem;
    const double& Root;
    const double& Rhizome;
    const double& Grain;

    // Pointers to output parameters
    double* total_biomass_op;

    // Main operation
    void do_operation() const;
};

string_vector total_biomass::get_inputs()
{
    return {
        "Leaf",     // Mg / ha
        "Stem",     // Mg / ha
        "Root",     // Mg / ha
        "Rhizome",  // Mg / ha
        "Grain"     // Mg / ha
    };
}

string_vector total_biomass::get_outputs()
{
    return {
        "total_biomass"  // Mg / ha
    };
}

void total_biomass::do_operation() const
{
    update(total_biomass_op, Leaf + Stem + Root + Rhizome + Grain);
}

#endif
