#ifndef NO_LEAF_RESP_NEG_ASSIM_PARTITIONING_GROWTH_CALCULATOR_H
#define NO_LEAF_RESP_NEG_ASSIM_PARTITIONING_GROWTH_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "respiration.h"  // for resp

namespace standardBML
{
/**
 *  @class no_leaf_resp_neg_assim_partitioning_growth_calculator
 *
 *  @brief Uses a set of partitioning coefficients to determine net assimilation
 *  rates due to photosynthesis and respiration for several plant organs.
 *
 *  ### Partitioning overview
 *
 *  BioCro includes several partitioning growth calculators that determine these
 *  rates using slightly different methods. The different modules can be
 *  distinguished by the sets of tissues they use, the ways they apply
 *  respiration and water stress, and their responses to negative canopy
 *  assimilation rates. (A negative canopy assimilation rate indicates that the
 *  leaves are respiring more than they are photosynthesizing.)
 *
 *  In all partitioning growth calculators, the base growth rate for an organ is
 *  determined from the net canopy assimilation rate and a coefficient that
 *  determines the fraction of the net assimilation that is "partitioned" to
 *  that organ. Then, further modifications may take place to account for water
 *  stress, maintenance respiration, or other processes that affect the amount
 *  of carbon available to the organ for growth. Note that losses due to
 *  senescence and gains due to remobilized carbon from other organs are handled
 *  elsewhere and are not included here.
 *
 *  Respiration is included via the `resp()` function, which implements an
 *  empirical rule for determining the fraction of energy spent on respiration
 *  at a particular temperature. See the following paper for a general
 *  discussion of the importance of respiration in understanding plant growth:
 *  [Amthor, J. S. "The role of maintenance respiration in plant growth" Plant,
 *  Cell & Environment 7, 561–569 (1984)]
 *  (https://doi.org/10.1111/1365-3040.ep11591833).
 *
 *  ### Specifics of this module
 *
 *  In this module, no distinction is made between positive and negative canopy
 *  assimilation rates. Thus, respiratory losses in the leaf that result in a
 *  negative canopy assimilation rate are spread out to the other organs.
 *
 *  This module does not attempt to explicitly include any effect due to water
 *  stress.
 *
 *  This module includes six organs:
 *  - `Leaf`: The leaf growth rate is *not* modified by respiration because the
 *     net canopy assimilation rate already includes it.
 *  - `Stem`: The stem growth rate is modified by respiration.
 *  - `Root`: The root growth rate is modified by respiration.
 *  - `Rhizome`: The rhizome growth rate is modified by respiration.
 *  - `Grain`: The grain growth rate is *not* modified by respiration.
 *  - `Shell`: The shell growth rate is *not* modified by respiration.
 */
class no_leaf_resp_neg_assim_partitioning_growth_calculator : public direct_module
{
   public:
    no_leaf_resp_neg_assim_partitioning_growth_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          kLeaf{get_input(input_quantities, "kLeaf")},
          kStem{get_input(input_quantities, "kStem")},
          kRoot{get_input(input_quantities, "kRoot")},
          kRhizome{get_input(input_quantities, "kRhizome")},
          kGrain{get_input(input_quantities, "kGrain")},
          kShell{get_input(input_quantities, "kShell")},
          kNodule{get_input(input_quantities, "kNodule")},
          canopy_assim{get_input(input_quantities, "canopy_assimilation_rate")},
          grc_stem{get_input(input_quantities, "grc_stem")},
          grc_root{get_input(input_quantities, "grc_root")},
          temp{get_input(input_quantities, "temp")},
          LeafWS{get_input(input_quantities, "LeafWS")},

