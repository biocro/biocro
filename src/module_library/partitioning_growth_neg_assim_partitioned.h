#ifndef PARTITIONING_GROWTH_NEG_ASSIM_PARTITIONED_H
#define PARTITIONING_GROWTH_NEG_ASSIM_PARTITIONED_H

#include "../modules.h"
#include "../state_map.h"

/**
 * @class partitioning_growth_neg_assim_partitioned
 *
 * @brief This module determines tissue mass derivatives based on the output of
 * either 'partitioning_growth_neg_assim_partitioned_calculator' or 'no_leaf_resp_neg_assim_partitioning_growth_calculator'.
 * While this module will function using an adaptive step size integrator, some aspects of
 * it may not work as intended (e.g. the check for negative rhizome mass).
 */
class partitioning_growth_neg_assim_partitioned : public DerivModule
{
   public:
    partitioning_growth_neg_assim_partitioned(
        const state_map* input_parameters,
        state_map* output_parameters)
        : DerivModule{"partitioning_growth_neg_assim_partitioned"},

          // Get references to input parameters
          kLeaf{get_input(input_parameters, "kLeaf")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          newLeafcol{get_input(input_parameters, "newLeafcol")},
          newStemcol{get_input(input_parameters, "newStemcol")},
          newRootcol{get_input(input_parameters, "newRootcol")},
          newRhizomecol{get_input(input_parameters, "newRhizomecol")},
          newGraincol{get_input(input_parameters, "newGraincol")},
          Leaf{get_input(input_parameters, "Leaf")},
          Stem{get_input(input_parameters, "Stem")},
          Root{get_input(input_parameters, "Root")},
          Rhizome{get_input(input_parameters, "Rhizome")},

          // Get pointers to output parameters
          Leaf_op{get_op(output_parameters, "Leaf")},
          Stem_op{get_op(output_parameters, "Stem")},
          Root_op{get_op(output_parameters, "Root")},
          Rhizome_op{get_op(output_parameters, "Rhizome")},
          Grain_op{get_op(output_parameters, "Grain")},
          rhizome_senescence_index_op{get_op(output_parameters, "rhizome_senescence_index")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input parameters
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& newLeafcol;
    const double& newStemcol;
    const double& newRootcol;
    const double& newRhizomecol;
    const double& newGraincol;
    const double& Leaf;
    const double& Stem;
    const double& Root;
    const double& Rhizome;

    // Pointers to output parameters
    double* Leaf_op;
    double* Stem_op;
    double* Root_op;
    double* Rhizome_op;
    double* Grain_op;
    double* rhizome_senescence_index_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector partitioning_growth_neg_assim_partitioned::get_inputs()
{
    return {
        "kLeaf",          // dimensionless
        "kStem",          // dimensionless
        "kRoot",          // dimensionless
        "kRhizome",       // dimensionless
        "kGrain",         // dimensionless
        "newLeafcol",     // Mg / ha / hour
        "newStemcol",     // Mg / ha / hour
        "newRootcol",     // Mg / ha / hour
        "newRhizomecol",  // Mg / ha / hour
        "newGraincol",    // Mg / ha / hour
        "Leaf",           // Mg / ha
        "Stem",           // Mg / ha
        "Root",           // Mg / ha
        "Rhizome"         // Mg / ha
    };
}

string_vector partitioning_growth_neg_assim_partitioned::get_outputs()
{
    return {
        "Leaf",                     // Mg / ha
        "Stem",                     // Mg / ha
        "Root",                     // Mg / ha
        "Rhizome",                  // Mg / ha
        "Grain",                    // Mg / ha
        "rhizome_senescence_index"  // dimensionless
    };
}

void partitioning_growth_neg_assim_partitioned::do_operation() const
{
    // Collect inputs and make calculations

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
    if (kGrain > 0.0) {
        dGrain += newGraincol;
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
