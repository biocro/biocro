#ifndef PARTITIONING_GROWTH_H
#define PARTITIONING_GROWTH_H

#include "../modules.h"
#include "../state_map.h"

/**
 *  @class partitioning_growth
 *
 *  @brief This module determines the growth rate for several plant organs from
 *  the net rate of carbon assimilation due to photosynthesis (determined by a
 *  "partitioning growth calculator" module) and any additional carbon that may
 *  come from retranslocation.
 *
 *  If the partitioning coefficient for an organ is negative, this indicates
 *  that the organ is donating some of its mass to the other organs. In this
 *  case, there is a negative term in the rate of change of that organ's mass,
 *  and smaller positive terms in the rates for the other organs.
 *
 *  If the partitioning coefficient for an organ is positive, it is growing. Its
 *  derivative will have a positive term representing the contribution from
 *  photosynthesis, and possibly other terms representing retranslocated carbon
 *  from other organs.
 *
 *  This module also keeps track of the time point at which the rhizome changes
 *  from a carbon source to a sink. This is accomplished via the
 *  `rhizome_senescence_index`, which increases at a rate of 1 per hour while
 *  the rhizome is a sink. This quantity is required by the
 *  `thermal_time_senescence` and `thermal_time_and_frost_senescence` modules,
 *  which need to look backward in time to the point where each organ begins to
 *  grow. Ultimately, this is not a good approach to senescence. If / when these
 *  senescence modules are replaced, the rhizome senescence index can safely be
 *  removed from this module.
 *
 *  Conceptually, there are some problems with this type of carbon allocation
 *  model.
 *
 *  One issue is that the final rates depend on the order in which the organs
 *  are addressed when mass is being remobilized. In practice this is rarely an
 *  issue since it is uncommon for multiple organs to act as carbon sources at
 *  the same time.
 *
 *  Another issue is that an organ's mass may become negative when it is being
 *  retranslocated if the system is being solved using the Euler method with a
 *  large step size. The rhizome is especially prone to suffer from this issue,
 *  so a clunky check has been included here to try to prevent it from
 *  occurring. Note that this check introduces a weird discontinuity and is only
 *  relevant when using the Euler solver with a step size of one hour. Strange
 *  behavior may occur when using other solvers for a plant with a remobilizing
 *  rhizome.
 *
 *  The model represented by this module is not officially described anywhere,
 *  although it has been used for carbon allocation in several published BioCro
 *  papers, such as the following:
 *
 *  - Miguez, F. E., Zhu, X., Humphries, S., Bollero, G. A. & Long, S. P. "A
 *    semimechanistic model predicting the growth and production of the
 *    bioenergy crop Miscanthus×giganteus: description, parameterization and
 *    validation. [GCB Bioenergy 1, 282–296 (2009)]
 *    (https://doi.org/10.1111/j.1757-1707.2009.01019.x)
 *
 *  - Jaiswal, D. et al. Brazilian sugarcane ethanol as an expandable green
 *    alternative to crude oil use. [Nature Climate Change 7, 788–792 (2017)]
 *    (https://doi.org/10.1038/nclimate3410)
 *
 *  Some discussion of similar models can also be found at several points in
 *  [Penning de Vries, F. W. T. & Laar, H. H. van. "Simulation of plant growth
 *  and crop production" (Pudoc, 1982)](http://edepot.wur.nl/167315)
 */
class partitioning_growth : public DerivModule
{
   public:
    partitioning_growth(
        const state_map* input_parameters,
        state_map* output_parameters)
        : DerivModule{"partitioning_growth"},