          // Get pointers to output quantities
          net_assimilation_rate_leaf_op{get_op(output_quantities, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem_op{get_op(output_quantities, "net_assimilation_rate_stem")},
          net_assimilation_rate_root_op{get_op(output_quantities, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome_op{get_op(output_quantities, "net_assimilation_rate_rhizome")},
          net_assimilation_rate_grain_op{get_op(output_quantities, "net_assimilation_rate_grain")},
          net_assimilation_rate_shell_op{get_op(output_quantities, "net_assimilation_rate_shell")},
          net_assimilation_rate_nodule_op{get_op(output_quantities, "net_assimilation_rate_nodule")},
          Stem_grr_op{get_op(output_quantities, "Stem_grr")},
          Root_grr_op{get_op(output_quantities, "Root_grr")},
          Rhizome_grr_op{get_op(output_quantities, "Rhizome_grr")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "no_leaf_resp_neg_assim_partitioning_growth_calculator"; }

   private:
    // References to input quantities
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& kShell;
    const double& kNodule;
    const double& canopy_assim;
    const double& grc_stem;
    const double& grc_root;
    const double& temp;
    const double& LeafWS;

    // Pointers to output quantities
    double* net_assimilation_rate_leaf_op;
    double* net_assimilation_rate_stem_op;
    double* net_assimilation_rate_root_op;
    double* net_assimilation_rate_rhizome_op;
    double* net_assimilation_rate_grain_op;
    double* net_assimilation_rate_shell_op;
    double* net_assimilation_rate_nodule_op;
    double* Stem_grr_op;
    double* Root_grr_op;
    double* Rhizome_grr_op;

    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector no_leaf_resp_neg_assim_partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",                     // dimensionless
        "kStem",                     // dimensionless
        "kRoot",                     // dimensionless
        "kRhizome",                  // dimensionless
        "kGrain",                    // dimensionless
        "kShell",                    // dimensionless
        "kNodule",                   // dimensionless
        "canopy_assimilation_rate",  // Mg / ha / hr
        "grc_stem",                  // dimensionless
        "grc_root",                  // dimensionless
        "temp",                      // degrees C
        "LeafWS"                     // dimensionless
    };
}

string_vector no_leaf_resp_neg_assim_partitioning_growth_calculator::get_outputs()
{
    return {
        "net_assimilation_rate_leaf",     // Mg / ha / hr
        "net_assimilation_rate_stem",     // Mg / ha / hr
        "net_assimilation_rate_root",     // Mg / ha / hr
        "net_assimilation_rate_rhizome",  // Mg / ha / hr
        "net_assimilation_rate_grain",    // Mg / ha / hr
        "net_assimilation_rate_shell",    // Mg / ha / hr
        "net_assimilation_rate_nodule",   // Mg / ha / hr
        "Stem_grr",                       // Mg / ha / hr
        "Root_grr",                       // Mg / ha / hr
        "Rhizome_grr"                     // Mg / ha / hr
    };
}

void no_leaf_resp_neg_assim_partitioning_growth_calculator::do_operation() const
{
    // Calculate the rate of new leaf production, accounting for water stress
    // but not respiratory costs (Mg / ha / hr)
    double const net_assimilation_rate_leaf{
        kLeaf > 0 ? canopy_assim * kLeaf * LeafWS : 0};

    // Calculate the rate of new stem production, accounting for respiratory
    // costs (Mg / ha / hr)
    respiration_outputs const adjusted_rate_stem{
        resp(kStem > 0 ? canopy_assim * kStem : 0, grc_stem, temp)};

    // Calculate the rate of new root production, accounting for respiratory
    // costs (Mg / ha / hr)
    respiration_outputs const adjusted_rate_root{
        resp(kRoot > 0 ? canopy_assim * kRoot : 0, grc_root, temp)};

    // Calculate the rate of new rhizome production, accounting for respiratory
    // costs (Mg / ha / hr)
    respiration_outputs const adjusted_rate_rhizome{
        resp(kRhizome > 0 ? canopy_assim * kRhizome : 0, grc_root, temp)};

    // Calculate the rate of new grain production without any respiratory costs
    // (Mg / ha / hr)
    double const net_assimilation_rate_grain{
        kGrain > 0 ? canopy_assim * kGrain : 0};

    // Calculate the rate of new shell production without any respiratory costs
    // (Mg / ha / hr)
    double const net_assimilation_rate_shell{
        kShell > 0 ? canopy_assim * kShell : 0};

    // Calculate the rate of new nodule production without any respiratory costs
    // (Mg / ha / hr)
    double net_assimilation_rate_nodule{
        kNodule > 0 ? canopy_assim * kNodule : 0};

    // Update the output quantity list
    update(net_assimilation_rate_leaf_op, net_assimilation_rate_leaf);         // Mg / ha / hr
    update(net_assimilation_rate_stem_op, adjusted_rate_stem.net_rate);        // Mg / ha / hr
    update(net_assimilation_rate_root_op, adjusted_rate_root.net_rate);        // Mg / ha / hr
    update(net_assimilation_rate_rhizome_op, adjusted_rate_rhizome.net_rate);  // Mg / ha / hr
    update(net_assimilation_rate_grain_op, net_assimilation_rate_grain);       // Mg / ha / hr
    update(net_assimilation_rate_shell_op, net_assimilation_rate_shell);       // Mg / ha / hr
    update(net_assimilation_rate_nodule_op, net_assimilation_rate_nodule);     // Mg / ha / hr
    update(Stem_grr_op, adjusted_rate_stem.respiration_rate);                  // Mg / ha / hr
    update(Root_grr_op, adjusted_rate_root.respiration_rate);                  // Mg / ha / hr
    update(Rhizome_grr_op, adjusted_rate_rhizome.respiration_rate);            // Mg / ha / hr
}

}  // namespace standardBML
#endif
