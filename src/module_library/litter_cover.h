#ifndef LITTER_COVER_H
#define LITTER_COVER_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class litter_cover
 *
 *  @brief Estimates the fraction of ground covered by leaf litter.
 *
 *  Here we just want a simple empirical way to estimate the fraction of ground
 *  area under the canopy that is covered by leaf litter, where we consider a
 *  small piece of ground to be "covered" if there is no direct line-of-sight
 *  through the litter from it. Ideally, the fraction would be 0 when there is
 *  no litter, and only reach its maximum value of 1 when there is a large
 *  amount of litter. A simple function having that property is a rectangular
 *  hyperbola:
 *
 *  > \f$ f_{litter} = \frac{M_{litter}}{M_{litter} + K_{litter}} \f$  [Equation `(1)`]
 *
 *  where \f$ f_{litter} \f$ is the fraction of the ground covered by litter,
 *  \f$ M_{litter} \f$ is the mass of litter per unit ground area, and
 *  \f$ K_{litter} \f$ is a constant.
 *
 *  This equation can be solved for \f$ M_{litter} \f$ to find the litter mass
 *  corresponding to a particular value of \f$ f_{litter} \f$:
 *
 *  > \f$ M_{litter} = K_{litter} \cdot \frac{1}{1 - f_{litter}} \f$
 *
 *  For example, 95% of the ground area is covered when \f$ M_{litter} = 19
 *  \cdot K_{litter} \f$. Thus, a simple way to choose a value for
 *  \f$ K_{litter} \f$ is to assume that 95% of the ground area is covered when
 *  the highest litter mass is observed during a particular growing season; in
 *  that case, \f$ K_{litter} = M_{litter}^{max} / 19 \f$, where
 *  \f$ M_{litter}^{max} \f$ is the maximum observed litter mass.
 *
 *  This module implements Equation `(1)`. For most crops modeled using BioCro,
 *  the vast majority of litter comes from senesced leaves, so the value of
 *  \f$ M_{litter} \f$ is set to the value of the ``'LeafLitter'`` BioCro
 *  quantity. \f$ K_{litter} \f$ and \f$ f_{litter} \f$ are called
 *  ``'km_leaf_litter'`` and ``'litter_cover_fraction'`` in BioCro,
 *  respectively.
 */
class litter_cover : public direct_module
{
   public:
    litter_cover(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          LeafLitter{get_input(input_quantities, "LeafLitter")},
          km_leaf_litter{get_input(input_quantities, "km_leaf_litter")},

          // Get pointers to output quantities
          lcf_op{get_op(output_quantities, "litter_cover_fraction")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "litter_cover"; }

   private:
    // References to input quantities
    double const& LeafLitter;
    double const& km_leaf_litter;

    // Pointers to output quantities
    double* lcf_op;

    // Main operation
    void do_operation() const;
};

string_vector litter_cover::get_inputs()
{
    return {
        "LeafLitter",     // Mg / ha
        "km_leaf_litter"  // Mg / ha
    };
}

string_vector litter_cover::get_outputs()
{
    return {
        "litter_cover_fraction"  // dimensionless
    };
}

void litter_cover::do_operation() const
{
    update(lcf_op, LeafLitter / (LeafLitter + km_leaf_litter));
}

}  // namespace standardBML
#endif