          // Get references to input parameters
          retrans{get_input(input_parameters, "retrans")},
          retrans_rhizome{get_input(input_parameters, "retrans_rhizome")},
          kLeaf{get_input(input_parameters, "kLeaf")},
          kStem{get_input(input_parameters, "kStem")},
          kRoot{get_input(input_parameters, "kRoot")},
          kRhizome{get_input(input_parameters, "kRhizome")},
          kGrain{get_input(input_parameters, "kGrain")},
          net_assimilation_rate_leaf{get_input(input_parameters, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem{get_input(input_parameters, "net_assimilation_rate_stem")},
          net_assimilation_rate_root{get_input(input_parameters, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome{get_input(input_parameters, "net_assimilation_rate_rhizome")},
          net_assimilation_rate_grain{get_input(input_parameters, "net_assimilation_rate_grain")},
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
    const double& retrans;
    const double& retrans_rhizome;
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& net_assimilation_rate_leaf;
    const double& net_assimilation_rate_stem;
    const double& net_assimilation_rate_root;
    const double& net_assimilation_rate_rhizome;
    const double& net_assimilation_rate_grain;
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

string_vector partitioning_growth::get_inputs()
{
    return {
        "retrans",          // dimensionless
        "retrans_rhizome",  // dimensionless
        "kLeaf",            // dimensionless
        "kStem",            // dimensionless
        "kRoot",            // dimensionless
        "kRhizome",         // dimensionless
        "kGrain",           // dimensionless
        "net_assimilation_rate_leaf",       // Mg / ha / hour
        "net_assimilation_rate_stem",       // Mg / ha / hour
        "net_assimilation_rate_root",       // Mg / ha / hour
        "net_assimilation_rate_rhizome",    // Mg / ha / hour
        "net_assimilation_rate_grain",      // Mg / ha / hour
        "Leaf",             // Mg / ha
        "Stem",             // Mg / ha
        "Root",             // Mg / ha
        "Rhizome"           // Mg / ha
    };
}

string_vector partitioning_growth::get_outputs()
{
    return {
        "Leaf",                     // Mg / ha / hour
        "Stem",                     // Mg / ha / hour
        "Root",                     // Mg / ha / hour
        "Rhizome",                  // Mg / ha / hour
        "Grain",                    // Mg / ha / hour
        "rhizome_senescence_index"  // hour^-1
    };
}

void partitioning_growth::do_operation() const
{
    // Initialize variables
    double dLeaf {0.0};
    double dStem {0.0};
    double dRoot {0.0};
    double dRhizome {0.0};
    double dGrain {0.0};
    double drhizome_senescence_index {0.0};

    // Determine whether Leaf is growing or decaying
    if (kLeaf > 0.0) {
        dLeaf += net_assimilation_rate_leaf;
    } else {
        dLeaf += Leaf * kLeaf;
        dRhizome += kRhizome * (-dLeaf) * retrans;
        dStem += kStem * (-dLeaf) * retrans;
        dRoot += kRoot * (-dLeaf) * retrans;
        dGrain += kGrain * (-dLeaf) * retrans;
    }

    // Determine whether Stem is growing or decaying
    if (kStem >= 0.0) {
        dStem += net_assimilation_rate_stem;
    } else {
        dStem += Stem * kStem;
        dRhizome += kRhizome * (-dStem) * retrans;
        dLeaf += kLeaf * (-dStem) * retrans;
        dRoot += kRoot * (-dStem) * retrans;
        dGrain += kGrain * (-dStem) * retrans;
    }

    // Determine whether Root is growing or decaying
    if (kRoot > 0.0) {
        dRoot += net_assimilation_rate_root;
    } else {
        dRoot += Root * kRoot;
        dRhizome += kRhizome * (-dRoot) * retrans;
        dStem += kStem * (-dRoot) * retrans;
        dLeaf += kLeaf * (-dRoot) * retrans;
        dGrain += kGrain * (-dRoot) * retrans;
    }

    // Determine whether Rhizome is growing or decaying
    if (kRhizome > 0.0) {
        dRhizome += net_assimilation_rate_rhizome;
        drhizome_senescence_index = 1.0;
    } else {
        dRhizome += Rhizome * kRhizome;
        if (dRhizome + Rhizome < 0) {
            // Try to prevent Rhizome mass from becoming negative
            dRhizome = -0.9 * Rhizome;
        }
        dRoot += kRoot * (-dRhizome) * retrans_rhizome;
        dStem += kStem * (-dRhizome) * retrans_rhizome;
        dLeaf += kLeaf * (-dRhizome) * retrans_rhizome;
        dGrain += kGrain * (-dRhizome) * retrans_rhizome;
    }

    // Determine whether Grain is growing
    if (kGrain > 0.0) {
        dGrain += net_assimilation_rate_grain;
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
