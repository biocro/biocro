#ifndef NO_LEAF_RESP_PARTITIONING_GROWTH_CALCULATOR_H
#define NO_LEAF_RESP_PARTITIONING_GROWTH_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "BioCro.h"  // for resp

namespace standardBML
{
/**
 *  @class no_leaf_resp_partitioning_growth_calculator
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
 *  When the canopy assimilation rate is negative, this module ensures that all
 *  losses come from the leaf, and the growth rates for the other organs are set
 *  to zero since no carbon is flowing out of the leaf.
 *
 *  This module does not attempt to explicitly include any effect due to water
 *  stress.
 *
 *  This module includes five organs:
 *  - `Leaf`: The leaf growth rate is *not* modified by respiration because the
 *     net canopy assimilation rate already includes it.
 *  - `Stem`: The stem growth rate is modified by respiration.
 *  - `Root`: The root growth rate is modified by respiration.
 *  - `Rhizome`: The rhizome growth rate is modified by respiration.
 *  - `Grain`: The grain growth rate is *not* modified by respiration.
 */
class no_leaf_resp_partitioning_growth_calculator : public direct_module
{
   public:
    no_leaf_resp_partitioning_growth_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          kLeaf{get_input(input_quantities, "kLeaf")},
          kStem{get_input(input_quantities, "kStem")},
          kRoot{get_input(input_quantities, "kRoot")},
          kRhizome{get_input(input_quantities, "kRhizome")},
          kGrain{get_input(input_quantities, "kGrain")},
          canopy_assim{get_input(input_quantities, "canopy_assimilation_rate")},
          mrc1{get_input(input_quantities, "mrc1")},
          mrc2{get_input(input_quantities, "mrc2")},
          temp{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          net_assimilation_rate_leaf_op{get_op(output_quantities, "net_assimilation_rate_leaf")},
          net_assimilation_rate_stem_op{get_op(output_quantities, "net_assimilation_rate_stem")},
          net_assimilation_rate_root_op{get_op(output_quantities, "net_assimilation_rate_root")},
          net_assimilation_rate_rhizome_op{get_op(output_quantities, "net_assimilation_rate_rhizome")},
          net_assimilation_rate_grain_op{get_op(output_quantities, "net_assimilation_rate_grain")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "no_leaf_resp_partitioning_growth_calculator"; }

   private:
    // Pointers to input quantities
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& canopy_assim;
    const double& mrc1;
    const double& mrc2;
    const double& temp;

    // Pointers to output quantities
    double* net_assimilation_rate_leaf_op;
    double* net_assimilation_rate_stem_op;
    double* net_assimilation_rate_root_op;
    double* net_assimilation_rate_rhizome_op;
    double* net_assimilation_rate_grain_op;

    // Main operation
    void do_operation() const;
};

string_vector no_leaf_resp_partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",                     // dimensionless
        "kStem",                     // dimensionless
        "kRoot",                     // dimensionless
        "kRhizome",                  // dimensionless
        "kGrain",                    // dimensionless
        "canopy_assimilation_rate",  // Mg / ha / hour
        "mrc1",                      // dimensionless
        "mrc2",                      // dimensionless
        "temp"                       // degrees C
    };
}

string_vector no_leaf_resp_partitioning_growth_calculator::get_outputs()
{
    return {
        "net_assimilation_rate_leaf",     // Mg / ha / hour
        "net_assimilation_rate_stem",     // Mg / ha / hour
        "net_assimilation_rate_root",     // Mg / ha / hour
        "net_assimilation_rate_rhizome",  // Mg / ha / hour
        "net_assimilation_rate_grain"     // Mg / ha / hour
    };
}

void no_leaf_resp_partitioning_growth_calculator::do_operation() const
{
    // Calculate the rate of new leaf production, including _all_ losses due to
    // photorespiration and Rd, but not accounting for water stress or any
    // additional respiration (Mg / ha / hr)
    double const net_assimilation_rate_leaf{
        kLeaf <= 0         ? 0
        : canopy_assim < 0 ? canopy_assim
                           : canopy_assim * kLeaf};

    // Determine the carbon flux to use for the non-leaf organs (Mg / ha / hr)
    double const nonleaf_carbon_flux{
        canopy_assim > 0 ? canopy_assim : 0};

    // Calculate the rate of new stem production, accounting for respiratory
    // costs (Mg / ha / hr)
    double const net_assimilation_rate_stem{
        kStem > 0 ? resp(nonleaf_carbon_flux * kStem, mrc1, temp) : 0};

    // Calculate the rate of new root production, accounting for respiratory
    // costs (Mg / ha / hr)
    double const net_assimilation_rate_root{
        kRoot > 0 ? resp(nonleaf_carbon_flux * kRoot, mrc2, temp) : 0};

    // Calculate the rate of new rhizome production, accounting for respiratory
    // costs (Mg / ha / hr)
    double const net_assimilation_rate_rhizome{
        kRhizome > 0 ? resp(nonleaf_carbon_flux * kRhizome, mrc2, temp) : 0};

    // Calculate the rate of new grain production without any respiratory costs
    // (Mg / ha / hr)
    double const net_assimilation_rate_grain{
        kGrain > 0 ? nonleaf_carbon_flux * kGrain : 0};

    // Update the output quantity list
    update(net_assimilation_rate_leaf_op, net_assimilation_rate_leaf);
    update(net_assimilation_rate_stem_op, net_assimilation_rate_stem);
    update(net_assimilation_rate_root_op, net_assimilation_rate_root);
    update(net_assimilation_rate_rhizome_op, net_assimilation_rate_rhizome);
    update(net_assimilation_rate_grain_op, net_assimilation_rate_grain);
}

}  // namespace standardBML
#endif
