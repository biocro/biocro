#ifndef EXAMPLE_MODEL_PARTITIONING_H
#define EXAMPLE_MODEL_PARTITIONING_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class example_model_partitioning
 *
 *  @brief An example for the BioCro II manuscript
 *
 *  ### Model overview
 *
 *  Here we model the partitioning of mass between tissues. This model is not
 *  meant to be biologically realistic.
 *
 *  In this model, it is assumed that the plant's mass gain is distributed
 *  between its leaf and root according to fractions `f_leaf` and `f_root`. In
 *  other words:
 *
 *  > `dm_leaf / dt = mass_gain * f_leaf` (1)
 *
 *  > `dm_root / dt = mass_gain * f_root` (2)
 *
 *  where 'mass_gain` is the plant's total rate of mass increase, `dm_leaf / dt`
 *  is the time derivative of the leaf mass `m_leaf` and `dm_root / dt` is
 *  defined analagously. Note: typically, the sum of `f_leaf` and `f_root`
 *  should be 1.
 *
 */
class example_model_partitioning : public differential_module
{
   public:
    example_model_partitioning(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          mass_gain{get_input(input_quantities, "mass_gain")},
          f_leaf{get_input(input_quantities, "f_leaf")},
          f_root{get_input(input_quantities, "f_root")},

          // Get pointers to output quantities
          Leaf_op{get_op(output_quantities, "Leaf")},
          Root_op{get_op(output_quantities, "Root")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "example_model_partitioning"; }

   private:
    // References to input quantities
    double const& mass_gain;
    double const& f_leaf;
    double const& f_root;

    // Pointers to output quantities
    double* Leaf_op;
    double* Root_op;

    // Main operation
    void do_operation() const;
};

string_vector example_model_partitioning::get_inputs()
{
    return {
        "mass_gain",  // kg / s
        "f_leaf",     // dimensionless from kg / kg
        "f_root"      // dimensionless from kg / kg
    };
}

string_vector example_model_partitioning::get_outputs()
{
    return {
        "Leaf",  // kg / s
        "Root"   // kg / s
    };
}

void example_model_partitioning::do_operation() const
{
    double const dLeaf = mass_gain * f_leaf;
    double const dRoot = mass_gain * f_root;

    update(Leaf_op, dLeaf);
    update(Root_op, dRoot);
}

}  // namespace standardBML
#endif
