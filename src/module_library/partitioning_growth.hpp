#ifndef PARTITIONING_GROWTH_H
#define PARTITIONING_GROWTH_H

#include "../modules.h"

/**
 * @class partitioning_growth
 * 
 * @brief This module determines tissue mass derivatives based on the output of
 * either 'partitioning_growth_calculator' or 'no_leaf_resp_partitioning_growth_calculator'.
 * While this module will function using an adaptive step size integrator, some aspects of
 * it may not work as intended (e.g. the check for negative rhizome mass).
 */
class partitioning_growth : public DerivModule
{
   public:
    partitioning_growth(const std::unordered_map<std::string, double>* input_parameters,
                        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          DerivModule("partitioning_growth"),
          // Get pointers to input parameters
          kLeaf_ip(get_ip(input_parameters, "kLeaf")),
          kStem_ip(get_ip(input_parameters, "kStem")),
          kRoot_ip(get_ip(input_parameters, "kRoot")),
          kRhizome_ip(get_ip(input_parameters, "kRhizome")),
          kGrain_ip(get_ip(input_parameters, "kGrain")),
          canopy_assimilation_rate_ip(get_ip(input_parameters, "canopy_assimilation_rate")),
          newLeafcol_ip(get_ip(input_parameters, "newLeafcol")),
          newStemcol_ip(get_ip(input_parameters, "newStemcol")),
          newRootcol_ip(get_ip(input_parameters, "newRootcol")),
          newRhizomecol_ip(get_ip(input_parameters, "newRhizomecol")),
          Leaf_ip(get_ip(input_parameters, "Leaf")),
          Stem_ip(get_ip(input_parameters, "Stem")),
          Root_ip(get_ip(input_parameters, "Root")),
          Rhizome_ip(get_ip(input_parameters, "Rhizome")),
          // Get pointers to output parameters
          Leaf_op(get_op(output_parameters, "Leaf")),
          Stem_op(get_op(output_parameters, "Stem")),
          Root_op(get_op(output_parameters, "Root")),
          Rhizome_op(get_op(output_parameters, "Rhizome")),
          Grain_op(get_op(output_parameters, "Grain")),
          rhizome_senescence_index_op(get_op(output_parameters, "rhizome_senescence_index"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* kLeaf_ip;
    const double* kStem_ip;
    const double* kRoot_ip;
    const double* kRhizome_ip;
    const double* kGrain_ip;
    const double* canopy_assimilation_rate_ip;
    const double* newLeafcol_ip;
    const double* newStemcol_ip;
    const double* newRootcol_ip;
    const double* newRhizomecol_ip;
    const double* Leaf_ip;
    const double* Stem_ip;
    const double* Root_ip;
    const double* Rhizome_ip;
    // Pointers to output parameters
    double* Leaf_op;
    double* Stem_op;
    double* Root_op;
    double* Rhizome_op;
    double* Grain_op;
    double* rhizome_senescence_index_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> partitioning_growth::get_inputs()
{
    return {
        "kLeaf",
        "kStem",
        "kRoot",
        "kRhizome",
        "kGrain",
        "canopy_assimilation_rate",
        "newLeafcol",
        "newStemcol",
        "newRootcol",
        "newRhizomecol",
        "Leaf",
        "Stem",
        "Root",
        "Rhizome"};
}

std::vector<std::string> partitioning_growth::get_outputs()
{
    return {
        "Leaf",
        "Stem",
        "Root",
        "Rhizome",
        "Grain",
        "rhizome_senescence_index"};
}

void partitioning_growth::do_operation() const
{
    // Collect inputs and make calculations

    double kLeaf = *kLeaf_ip;
    double kStem = *kStem_ip;
    double kRoot = *kRoot_ip;
    double kRhizome = *kRhizome_ip;
    double kGrain = *kGrain_ip;
    double canopy_assimilation_rate = *canopy_assimilation_rate_ip;

    double newLeafcol = *newLeafcol_ip;
    double newStemcol = *newStemcol_ip;
    double newRootcol = *newRootcol_ip;
    double newRhizomecol = *newRhizomecol_ip;

    double Leaf = *Leaf_ip;
    double Stem = *Stem_ip;
    double Root = *Root_ip;
    double Rhizome = *Rhizome_ip;

    double dLeaf = 0.0, dStem = 0.0, dRoot = 0.0, dRhizome = 0.0, dGrain = 0.0, drhizome_senescence_index = 0.0;

    double retrans = 0.9;  // 0.9 is the efficiency of retranslocation (EBL: should this really be hard-coded into the model?)

    // Determine whether leaf is growing or decaying
    if (kLeaf > 0.0) {
        dLeaf += newLeafcol;
    } else {
        dLeaf += Leaf * kLeaf;
        dRhizome += kRhizome * (-dLeaf) * retrans;
        dStem += kStem * (-dLeaf) * retrans;
        dRoot += kRoot * (-dLeaf) * retrans;
        dGrain += kGrain * (-dLeaf) * retrans;
    }

    // Add any new Stem growth
    if (kStem >= 0.0) {
        dStem += newStemcol;
    } else {
        dStem += Stem * kStem;
        dRhizome += kRhizome * (-dStem) * retrans;
        dLeaf += kLeaf * (-dStem) * retrans;
        dRoot += kRoot * (-dStem) * retrans;
        dGrain += kGrain * (-dStem) * retrans;
    }

    // Determine whether Root is growing or decaying
    if (kRoot > 0.0) {
        dRoot += newRootcol;
    } else {
        dRoot += Root * kRoot;
        dRhizome += kRhizome * (-dRoot) * retrans;
        dStem += kStem * (-dRoot) * retrans;
        dLeaf += kLeaf * (-dRoot) * retrans;
        dGrain += kGrain * (-dRoot) * retrans;
    }

    // Determine whether the rhizome is growing or decaying
    if (kRhizome > 0.0) {
        dRhizome += newRhizomecol;
        // Here i will not work because the rhizome goes from being a source
        //  to a sink. I need its own index. Let's call it rhizome's i or ri
        drhizome_senescence_index += 1.0;
    } else {
        dRhizome += Rhizome * kRhizome;
        if (dRhizome + Rhizome < 0) {
            // Don't allow Rhizome mass to become negative
            // (only guaranteed to work for Euler method with 1 hour timestep)
            dRhizome = -0.9 * Rhizome;
        }
        dRoot += kRoot * (-dRhizome);  // Rhizome retranslocation efficiency is 1?
        dStem += kStem * (-dRhizome);
        dLeaf += kLeaf * (-dRhizome);
        dGrain += kGrain * (-dRhizome);
    }

    // Determine whether the grain is growing
    if (kGrain > 0.0 && canopy_assimilation_rate > 0.0) {
        dGrain += canopy_assimilation_rate * kGrain;
    }

    // Update the output parameter list
    update(Leaf_op, dLeaf);
    update(Stem_op, dStem);
    update(Root_op, dRoot);
    update(Rhizome_op, dRhizome);
    update(Grain_op, dGrain);
    update(rhizome_senescence_index_op, drhizome_senescence_index);
}

#endif
