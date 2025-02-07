#ifndef STANDARDBML_CUMULATIVE_RESPIRATION_H
#define STANDARDBML_CUMULATIVE_RESPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class cumulative_respiration
 *
 * @brief Enables calculations of cumulative respiration costs, which will be
 * included in the simulation output as differential quantities called
 * ``'Leaf_mr'``, ``'Stem_mr'``, etc, where "mr" standard for "maintenance
 * respiration."
 *
 */
class cumulative_respiration : public differential_module
{
   public:
    cumulative_respiration(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          Leaf_mrr{get_input(input_quantities, "Leaf_mrr")},
          Stem_mrr{get_input(input_quantities, "Stem_mrr")},
          Root_mrr{get_input(input_quantities, "Root_mrr")},
          Rhizome_mrr{get_input(input_quantities, "Rhizome_mrr")},
          Grain_mrr{get_input(input_quantities, "Grain_mrr")},
          Shell_mrr{get_input(input_quantities, "Shell_mrr")},

          // Get pointers to output quantities
          Leaf_mr_op{get_op(output_quantities, "Leaf_mr")},
          Stem_mr_op{get_op(output_quantities, "Stem_mr")},
          Root_mr_op{get_op(output_quantities, "Root_mr")},
          Rhizome_mr_op{get_op(output_quantities, "Rhizome_mr")},
          Grain_mr_op{get_op(output_quantities, "Grain_mr")},
          Shell_mr_op{get_op(output_quantities, "Shell_mr")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_respiration"; }

   private:
    // References to input quantities
    double const& Leaf_mrr;
    double const& Stem_mrr;
    double const& Root_mrr;
    double const& Rhizome_mrr;
    double const& Grain_mrr;
    double const& Shell_mrr;

    // Pointers to output quantities
    double* Leaf_mr_op;
    double* Stem_mr_op;
    double* Root_mr_op;
    double* Rhizome_mr_op;
    double* Grain_mr_op;
    double* Shell_mr_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_respiration::get_inputs()
{
    return {
        "Leaf_mrr",     // Mg / ha / hr
        "Stem_mrr",     // Mg / ha / hr
        "Root_mrr",     // Mg / ha / hr
        "Rhizome_mrr",  // Mg / ha / hr
        "Grain_mrr",    // Mg / ha / hr
        "Shell_mrr"     // Mg / ha / hr
    };
}

string_vector cumulative_respiration::get_outputs()
{
    return {
        "Leaf_mr",     // Mg / ha / hr
        "Stem_mr",     // Mg / ha / hr
        "Root_mr",     // Mg / ha / hr
        "Rhizome_mr",  // Mg / ha / hr
        "Grain_mr",    // Mg / ha / hr
        "Shell_mr"     // Mg / ha / hr
    };
}

void cumulative_respiration::do_operation() const
{
    update(Leaf_mr_op, Leaf_mrr);        // Mg / ha / hr
    update(Stem_mr_op, Stem_mrr);        // Mg / ha / hr
    update(Root_mr_op, Root_mrr);        // Mg / ha / hr
    update(Rhizome_mr_op, Rhizome_mrr);  // Mg / ha / hr
    update(Grain_mr_op, Grain_mrr);      // Mg / ha / hr
    update(Shell_mr_op, Shell_mrr);      // Mg / ha / hr
}

}  // namespace standardBML
#endif
