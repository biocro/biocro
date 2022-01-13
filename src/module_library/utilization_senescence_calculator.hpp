#ifndef UTILIZATION_SENESCENCE_CALCULATOR_H
#define UTILIZATION_SENESCENCE_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

class utilization_senescence_calculator : public direct_module
{
   public:
    utilization_senescence_calculator(state_map const& input_quantities, state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          rate_constant_leaf_senescence_ip(get_ip(input_quantities, "rate_constant_leaf_senescence")),
          rate_constant_stem_senescence_ip(get_ip(input_quantities, "rate_constant_stem_senescence")),
          rate_constant_root_senescence_ip(get_ip(input_quantities, "rate_constant_root_senescence")),
          rate_constant_rhizome_senescence_ip(get_ip(input_quantities, "rate_constant_rhizome_senescence")),
          Leaf_ip(get_ip(input_quantities, "Leaf")),
          Stem_ip(get_ip(input_quantities, "Stem")),
          Root_ip(get_ip(input_quantities, "Root")),
          Rhizome_ip(get_ip(input_quantities, "Rhizome")),

          // Get pointers to output quantities
          senescence_leaf_op(get_op(output_quantities, "senescence_leaf")),
          senescence_stem_op(get_op(output_quantities, "senescence_stem")),
          senescence_root_op(get_op(output_quantities, "senescence_root")),
          senescence_rhizome_op(get_op(output_quantities, "senescence_rhizome"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "utilization_senescence_calculator"; }

   private:
    // Pointers to input quantities
    const double* rate_constant_leaf_senescence_ip;
    const double* rate_constant_stem_senescence_ip;
    const double* rate_constant_root_senescence_ip;
    const double* rate_constant_rhizome_senescence_ip;
    const double* Leaf_ip;
    const double* Stem_ip;
    const double* Root_ip;
    const double* Rhizome_ip;

    // Pointers to output quantities
    double* senescence_leaf_op;
    double* senescence_stem_op;
    double* senescence_root_op;
    double* senescence_rhizome_op;

    // Main operation
    void do_operation() const;
};

string_vector utilization_senescence_calculator::get_inputs()
{
    return {
        "rate_constant_leaf_senescence",
        "rate_constant_stem_senescence",
        "rate_constant_root_senescence",
        "rate_constant_rhizome_senescence",
        "Leaf",
        "Stem",
        "Root",
        "Rhizome"};
}

string_vector utilization_senescence_calculator::get_outputs()
{
    return {
        "senescence_leaf",
        "senescence_stem",
        "senescence_root",
        "senescence_rhizome"};
}

void utilization_senescence_calculator::do_operation() const
{
    // Collect inputs and make calculations

    double kLeaf = *rate_constant_leaf_senescence_ip;
    double kStem = *rate_constant_stem_senescence_ip;
    double kRoot = *rate_constant_root_senescence_ip;
    double kRhizome = *rate_constant_rhizome_senescence_ip;

    double Leaf = *Leaf_ip;
    double Stem = *Stem_ip;
    double Root = *Root_ip;
    double Rhizome = *Rhizome_ip;

    // In the Thornley senescence model, sensescence is proportional to structural mass
    //  e.g. senescence_leaf = kLeaf * Leaf, for 0 < kLeaf < 1
    // This is quite general. To see why, imagine using Michaelis-Menton kinetics here.
    // In this case, we would have a catalyzed reaction turning structural carbon into
    //  substrate or litter
    // The rate of change of the structural carbon concentration would be
    //  proportional to [structural carbon]/(Km + [structural carbon])
    // And so the rate of change of total structural carbon would be proportional
    //  to volume * [structural carbon]/(Km + [structural carbon])
    // For a tissue, volume is proportional to the amount of structural carbon
    // In this case, [structural carbon] is constant
    //  E.g. we have a rate proportional to Leaf * constant/(Km + constant)
    //  We can absorb the constant into the proportionality constant, ending up with
    //   senescence_leaf = kLeaf * Leaf again
    //  So ultimately this is the same as the simpler assumption
    // Here we do not allow the grain to senesce

    double senescence_leaf = kLeaf * Leaf;
    double senescence_stem = kStem * Stem;
    double senescence_root = kRoot * Root;
    double senescence_rhizome = kRhizome * Rhizome;

    // Update the output quantity list
    update(senescence_leaf_op, senescence_leaf);
    update(senescence_stem_op, senescence_stem);
    update(senescence_root_op, senescence_root);
    update(senescence_rhizome_op, senescence_rhizome);
}

#endif
