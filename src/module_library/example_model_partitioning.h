#ifndef EXAMPLE_MODEL_PARTITIONING_H
#define EXAMPLE_MODEL_PARTITIONING_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class example_model
 *
 * @brief An example for the BioCro II manuscript
 *
 * ### Model overview
 *
 * Model partitioning of mass between tissues.
 *
 * It is not meant to be bioloigcally realistic.
 *
 */
class example_model_partitioning : public differential_module
{
   public:
    example_model_partitioning(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          differential_module("example_model_partitioning"),

          // Get pointers to input quantities
          mass_gain(get_input(input_quantities, "mass_gain")),
          f_leaf(get_input(input_quantities, "f_leaf")),
          f_root(get_input(input_quantities, "f_root")),

          // Get pointers to output quantities
          Leaf_op(get_op(output_quantities, "Leaf")),
          Root_op(get_op(output_quantities, "Root"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

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
        "f_leaf",        // dimensionless from kg / kg
        "f_root"         // dimensionless from kg / kg
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

#endif
