#ifndef PARTITIONING_GROWTH_CALCULATOR_H
#define PARTITIONING_GROWTH_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "BioCro.h"  // for resp

namespace standardBML
{
/**
 *  @class partitioning_growth_calculator
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
 *  This module includes five organs:
 *  - `Leaf`: The leaf growth rate is modified by water stress and then
 *     respiration. Note that this effectively double-counts leaf respiration
 *     because the net canopy assimilation rate already includes it.
 *  - `Stem`: The stem growth rate is modified by respiration.
 *  - `Root`: The root growth rate is modified by respiration.
 *  - `Rhizome`: The rhizome growth rate is modified by respiration.
 *  - `Grain`: The grain growth rate is *not* modified by respiration and is not
 *     allowed to become negative, even when the canopy assimilation rate is
 *     negative.
 *
 *  Here it is assumed that the major effect of water stress on mass
 *  accumulation is a reduction in the leaf growth rate, following
 *  [Boyer, J. S. "Leaf Enlargement and Metabolic Rates in Corn, Soybean, and
 *  Sunflower at Various Leaf Water Potentials" Plant Physiology 46, 233–235 (1970)]
 *  (https://doi.org/10.1104/pp.46.2.233).
 */
class partitioning_growth_calculator : public direct_module
{
   public:
    partitioning_growth_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          kLeaf{get_input(input_quantities, "kLeaf")},
          kStem{get_input(input_quantities, "kStem")},
          kRoot{get_input(input_quantities, "kRoot")},
          kRhizome{get_input(input_quantities, "kRhizome")},
          kGrain{get_input(input_quantities, "kGrain")},
          canopy_assimilation_rate{get_input(input_quantities, "canopy_assimilation_rate")},
          LeafWS{get_input(input_quantities, "LeafWS")},
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
    static std::string get_name() { return "partitioning_growth_calculator"; }

   private:
    // Pointers to input quantities
    const double& kLeaf;
    const double& kStem;
    const double& kRoot;
    const double& kRhizome;
    const double& kGrain;
    const double& canopy_assimilation_rate;
    const double& LeafWS;
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

string_vector partitioning_growth_calculator::get_inputs()
{
    return {
        "kLeaf",                     // dimensionless
        "kStem",                     // dimensionless
        "kRoot",                     // dimensionless
        "kRhizome",                  // dimensionless
        "kGrain",                    // dimensionless
        "canopy_assimilation_rate",  // Mg / ha / hour
        "LeafWS",                    // dimensionless
        "mrc1",                      // dimensionless
        "mrc2",                      // dimensionless
        "temp"                       // degrees C
    };
}

string_vector partitioning_growth_calculator::get_outputs()
{
    return {
        "net_assimilation_rate_leaf",     // Mg / ha / hour
        "net_assimilation_rate_stem",     // Mg / ha / hour
        "net_assimilation_rate_root",     // Mg / ha / hour
        "net_assimilation_rate_rhizome",  // Mg / ha / hour
        "net_assimilation_rate_grain"     // Mg / ha / hour
    };
}

void partitioning_growth_calculator::do_operation() const
{
    double net_assimilation_rate_leaf{0.0};
    double net_assimilation_rate_stem{0.0};
    double net_assimilation_rate_root{0.0};
    double net_assimilation_rate_rhizome{0.0};
    double net_assimilation_rate_grain{0.0};

    // Calculate the rate of new leaf production
    if (kLeaf > 0) {
        net_assimilation_rate_leaf = canopy_assimilation_rate * kLeaf * LeafWS;
        net_assimilation_rate_leaf = resp(net_assimilation_rate_leaf, mrc1, temp);
    } else {
        net_assimilation_rate_leaf = 0.0;
    }

    // Calculate the rate of new stem production
    if (kStem >= 0) {
        net_assimilation_rate_stem = canopy_assimilation_rate * kStem;
        net_assimilation_rate_stem = resp(net_assimilation_rate_stem, mrc1, temp);
    } else {
        net_assimilation_rate_stem = 0.0;
    }

    // Calculate the rate of new root production
    if (kRoot > 0) {
        net_assimilation_rate_root = canopy_assimilation_rate * kRoot;
        net_assimilation_rate_root = resp(net_assimilation_rate_root, mrc2, temp);
    } else {
        net_assimilation_rate_root = 0.0;
    }

    // Calculate the rate of new rhizome production
    if (kRhizome > 0) {
        net_assimilation_rate_rhizome = canopy_assimilation_rate * kRhizome;
        net_assimilation_rate_rhizome = resp(net_assimilation_rate_rhizome, mrc2, temp);
    } else {
        net_assimilation_rate_rhizome = 0.0;
    }

    // Calculate the rate of grain production
    if (kGrain > 0 && canopy_assimilation_rate > 0) {
        net_assimilation_rate_grain = canopy_assimilation_rate * kGrain;
    } else {
        net_assimilation_rate_grain = 0.0;
    }

    // Update the output quantity list
    update(net_assimilation_rate_leaf_op, net_assimilation_rate_leaf);
    update(net_assimilation_rate_stem_op, net_assimilation_rate_stem);
    update(net_assimilation_rate_root_op, net_assimilation_rate_root);
    update(net_assimilation_rate_rhizome_op, net_assimilation_rate_rhizome);
    update(net_assimilation_rate_grain_op, net_assimilation_rate_grain);
}

}  // namespace standardBML
#endif
